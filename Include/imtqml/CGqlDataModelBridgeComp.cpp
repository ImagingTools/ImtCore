// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CGqlDataModelBridgeComp.h>


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QMutexLocker>

// ImtCore includes
#include <imtclientgql/IAsyncGqlResponseHandler.h>
#include <imtclientgql/IAsyncGqlRequestToken.h>
#include <imtgql/CGqlParamObject.h>
#include <imtgql/CGqlFieldObject.h>


namespace imtqml
{


namespace
{


/**
	\brief Schedule \a fn on the GUI thread via the global
	\c QCoreApplication, so that bridge callbacks always fire on the
	GUI thread regardless of which network / WebSocket thread produced
	the result (\c IDataModelBridge contract).
*/
template<class Fn>
void PostToMainThread(Fn&& fn)
{
	QCoreApplication* appPtr = QCoreApplication::instance();
	if (appPtr == nullptr){
		// In headless / unit-test scenarios fall back to a direct call
		// so that the contract "callback fires exactly once" still
		// holds.
		fn();
		return;
	}
	QMetaObject::invokeMethod(appPtr, std::forward<Fn>(fn), Qt::QueuedConnection);
}


/**
	\brief Self-deleting response sink for the asynchronous GraphQL
	client.

	\details
	\c IAsyncGqlResponseHandler is invoked exactly once per request
	(success or error, including cancellation), so the handler owns
	itself: it marshals the result to the GUI thread and deletes
	itself afterwards. It intentionally holds no pointer back to the
	bridge component — destroying the bridge while requests are in
	flight is safe.
*/
class CSelfDeletingResponseHandler: virtual public imtclientgql::IAsyncGqlResponseHandler
{
public:
	typedef std::function<void(const QByteArray& /*responseData*/, const QString& /*errorMessage*/)> ResultCallback;

	explicit CSelfDeletingResponseHandler(ResultCallback callback)
		:m_callback(std::move(callback))
	{
	}

	// reimplemented (imtclientgql::IAsyncGqlResponseHandler)
	virtual void OnResponseReceived(GqlResponsePtr responsePtr) override
	{
		QByteArray responseData;
		if (responsePtr.IsValid()){
			responseData = responsePtr->GetResponseData();
		}

		ResultCallback callback = std::move(m_callback);
		PostToMainThread([callback, responseData](){
			callback(responseData, QString{});
		});

		delete this;
	}

	virtual void OnError(ErrorCategory category, const QString& message) override
	{
		Q_UNUSED(category);

		QString errorMessage = message.isEmpty() ?
				QStringLiteral("GraphQL request failed") : message;

		ResultCallback callback = std::move(m_callback);
		PostToMainThread([callback, errorMessage](){
			callback(QByteArray{}, errorMessage);
		});

		delete this;
	}

private:
	ResultCallback m_callback;
};


void InsertParamValue(imtgql::CGqlParamObject& paramObject, const QByteArray& paramId, const QVariant& value);


void FillParamObject(imtgql::CGqlParamObject& paramObject, const QVariantMap& values)
{
	for (auto iter = values.constBegin(); iter != values.constEnd(); ++iter){
		InsertParamValue(paramObject, iter.key().toUtf8(), iter.value());
	}
}


void InsertParamValue(imtgql::CGqlParamObject& paramObject, const QByteArray& paramId, const QVariant& value)
{
	if (value.typeId() == QMetaType::QVariantMap){
		imtgql::CGqlParamObject* nestedObjectPtr = paramObject.CreateParamObject(paramId);
		if (nestedObjectPtr != nullptr){
			FillParamObject(*nestedObjectPtr, value.toMap());
		}
		return;
	}

	if (value.typeId() == QMetaType::QVariantList){
		const QVariantList list = value.toList();
		if (!list.isEmpty() && list.first().typeId() == QMetaType::QVariantMap){
			for (const QVariant& element: list){
				imtgql::CGqlParamObject elementObject;
				FillParamObject(elementObject, element.toMap());
				paramObject.AppendParamToArray(paramId, elementObject);
			}
			return;
		}
	}

	paramObject.InsertParam(paramId, value);
}


/**
	\brief Selection-set node built from the dotted field paths of the
	\c Fields / \c MutationFields attributes (e.g. \c address.city).
*/
struct FieldNode
{
	QMap<QByteArray, FieldNode> children;
};


void FillFieldObject(imtgql::CGqlFieldObject& fieldObject, const QMap<QByteArray, FieldNode>& children)
{
	for (auto iter = children.constBegin(); iter != children.constEnd(); ++iter){
		if (iter->children.isEmpty()){
			fieldObject.InsertField(iter.key());
			continue;
		}

		imtgql::CGqlFieldObject* childObjectPtr = fieldObject.CreateFieldObject(iter.key());
		if (childObjectPtr != nullptr){
			FillFieldObject(*childObjectPtr, iter->children);
		}
	}
}


/**
	\brief Insert the (possibly dotted) field paths into the selection
	set of \a request, merging paths with a common prefix into one
	nested object (e.g. \c address.city + \c address.zip).
*/
void AddFieldPaths(imtgql::CGqlRequest& request, const QByteArrayList& fieldPaths)
{
	QMap<QByteArray, FieldNode> rootNodes;
	for (const QByteArray& fieldPath: fieldPaths){
		const QByteArrayList parts = fieldPath.split('.');
		FieldNode* nodePtr = &rootNodes[parts.first()];
		for (int i = 1; i < parts.size(); ++i){
			nodePtr = &nodePtr->children[parts[i]];
		}
	}

	for (auto iter = rootNodes.constBegin(); iter != rootNodes.constEnd(); ++iter){
		if (iter->children.isEmpty()){
			request.AddSimpleField(iter.key());
			continue;
		}

		imtgql::CGqlFieldObject fieldObject;
		FillFieldObject(fieldObject, iter->children);
		request.AddField(iter.key(), fieldObject);
	}
}


} // anonymous namespace


// public methods

// reimplemented (IDataModelBridge)

void CGqlDataModelBridgeComp::GetModel(
		const QString& modelId,
		const QVariantMap& parameters,
		GetModelCallback callback)
{
	Q_UNUSED(modelId);

	if (!callback){
		return;
	}

	if (!m_gqlClientCompPtr.IsValid()){
		callback(QVariant{}, QStringLiteral(
				"CGqlDataModelBridgeComp: no GraphQL client attached"));
		return;
	}

	const QByteArray commandId = m_queryCommandIdAttrPtr.IsValid() ?
			*m_queryCommandIdAttrPtr : QByteArray{};
	if (commandId.isEmpty()){
		callback(QVariant{}, QStringLiteral(
				"CGqlDataModelBridgeComp: QueryCommandId is not configured"));
		return;
	}

	imtgql::CGqlRequest* gqlRequestPtr =
			new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	SetupRequest(*gqlRequestPtr, parameters, GetQueryFieldPaths());
	imtclientgql::IAsyncGqlClient::GqlRequestPtr requestPtr(gqlRequestPtr);

	auto* handlerPtr = new CSelfDeletingResponseHandler(
			[callback, commandId](const QByteArray& responseData, const QString& errorMessage){
				if (!errorMessage.isEmpty()){
					callback(QVariant{}, errorMessage);
					return;
				}

				QVariantMap payload;
				QString decodeError;
				if (!DecodePayload(responseData, commandId, payload, decodeError)){
					callback(QVariant{}, decodeError);
					return;
				}

				callback(QVariant::fromValue(payload), QString{});
			});

	m_gqlClientCompPtr->SendRequest(requestPtr, handlerPtr);
}


void CGqlDataModelBridgeComp::SetModel(
		const QString& modelId,
		const QVariantMap& parameters,
		const QVariant& model,
		SetModelCallback callback)
{
	Q_UNUSED(modelId);

	if (!m_gqlClientCompPtr.IsValid()){
		if (callback){
			callback(QStringLiteral(
					"CGqlDataModelBridgeComp: no GraphQL client attached"));
		}
		return;
	}

	const QByteArray commandId = m_mutationCommandIdAttrPtr.IsValid() ?
			*m_mutationCommandIdAttrPtr : QByteArray{};
	if (commandId.isEmpty()){
		if (callback){
			callback(QStringLiteral(
					"CGqlDataModelBridgeComp: MutationCommandId is not configured"));
		}
		return;
	}

	// The mutation input is the merge of the routing parameters
	// (e.g. object ID) and the edited ViewModel values.
	QVariantMap inputValues = parameters;
	const QVariantMap modelValues = model.toMap();
	for (auto iter = modelValues.constBegin(); iter != modelValues.constEnd(); ++iter){
		inputValues.insert(iter.key(), iter.value());
	}

	imtgql::CGqlRequest* gqlRequestPtr =
			new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_MUTATION, commandId);
	SetupRequest(*gqlRequestPtr, inputValues, GetMutationFieldPaths());
	imtclientgql::IAsyncGqlClient::GqlRequestPtr requestPtr(gqlRequestPtr);

	auto* handlerPtr = new CSelfDeletingResponseHandler(
			[callback, commandId](const QByteArray& responseData, const QString& errorMessage){
				if (!callback){
					return;
				}

				if (!errorMessage.isEmpty()){
					callback(errorMessage);
					return;
				}

				QVariantMap payload;
				QString decodeError;
				if (!DecodePayload(responseData, commandId, payload, decodeError)){
					callback(decodeError);
					return;
				}

				callback(QString{});
			});

	m_gqlClientCompPtr->SendRequest(requestPtr, handlerPtr);
}


int CGqlDataModelBridgeComp::SubscribeModel(
		const QString& modelId,
		const QVariantMap& parameters,
		ModelUpdateCallback callback)
{
	Q_UNUSED(modelId);

	if (!callback){
		return 0;
	}

	if (!m_subscriptionManagerCompPtr.IsValid()){
		return 0;
	}

	const QByteArray commandId = m_subscriptionCommandIdAttrPtr.IsValid() ?
			*m_subscriptionCommandIdAttrPtr : QByteArray{};
	if (commandId.isEmpty()){
		return 0;
	}

	const QByteArray signature = CreateSubscriptionSignature(commandId, parameters);

	QByteArray wireSubscriptionId;
	{
		QMutexLocker locker(&m_subscriptionsMutex);
		wireSubscriptionId = m_wireSubscriptionIds.value(signature);
	}

	if (wireSubscriptionId.isEmpty()){
		// Register this bridge at the subscription manager exactly
		// once per distinct request — the manager appends the client
		// per registration and would otherwise dispatch duplicates.
		imtgql::CGqlRequest subscriptionRequest(imtgql::IGqlRequest::RT_SUBSCRIPTION, commandId);
		SetupRequest(subscriptionRequest, parameters, GetQueryFieldPaths());

		wireSubscriptionId =
				m_subscriptionManagerCompPtr->RegisterSubscription(subscriptionRequest, this);
		if (wireSubscriptionId.isEmpty()){
			return 0;
		}
	}

	QMutexLocker locker(&m_subscriptionsMutex);

	m_wireSubscriptionIds.insert(signature, wireSubscriptionId);

	int subscriptionId = m_nextSubscriptionId++;
	m_subscriptions.insert(subscriptionId, Subscription{wireSubscriptionId, callback});

	return subscriptionId;
}


void CGqlDataModelBridgeComp::UnsubscribeModel(int subscriptionId)
{
	QMutexLocker locker(&m_subscriptionsMutex);

	auto iter = m_subscriptions.find(subscriptionId);
	if (iter == m_subscriptions.end()){
		return;
	}

	const QByteArray wireSubscriptionId = iter->wireSubscriptionId;
	m_subscriptions.erase(iter);

	// The subscription manager shares one wire subscription between
	// identical requests — only unregister when no other local
	// subscriber uses it.
	for (const Subscription& subscription: std::as_const(m_subscriptions)){
		if (subscription.wireSubscriptionId == wireSubscriptionId){
			return;
		}
	}

	for (auto signatureIter = m_wireSubscriptionIds.begin(); signatureIter != m_wireSubscriptionIds.end();){
		if (signatureIter.value() == wireSubscriptionId){
			signatureIter = m_wireSubscriptionIds.erase(signatureIter);
		}
		else{
			++signatureIter;
		}
	}

	locker.unlock();

	if (m_subscriptionManagerCompPtr.IsValid()){
		m_subscriptionManagerCompPtr->UnregisterSubscription(wireSubscriptionId);
	}
}


// protected methods

// reimplemented (imtclientgql::IGqlSubscriptionClient)

void CGqlDataModelBridgeComp::OnResponseReceived(
		const QByteArray& subscriptionId,
		const QByteArray& subscriptionData)
{
	const QByteArray commandId = m_subscriptionCommandIdAttrPtr.IsValid() ?
			*m_subscriptionCommandIdAttrPtr : QByteArray{};

	QVariantMap payload;
	QString decodeError;
	if (!DecodePayload(subscriptionData, commandId, payload, decodeError)){
		qWarning("CGqlDataModelBridgeComp: dropping subscription update: %s",
				qPrintable(decodeError));
		return;
	}

	QList<ModelUpdateCallback> callbacks;
	{
		QMutexLocker locker(&m_subscriptionsMutex);
		for (const Subscription& subscription: std::as_const(m_subscriptions)){
			if (subscription.wireSubscriptionId == subscriptionId){
				callbacks.append(subscription.callback);
			}
		}
	}

	if (callbacks.isEmpty()){
		return;
	}

	QVariant model = QVariant::fromValue(payload);
	PostToMainThread([callbacks, model](){
		for (const ModelUpdateCallback& callback: callbacks){
			callback(model);
		}
	});
}


void CGqlDataModelBridgeComp::OnSubscriptionStatusChanged(
		const QByteArray& subscriptionId,
		const SubscriptionStatus& status,
		const QString& message)
{
	Q_UNUSED(subscriptionId);

	if (status == SS_UNREGISTERED && !message.isEmpty()){
		qWarning("CGqlDataModelBridgeComp: subscription unregistered: %s",
				qPrintable(message));
	}
}


// reimplemented (icomp::CComponentBase)

void CGqlDataModelBridgeComp::OnComponentDestroyed()
{
	QByteArrayList wireSubscriptionIds;
	{
		QMutexLocker locker(&m_subscriptionsMutex);
		for (const Subscription& subscription: std::as_const(m_subscriptions)){
			if (!wireSubscriptionIds.contains(subscription.wireSubscriptionId)){
				wireSubscriptionIds.append(subscription.wireSubscriptionId);
			}
		}
		m_subscriptions.clear();
		m_wireSubscriptionIds.clear();
	}

	if (m_subscriptionManagerCompPtr.IsValid()){
		for (const QByteArray& wireSubscriptionId: std::as_const(wireSubscriptionIds)){
			m_subscriptionManagerCompPtr->UnregisterSubscription(wireSubscriptionId);
		}
	}

	BaseClass::OnComponentDestroyed();
}


// private methods

QByteArray CGqlDataModelBridgeComp::CreateSubscriptionSignature(
		const QByteArray& commandId,
		const QVariantMap& parameters)
{
	return commandId + '|' + QJsonDocument(QJsonObject::fromVariantMap(parameters))
			.toJson(QJsonDocument::Compact);
}


void CGqlDataModelBridgeComp::SetupRequest(
		imtgql::CGqlRequest& request,
		const QVariantMap& inputValues,
		const QByteArrayList& fieldPaths) const
{
	if (!inputValues.isEmpty()){
		imtgql::CGqlParamObject inputObject;
		FillParamObject(inputObject, inputValues);
		request.AddParam("input", inputObject);
	}

	AddFieldPaths(request, fieldPaths);
}


QByteArrayList CGqlDataModelBridgeComp::GetQueryFieldPaths() const
{
	QByteArrayList fieldPaths;
	for (int i = 0; i < m_fieldsAttrPtr.GetCount(); ++i){
		fieldPaths.append(m_fieldsAttrPtr[i]);
	}

	return fieldPaths;
}


QByteArrayList CGqlDataModelBridgeComp::GetMutationFieldPaths() const
{
	QByteArrayList fieldPaths;
	for (int i = 0; i < m_mutationFieldsAttrPtr.GetCount(); ++i){
		fieldPaths.append(m_mutationFieldsAttrPtr[i]);
	}

	if (fieldPaths.isEmpty()){
		fieldPaths.append(QByteArrayLiteral("success"));
	}

	return fieldPaths;
}


bool CGqlDataModelBridgeComp::DecodePayload(
		const QByteArray& responseData,
		const QByteArray& commandId,
		QVariantMap& payload,
		QString& errorMessage)
{
	QJsonParseError parseError;
	QJsonDocument document = QJsonDocument::fromJson(responseData, &parseError);
	if (!document.isObject()){
		errorMessage = QStringLiteral("Invalid GraphQL response: %1")
				.arg(parseError.error != QJsonParseError::NoError ?
						parseError.errorString() : QStringLiteral("not a JSON object"));
		return false;
	}

	QJsonObject rootObject = document.object();

	if (rootObject.contains(QStringLiteral("errors"))){
		QString message;
		const QJsonValue errorsValue = rootObject.value(QStringLiteral("errors"));
		if (errorsValue.isArray() && !errorsValue.toArray().isEmpty()){
			message = errorsValue.toArray().first().toObject()
					.value(QStringLiteral("message")).toString();
		}
		errorMessage = message.isEmpty() ?
				QStringLiteral("GraphQL request failed") : message;
		return false;
	}

	QJsonObject dataObject = rootObject;
	if (dataObject.contains(QStringLiteral("data"))){
		dataObject = dataObject.value(QStringLiteral("data")).toObject();
	}

	const QString commandKey = QString::fromUtf8(commandId);
	if (!commandKey.isEmpty() && dataObject.contains(commandKey)){
		dataObject = dataObject.value(commandKey).toObject();
	}

	payload = dataObject.toVariantMap();

	return true;
}


} // namespace imtqml
