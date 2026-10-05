// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CWebSocketSubscriptionManagerComp.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QUuid>
#include <QtCore/QUrl>

// ImtCore includes
#include <imtbase/imtbase.h>


namespace imtclientgql
{


// public methods

CWebSocketSubscriptionManagerComp::CWebSocketSubscriptionManagerComp()
	:m_accessTokenObserver(*this)
{
}


// reimplemented (imtclientgql::IGqlSubscriptionManager)

QByteArray CWebSocketSubscriptionManagerComp::RegisterSubscription(
			const imtgql::IGqlRequest& subscriptionRequest,
			IGqlSubscriptionClient& subscriptionClient)
{
	if (!m_connectionStatusProviderCompPtr.IsValid()){
		return QByteArray();
	}

	auto requestImplPtr = dynamic_cast<const imtgql::CGqlRequest*>(&subscriptionRequest);
	if (requestImplPtr == nullptr){
		SendWarningMessage(0, "Unexpected subscription request");

		return QByteArray();
	}

	QByteArray clientId = requestImplPtr->GetHeader("clientid");

	QMutexLocker locker(&m_registeredClientsMutex);

	for (QByteArray subscriptionId : m_registeredClients.keys()){
		if (m_registeredClients[subscriptionId].m_request.IsEqual(subscriptionRequest) && m_registeredClients[subscriptionId].m_clientId == clientId){
			m_registeredClients[subscriptionId].m_clients.append(&subscriptionClient);

			return subscriptionId;
		}
	}

	QByteArray subscriptionId = QUuid::createUuid().toString(QUuid::WithoutBraces).toLocal8Bit();

	SubscriptionHelper subscriptionHelper;
	subscriptionHelper.m_request = *requestImplPtr;
	subscriptionHelper.m_clientId = clientId;
	subscriptionHelper.m_status = IGqlSubscriptionClient::SS_IN_REGISTRATION;
	subscriptionHelper.m_clients.append(&subscriptionClient);
	m_registeredClients.insert(subscriptionId, subscriptionHelper);

	locker.unlock();

	if (!SubscriptionRegister(*requestImplPtr, subscriptionId)){
		QMutexLocker failedRegistrationLocker(&m_registeredClientsMutex);
		m_registeredClients.remove(subscriptionId);

		return QByteArray();
	}

	if (m_connectionStatusProviderCompPtr->GetConnectionStatus() == imtcom::IConnectionStatusProvider::CS_CONNECTED){
		{
			QMutexLocker registeredLocker(&m_registeredClientsMutex);
			if (m_registeredClients.contains(subscriptionId)){
				m_registeredClients[subscriptionId].m_status = IGqlSubscriptionClient::SS_REGISTERED;
			}
		}

		UpdateCustomerSubscriptionStatuses(subscriptionId);
	}

	return subscriptionId;
}


bool CWebSocketSubscriptionManagerComp::UnregisterSubscription(
			const QByteArray& subscriptionId,
			const imtclientgql::IGqlSubscriptionClient& subscriptionClient)
{
	QMutexLocker locker(&m_registeredClientsMutex);

	if (m_registeredClients.contains(subscriptionId)){
		m_registeredClients[subscriptionId].m_clients.removeAll(const_cast<imtclientgql::IGqlSubscriptionClient*>(&subscriptionClient));
		if (m_registeredClients[subscriptionId].m_clients.isEmpty()){
			m_registeredClients.remove(subscriptionId);
		}

		return true;
	}

	return false;
}


// reimplemented (imod::CSingleModelObserverBase)

void CWebSocketSubscriptionManagerComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (!m_connectionStatusProviderCompPtr.IsValid()){
		return;
	}

	QMutexLocker locker(&m_registeredClientsMutex);

	QByteArray clientId = changeSet.GetChangeInfo("ClientId").toByteArray();
	QByteArrayList subscriptionIds = m_registeredClients.keys();

	for (const QByteArray& subscriptionId : subscriptionIds){
		if (changeSet.Contains(imtcom::IConnectionStatusProvider::CF_CONNECTED)){
			if (m_registeredClients[subscriptionId].m_clientId == clientId){
				const bool isRegistered = SubscriptionRegister(m_registeredClients[subscriptionId].m_request, subscriptionId);
				m_registeredClients[subscriptionId].m_status = isRegistered
						? IGqlSubscriptionClient::SS_REGISTERED
						: IGqlSubscriptionClient::SS_IN_REGISTRATION;
			}
		}
		else{
			if (m_registeredClients[subscriptionId].m_clientId == clientId){
				m_registeredClients[subscriptionId].m_status = IGqlSubscriptionClient::SS_IN_REGISTRATION;
			}
		}
	}

	locker.unlock();

	for (const QByteArray& subscriptionId : subscriptionIds){
		UpdateCustomerSubscriptionStatuses(subscriptionId);
	}
}


// reimplemented (imtrest::ISubscriptionMessageHandler)

imtrest::ConstResponsePtr CWebSocketSubscriptionManagerComp::ProcessMessage(
			const QByteArray& subscriptionId,
			const QByteArray& methodType,
			const QByteArray& message) const
{
	QMutexLocker locker(&m_registeredClientsMutex);

	QJsonParseError jsonError;
	QJsonDocument jsonDocument = QJsonDocument::fromJson(message, &jsonError);
	if (jsonDocument.isNull()){
		QByteArray errorMessage = QStringLiteral("Unable to convert message to JSON: '%1'").arg(jsonError.errorString()).toUtf8();

		locker.unlock();
		SendErrorMessage(0, QString(errorMessage));

		return imtrest::ConstResponsePtr();
	}

	QJsonObject rootObject = jsonDocument.object();

	if (methodType == "connection_ack"){
		for (const QByteArray& registeredSubscriptionId : m_registeredClients.keys()){
			if (m_registeredClients[registeredSubscriptionId].m_status == IGqlSubscriptionClient::SS_IN_REGISTRATION){
				istd::IChangeableUniquePtr objectPtr(m_registeredClients[registeredSubscriptionId].m_request.CloneMe());
				locker.unlock();
				auto requestPtr = dynamic_cast<imtgql::CGqlRequest*>(objectPtr.GetPtr());
				if (requestPtr != nullptr){
					SubscriptionRegister(*requestPtr, registeredSubscriptionId);
				}
				locker.relock();
			}
		}
	}
	else if (methodType == "start_ack"){
		if (m_registeredClients.contains(subscriptionId)){
			m_registeredClients[subscriptionId].m_status = IGqlSubscriptionClient::SS_REGISTERED;

			UpdateCustomerSubscriptionStatuses(subscriptionId, QString::fromUtf8(message));
		}
	}
	else if (methodType == "data"){
		if (m_registeredClients.contains(subscriptionId)){
			for (IGqlSubscriptionClient* subscriptionClientPtr : m_registeredClients[subscriptionId].m_clients){
				if (subscriptionClientPtr != nullptr){
					if (!rootObject.contains("payload")){
						break;
					}

					QJsonObject payloadObject = rootObject.value("payload").toObject().value("data").toObject();

					QJsonDocument document;
					document.setObject(payloadObject);

					QByteArray payload = document.toJson(QJsonDocument::Compact);

					locker.unlock();
					subscriptionClientPtr->OnResponseReceived(subscriptionId, payload);
					locker.relock();
				}
			}
		}
	}
	else{
		QByteArray errorMessage = QStringLiteral("Method type not correct: %1").arg(QString::fromUtf8(methodType)).toUtf8();

		locker.unlock();
		SendErrorMessage(0, QString(errorMessage));

		return imtrest::ConstResponsePtr();
	}

	return imtrest::ConstResponsePtr();
}


// protected methods

bool CWebSocketSubscriptionManagerComp::SubscriptionRegister(const imtgql::CGqlRequest& subscriptionRequest, const QByteArray& subscriptionId) const
{
	if (!m_engineCompPtr.IsValid()){
		Q_ASSERT(0);

		return false;
	}

	QString authToken;

	QByteArray endpoint;
	QUrl url(endpoint);
	QString host = url.host();

	QJsonObject authorization;
	authorization["Authorization"] = authToken;
	authorization["host"] = host;

	QJsonObject extensions;
	extensions["authorization"] = authorization;

	QJsonObject payload;
	payload["data"] = QString(subscriptionRequest.GetQuery());
	payload["extensions"] = extensions;

	QJsonObject registerSubscription;
	registerSubscription["id"] = QString(subscriptionId);
	registerSubscription["type"] = "start";
	registerSubscription["payload"] = payload;

	QJsonObject headersObject;
	const imtgql::IGqlContext* contextPtr = subscriptionRequest.GetRequestContext();
	if (contextPtr != nullptr){
		imtgql::IGqlContext::Headers headers = contextPtr->GetHeaders();
		for (const QByteArray& headerId : headers.keys()){
			if (headerId != "accept-encoding"){
				headersObject[headerId] = QString(headers.value(headerId));
			}
		}

		const QByteArray languageId = contextPtr->GetLanguageId();
		if (!languageId.isEmpty()){
			headersObject[QString(imtbase::s_languageIdHeaderId)] = QString(languageId);
		}
	}

	// The request context is a clone taken when the subscription was created, so
	// its token is the one that was current back then. The server authenticates
	// every registration, so a re-registration after a refresh has to present the
	// token that is current now, not the captured one.
	if (m_accessTokenProviderCompPtr.IsValid()){
		const QByteArray accessToken = m_accessTokenProviderCompPtr->GetToken(QByteArray());
		const QString authenticationTokenHeaderId(imtbase::s_authenticationTokenHeaderId);
		if (accessToken.isEmpty()){
			headersObject.remove(authenticationTokenHeaderId);
		}
		else{
			headersObject[authenticationTokenHeaderId] = QString(accessToken);
		}
	}

	registerSubscription["headers"] = headersObject;

	QByteArray queryData = QJsonDocument(registerSubscription).toJson(QJsonDocument::Compact);

	imtrest::ConstRequestPtr requestPtr(m_engineCompPtr->CreateRequestForSend(m_requestServletStub, 0, queryData, "").PopInterfacePtr());

	return SendRequestInternal(subscriptionRequest, requestPtr);
}


bool CWebSocketSubscriptionManagerComp::SubscriptionUnregister(const imtgql::CGqlRequest& subscriptionRequest, const QByteArray& subscriptionId) const
{
	if (!m_engineCompPtr.IsValid()){
		Q_ASSERT(0);

		return false;
	}

	QJsonObject unregisterSubscription;
	unregisterSubscription["id"] = QString(subscriptionId);
	unregisterSubscription["type"] = "stop";
	unregisterSubscription["payload"] = QJsonObject();

	QJsonObject headersObject;
	const imtgql::IGqlContext* contextPtr = subscriptionRequest.GetRequestContext();
	if (contextPtr != nullptr){
		imtgql::IGqlContext::Headers headers = contextPtr->GetHeaders();
		for (const QByteArray& headerId : headers.keys()){
			if (headerId != "accept-encoding"){
				headersObject[headerId] = QString(headers.value(headerId));
			}
		}
	}

	unregisterSubscription["headers"] = headersObject;

	QByteArray queryData = QJsonDocument(unregisterSubscription).toJson(QJsonDocument::Compact);

	imtrest::ConstRequestPtr requestPtr(m_engineCompPtr->CreateRequestForSend(m_requestServletStub, 0, queryData, "").PopInterfacePtr());

	return SendRequestInternal(subscriptionRequest, requestPtr);
}


bool CWebSocketSubscriptionManagerComp::SendRequestInternal(const imtgql::IGqlRequest& request, imtrest::ConstRequestPtr& requestPtr) const
{
	bool retVal = false;
	QByteArray clientId;

	auto requestImplPtr = dynamic_cast<const imtgql::CGqlRequest*>(&request);
	if (requestImplPtr != nullptr){
		clientId = requestImplPtr->GetHeader("clientid");
	}

	if (m_subscriptionSenderCompPtr.IsValid()){
		QByteArray data = requestPtr->GetBody();
		retVal = m_subscriptionSenderCompPtr->SendData(data);
	}
	else if (m_requestManagerCompPtr.IsValid()){
		if (clientId.isEmpty()){
			SendErrorMessage(
				0,
				QStringLiteral("Outbound WebSocket request has empty clientid — cannot route to recipient"),
				"SubscriptionManager");
			return false;
		}
		retVal = m_requestManagerCompPtr->SendRequest(clientId, requestPtr);
		if (!retVal){
			SendErrorMessage(
				0,
				QStringLiteral("No WebSocket sender registered for clientid '%1' (client offline or id mismatch)")
				.arg(clientId),
				"SubscriptionManager");
		}
	}

	return retVal;
}


// reimplemented (icomp::CComponentBase)

void CWebSocketSubscriptionManagerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_connectionStatusProviderModelCompPtr.IsValid()){
		m_connectionStatusProviderModelCompPtr->AttachObserver(this);
	}

	if (m_accessTokenProviderModelCompPtr.IsValid()){
		m_accessTokenProviderModelCompPtr->AttachObserver(&m_accessTokenObserver);
	}
}


void CWebSocketSubscriptionManagerComp::ReregisterSubscriptions() const
{
	QByteArrayList subscriptionIds;
	{
		QMutexLocker locker(&m_registeredClientsMutex);
		subscriptionIds = m_registeredClients.keys();
	}

	for (const QByteArray& subscriptionId : subscriptionIds){
		imtgql::CGqlRequest request;
		{
			QMutexLocker locker(&m_registeredClientsMutex);
			if (!m_registeredClients.contains(subscriptionId)){
				continue;
			}

			request = m_registeredClients[subscriptionId].m_request;
		}

		// Sent outside the lock: registration goes through the transport and
		// may re-enter this component.
		// The "stop" is sent first, because server side controllers append a new
		// registration for a repeated "start" instead of replacing the old one.
		// The transport keeps the message order, so the server drops the previous
		// registration before the new one arrives.
		SubscriptionUnregister(request, subscriptionId);
		SubscriptionRegister(request, subscriptionId);
	}
}


// public methods of the embedded class AccessTokenObserver

CWebSocketSubscriptionManagerComp::AccessTokenObserver::AccessTokenObserver(CWebSocketSubscriptionManagerComp& parent)
	:m_parent(parent)
{
}


// protected methods of the embedded class AccessTokenObserver

// reimplemented (imod::CSingleModelObserverBase)

void CWebSocketSubscriptionManagerComp::AccessTokenObserver::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_parent.ReregisterSubscriptions();
}


// public methods of the embedded class RequestServletStub

// reimplemented (imtrest::IRequestServlet)

bool CWebSocketSubscriptionManagerComp::RequestServletStub::IsCommandSupported(const QByteArray& /*commandId*/) const
{
	return true;
}


imtrest::ConstResponsePtr CWebSocketSubscriptionManagerComp::RequestServletStub::ProcessRequest(const imtrest::IRequest& /*request*/, const QByteArray& /*subCommandId*/) const
{
	return imtrest::ConstResponsePtr();
}


// private methods

void CWebSocketSubscriptionManagerComp::UpdateCustomerSubscriptionStatuses(const QByteArray& subscriptionId, const QString& message) const
{
	QList<IGqlSubscriptionClient*> clients;

	{
		QMutexLocker lock(&m_registeredClientsMutex);
		if (m_registeredClients.contains(subscriptionId)){
			clients = m_registeredClients[subscriptionId].m_clients;
		}
	}

	for (IGqlSubscriptionClient* subscriptionClientPtr : clients){
		if (subscriptionClientPtr != nullptr){
			subscriptionClientPtr->OnSubscriptionStatusChanged(subscriptionId, m_registeredClients[subscriptionId].m_status, message);
		}
	}
}


} // namespace imtclientgql
