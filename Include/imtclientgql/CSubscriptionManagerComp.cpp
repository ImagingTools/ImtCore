// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CSubscriptionManagerComp.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QMessageAuthenticationCode>
#include <QtNetwork/QNetworkRequest>
#include <QtCore/QUrl>

// ImtCore includes
#include <imtrest/CWebSocketRequest.h>
#include <imtgql/CGqlResponse.h>


namespace imtclientgql
{


// public methods

// reimplemented (imtgql::IGqlSubscriptionManager)

QByteArray CSubscriptionManagerComp::RegisterSubscription(
			const imtgql::IGqlRequest& subscriptionRequest,
			IGqlSubscriptionClient* subscriptionClient)
{
	if (!m_connectionStatusProviderCompPtr.IsValid()){
		return QByteArray();
	}

	if (!subscriptionClient){
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
			m_registeredClients[subscriptionId].m_clients.append(subscriptionClient);

			return subscriptionId;
		}
	}

	QString subscriptionId = QUuid::createUuid().toString(QUuid::WithoutBraces);

	SubscriptionHelper subscriptionHelper;
	subscriptionHelper.m_request = *requestImplPtr;
	subscriptionHelper.m_clientId = clientId;
	subscriptionHelper.m_status = IGqlSubscriptionClient::SS_IN_REGISTRATION;
	subscriptionHelper.m_clients.append(subscriptionClient);
	m_registeredClients.insert(subscriptionId.toLocal8Bit(), subscriptionHelper);

	locker.unlock();

	SubscriptionRegister(*requestImplPtr, subscriptionId.toLocal8Bit());

	return subscriptionId.toLocal8Bit();
}


bool CSubscriptionManagerComp::UnregisterSubscription(const QByteArray& subscriptionId)
{
	QMutexLocker locker(&m_registeredClientsMutex);

	if (m_registeredClients.contains(subscriptionId)){
		m_registeredClients.remove(subscriptionId);

		return true;
	}

	return false;
}


void CSubscriptionManagerComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (!m_connectionStatusProviderCompPtr.IsValid()){
		return;
	}

	QMutexLocker locker(&m_registeredClientsMutex);

	QByteArray clientId = changeSet.GetChangeInfo("ClientId").toByteArray();

	for (const QByteArray& subscriptionId : m_registeredClients.keys()){
		if (!changeSet.Contains(imtcom::IConnectionStatusProvider::CS_CONNECTED)){
			if (m_registeredClients[subscriptionId].m_clientId == clientId){
				m_registeredClients[subscriptionId].m_status = IGqlSubscriptionClient::SS_IN_REGISTRATION;
			}
		}
		else{
			if (m_registeredClients[subscriptionId].m_clientId == clientId){
				SubscriptionRegister(m_registeredClients[subscriptionId].m_request, subscriptionId);
			}
		}
	}
}


// reimplemented (imtrest::IRequestServlet)

bool CSubscriptionManagerComp::IsCommandSupported(const QByteArray& /*commandId*/) const
{
	return true;
}


imtrest::ConstResponsePtr CSubscriptionManagerComp::ProcessRequest(const imtrest::IRequest& request, const QByteArray& subCommandId) const
{
	QMutexLocker locker(&m_registeredClientsMutex);

	QByteArray commandId = subCommandId.isEmpty() ? request.GetCommandId() : subCommandId;

	auto webSocketRequest = dynamic_cast<const imtrest::CWebSocketRequest*>(&request);
	if (webSocketRequest != nullptr){
		QByteArray message = webSocketRequest->GetBody();

		QJsonParseError jsonError;
		QJsonDocument jsonDocument = QJsonDocument::fromJson(message, &jsonError);
		if (jsonDocument.isNull()){
			QByteArray errorMessage = QString("Unable to convert message to JSON: '%1'").arg(qPrintable(jsonError.errorString())).toUtf8();
			qDebug() << errorMessage;

			locker.unlock();
			return CreateErrorResponse(errorMessage, request);
		}

		QJsonObject rootObject = jsonDocument.object();

		switch (webSocketRequest->GetMethodType())
		{
		case imtrest::CWebSocketRequest::MT_CONNECTION_ACK:
			for (const QByteArray& subscriptionId : m_registeredClients.keys()){
				if (m_registeredClients[subscriptionId].m_status == IGqlSubscriptionClient::SS_IN_REGISTRATION){
					istd::IChangeableUniquePtr objectPtr(m_registeredClients[subscriptionId].m_request.CloneMe());
					locker.unlock();
					auto requestPtr = dynamic_cast<imtgql::CGqlRequest*>(objectPtr.GetPtr());
					if (requestPtr != nullptr){
						SubscriptionRegister(*requestPtr, subscriptionId);
					}
					locker.relock();
				}
			}
		break;

		case imtrest::CWebSocketRequest::MT_START_ACK:{
			QByteArray subscriptionId = rootObject.value("id").toString().toLocal8Bit();
			if (m_registeredClients.contains(subscriptionId)){
				m_registeredClients[subscriptionId].m_status = IGqlSubscriptionClient::SS_REGISTERED;

				for (IGqlSubscriptionClient* subscriptionClientPtr : m_registeredClients[subscriptionId].m_clients){
					if (subscriptionClientPtr != nullptr){
						subscriptionClientPtr->OnSubscriptionStatusChanged(subscriptionId, m_registeredClients[subscriptionId].m_status, message);
					}
				}
			}
		}
		break;

		case imtrest::CWebSocketRequest::MT_DATA:{
			QByteArray subscriptionId = rootObject.value("id").toString().toLocal8Bit();
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
		break;

		case imtrest::CWebSocketRequest::MT_QUERY_DATA:{
			QWriteLocker queryLocker(&m_queryDataMapLock);

			m_queryDataMap.insert(webSocketRequest->GetQueryId(), webSocketRequest->GetBody());

			queryLocker.unlock();
			locker.unlock();

			Q_EMIT OnQueryDataReceived(1);
		}
		break;

		case imtrest::CWebSocketRequest::MT_ERROR:
		{
			QWriteLocker queryLocker(&m_queryDataMapLock);

			m_queryDataMap.insert(webSocketRequest->GetQueryId(), webSocketRequest->GetBody());

			queryLocker.unlock();
			locker.unlock();

			Q_EMIT OnQueryDataReceived(1);
		}
			break;

		default:
			{
				QByteArray errorMessage = QString("Method type not correct: %1").arg(webSocketRequest->GetMethodType()).toUtf8();

				return CreateErrorResponse(errorMessage, request);
			}
		break;
		}
	}
	else{
		QByteArray errorMessage = "Error request";

		return CreateErrorResponse(errorMessage, request);
	}

	return imtrest::ConstResponsePtr();
}


// reimplemented (IGqlClient)

IGqlClient::GqlResponsePtr CSubscriptionManagerComp::SendRequest(IGqlClient::GqlRequestPtr requestPtr, imtbase::IUrlParam* /* urlParamPtr */) const
{
	QString key = QUuid::createUuid().toString(QUuid::WithoutBraces);

	QJsonObject dataObject;
	dataObject["type"] = "query";
	dataObject["id"] = key;
	QJsonObject payloadObject;
	payloadObject["data"] = QString(requestPtr->GetQuery());
	dataObject["payload"] = payloadObject;
	QJsonObject headersObject;
	const imtgql::IGqlContext* contextPtr = requestPtr->GetRequestContext();
	if (contextPtr != nullptr){
		imtgql::IGqlContext::Headers headers = contextPtr->GetHeaders();
		for (const QByteArray& headerId: headers.keys()){
			if (headerId != "accept-encoding" && headerId != "x-authentication-token"){
				headersObject[headerId] = QString(headers.value(headerId));
			}
		}
	}
	dataObject["headers"] = headersObject;

	QByteArray queryData = QJsonDocument(dataObject).toJson(QJsonDocument::Compact);

	imtrest::ConstRequestPtr constRequestPtr(m_engineCompPtr->CreateRequestForSend(*this, 0, queryData, "").PopInterfacePtr());

	NetworkOperation networkOperation(100, this);

	GqlResponsePtr retVal;

	if (!SendRequestInternal(*requestPtr, constRequestPtr)){
		SendErrorMessage(0, QString("Request could not be sent: '%1'").arg(QString(requestPtr->GetCommandId())));

		return retVal;
	}

	int resultCode = 0;
	for (int i = 0; i < 1000; i++){
		networkOperation.timer.start();
		resultCode = networkOperation.connectionLoop.exec();
		QCoreApplication::processEvents();
		QReadLocker queryLocker(&m_queryDataMapLock);

		if (m_queryDataMap.contains(key)){
			resultCode = 1;

			break;
		}

		resultCode = 0;
	}

	if(resultCode == 1){
		QWriteLocker queryLocker(&m_queryDataMapLock);

		if(m_queryDataMap.contains(key)){
			QByteArray responseData = m_queryDataMap.value(key);
			m_queryDataMap.remove(key);
			queryLocker.unlock();

			auto responsePtr = new imtgql::CGqlResponse(requestPtr);
			responsePtr->SetResponseData(responseData);

			retVal.SetPtr(responsePtr);

			return retVal;
		}
	}

	return retVal;
}


// protected methods

void CSubscriptionManagerComp::SubscriptionRegister(const imtgql::CGqlRequest& subscriptionRequest, const QByteArray& subscriptionId) const
{
	if (!m_engineCompPtr.IsValid()){
		Q_ASSERT(0);

		return;
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
		for (const QByteArray& headerId: headers.keys()){
			if (headerId != "accept-encoding"){
				headersObject[headerId] = QString(headers.value(headerId));
			}
		}
	}
	registerSubscription["headers"] = headersObject;

	QByteArray queryData = QJsonDocument(registerSubscription).toJson(QJsonDocument::Compact);

	imtrest::ConstRequestPtr requestPtr(m_engineCompPtr->CreateRequestForSend(*this, 0, queryData, "").PopInterfacePtr());

	SendRequestInternal(subscriptionRequest, requestPtr);
}


bool CSubscriptionManagerComp::SendRequestInternal(const imtgql::IGqlRequest& request, imtrest::ConstRequestPtr& requestPtr) const
{
	bool retVal = false;
	QByteArray clientId;

	auto requestImplPtr = dynamic_cast<const imtgql::CGqlRequest*>(&request);
	if (requestImplPtr != nullptr){
		clientId = requestImplPtr->GetHeader("clientid");
	}

	if (m_subscriptionSenderCompPtr.IsValid()){
		retVal = m_subscriptionSenderCompPtr->SendRequest(requestPtr);
	}
	else if (m_requestManagerCompPtr.IsValid()){
		const imtrest::ISender* sender = m_requestManagerCompPtr->GetSender(clientId);
		if (sender != nullptr){
			retVal = sender->SendRequest(requestPtr);
		}
	}

	return retVal;
}


// reimplemented (icomp::CComponentBase)

void CSubscriptionManagerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_connectionStatusProviderModelCompPtr.IsValid()){
		m_connectionStatusProviderModelCompPtr->AttachObserver(this);
	}
}


imtrest::ConstResponsePtr CSubscriptionManagerComp::CreateErrorResponse(const QByteArray& errorMessage, const imtrest::IRequest& request) const
{
	QByteArray requestBody = request.GetBody();
	QJsonDocument document = QJsonDocument::fromJson(requestBody);
	QJsonObject object = document.object();

	const imtrest::IProtocolEngine& engine = request.GetProtocolEngine();

	QString body =
		QString(R"({"id": "%1","type": "error","payload": {"errors": [{"errorType": "ProcessRequestError","message": "%2"}]}})")
						.arg(object["id"].toString())
						.arg(QString(errorMessage));

	QByteArray responseTypeId("text/html; charset=utf-8");
	imtrest::ConstResponsePtr responsePtr(
				engine.CreateResponse(
							request,
							imtrest::IProtocolEngine::SC_OPERATION_NOT_AVAILABLE,
							body.toUtf8(),
							responseTypeId).PopInterfacePtr());

	SendErrorMessage(0, QString(errorMessage));

	return responsePtr;
}


// public methods of the embedded class NetworkOperation

CSubscriptionManagerComp::NetworkOperation::NetworkOperation(int timeout, const CSubscriptionManagerComp* parent)
{
	Q_ASSERT(parent != nullptr);

	timerFlag = false;

	// If the network reply is finished, the internal event loop will be finished:
	connect(parent, &CSubscriptionManagerComp::OnQueryDataReceived, &connectionLoop, &QEventLoop::exit);

	// If the application will be finished, the internal event loop will be also finished:
	connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, &connectionLoop, &QEventLoop::quit);

	// If a timeout for the request was defined, start the timer:
	if (timeout > 0){
		timer.setSingleShot(true);

		// If the timer is running out, the internal event loop will be finished:
		connect(&timer, &QTimer::timeout, &connectionLoop, &QEventLoop::quit);
		timer.setInterval(timeout);
	}
}


CSubscriptionManagerComp::NetworkOperation::~NetworkOperation()
{
	timer.stop();
}


} // namespace imtclientgql


