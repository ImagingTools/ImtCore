#include <imtclientgql/CSubscriptionManagerComp.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QMessageAuthenticationCode>
#include <QtNetwork/QNetworkRequest>
#include <QtCore/QUrl>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>

// ACF includes
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtrest/CWebSocketRequest.h>


namespace imtclientgql
{


// public methods

CSubscriptionManagerComp::CSubscriptionManagerComp()
	:m_loginStatus(imtauth::ILoginStatusProvider::LSF_CACHED)
{
}


// reimplemented (imtgql::IGqlSubscriptionManager)

QByteArray CSubscriptionManagerComp::RegisterSubscription(
			const imtgql::IGqlRequest& subscriptionRequest,
			imtclientgql::IGqlSubscriptionClient* subscriptionClient)
{
	if (!subscriptionClient){
		return QByteArray();
	}

	QByteArray clientId;
	const imtgql::CGqlRequest* requestImplPtr = dynamic_cast<const imtgql::CGqlRequest*>(&subscriptionRequest);
	if (requestImplPtr != nullptr) {
		requestImplPtr->GetParams();
		const imtgql::CGqlObject* input = requestImplPtr->GetParam("input");
		if (input != nullptr) {
			const imtgql::CGqlObject* addition = input->GetFieldArgumentObjectPtr("addition");
			if (addition != nullptr) {
				clientId = addition->GetFieldArgumentValue("clientId").toByteArray();
			}
		}
	}

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
	subscriptionHelper.m_status = imtclientgql::IGqlSubscriptionClient::SS_IN_REGISTRATION;
	subscriptionHelper.m_clients.append(subscriptionClient);
	m_registeredClients.insert(subscriptionId.toLocal8Bit(), subscriptionHelper);

	if (m_loginStatus == imtauth::ILoginStatusProvider::LSF_LOGGED_IN){
		ServiceManagerRegister(*requestImplPtr, subscriptionId.toLocal8Bit());
	}

	return subscriptionId.toLocal8Bit();
}


bool CSubscriptionManagerComp::UnregisterSubscription(const QByteArray& subscriptionId)
{
	if (m_registeredClients.contains(subscriptionId)){
		m_registeredClients.remove(subscriptionId);

		return true;
	}

	return false;
}


void CSubscriptionManagerComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	if (!m_loginStatusCompPtr.IsValid()){
		return;
	}

	m_loginStatus = (imtauth::ILoginStatusProvider::LoginStatusFlags)m_loginStatusCompPtr->GetLoginStatus();

	QByteArray clientId = changeSet.GetChangeInfo("ClientId").toByteArray();

	for (QByteArray subscriptionId : m_registeredClients.keys()){
		if (m_loginStatus != imtauth::ILoginStatusProvider::LSF_LOGGED_IN){
			if (m_registeredClients[subscriptionId].m_clientId == clientId){
				m_registeredClients[subscriptionId].m_status = imtclientgql::IGqlSubscriptionClient::SS_IN_REGISTRATION;
			}
		}
		else{
			if (m_registeredClients[subscriptionId].m_clientId == clientId){
				ServiceManagerRegister(m_registeredClients[subscriptionId].m_request, subscriptionId);
			}
		}
	}
}


// reimplemented (imtrest::IRequestServlet)

imtrest::ConstResponsePtr CSubscriptionManagerComp::ProcessRequest(const imtrest::IRequest& request) const
{
	QByteArray commandId = request.GetCommandId();

	const imtrest::CWebSocketRequest* webSocketRequest = dynamic_cast<const imtrest::CWebSocketRequest*>(&request);
	if (webSocketRequest != nullptr){
		QByteArray message = webSocketRequest->GetBody();

		qDebug() << "ProcessRequest: " << message;
		qDebug() << "MethodType: " << webSocketRequest->GetMethodType();

		QJsonParseError jsonError;
		QJsonDocument jsonDocument = QJsonDocument::fromJson(message, &jsonError);
		if (jsonDocument.isNull()){
			QByteArray errorMessage = QString("Unable to convert message to JSON: '%1'").arg(qPrintable(jsonError.errorString())).toUtf8();
			qDebug() << errorMessage;

			return CreateErrorResponse(errorMessage, request);
		}

		QJsonObject rootObject = jsonDocument.object();

		switch (webSocketRequest->GetMethodType())
		{
		case imtrest::CWebSocketRequest::MT_CONNECTION_ASK:
			m_loginStatus = imtauth::ILoginStatusProvider::LSF_LOGGED_IN;

			for (QByteArray subscriptionId : m_registeredClients.keys()){
				if (m_registeredClients[subscriptionId].m_status == imtclientgql::IGqlSubscriptionClient::SS_IN_REGISTRATION){
					ServiceManagerRegister(m_registeredClients[subscriptionId].m_request, subscriptionId);
				}
			}
		break;

		case imtrest::CWebSocketRequest::MT_START_ASK:{
			QByteArray subscriptionId = rootObject.value("id").toString().toLocal8Bit();
			if (m_registeredClients.contains(subscriptionId)){
				m_registeredClients[subscriptionId].m_status = imtclientgql::IGqlSubscriptionClient::SS_REGISTERED;

				for (imtclientgql::IGqlSubscriptionClient* subscriptionClientPtr : m_registeredClients[subscriptionId].m_clients){
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
				for (imtclientgql::IGqlSubscriptionClient* subscriptionClientPtr : m_registeredClients[subscriptionId].m_clients){
					if (subscriptionClientPtr != nullptr){
						if (!rootObject.contains("payload")){
							qDebug() << "rootObject not contains 'payload'";

							break;
						}

						QJsonObject payloadObject = rootObject.value("payload").toObject();

						QJsonDocument document;
						document.setObject(payloadObject);

						qDebug() << "document" << document.toJson();

						QByteArray payload = document.toJson(QJsonDocument::Compact);

						subscriptionClientPtr->OnResponseReceived(subscriptionId, payload);
					}
				}
			}
		}
		break;

		case imtrest::CWebSocketRequest::MT_QUERY_DATA:{
			m_queryDataMap.insert(webSocketRequest->GetSubscriptionId(), webSocketRequest->GetBody());

			Q_EMIT OnQueryDataReceived(1);
		}
		break;

		case imtrest::CWebSocketRequest::MT_ERROR:
			SendErrorMessage(0, message);
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


QByteArray CSubscriptionManagerComp::GetSupportedCommandId() const
{
	return QByteArray();
}


// reimplemented (IGqlClient)

bool CSubscriptionManagerComp::SendRequest(const imtgql::IGqlRequest& request, imtgql::IGqlResponseHandler& responseHandler) const
{
	QString key = QUuid::createUuid().toString(QUuid::WithoutBraces);

	QJsonObject dataObject;
	dataObject["type"] = "query";
	dataObject["id"] = key;
	QJsonObject payloadObject;
	payloadObject["data"] = QString(request.GetQuery());
	dataObject["payload"] = payloadObject;
	QByteArray queryData = QJsonDocument(dataObject).toJson(QJsonDocument::Compact);

	imtrest::ConstRequestPtr requestPtr(m_engineCompPtr->CreateRequestForSend(*this, 0, queryData, ""));

	NetworkOperation networkOperation(10000, this);

	if (!SendRequestInternal(request, requestPtr)){
		SendErrorMessage(0, QString("Request could not be sent: '%1'").arg(QString(request.GetCommandId())));

		return false;
	}

	while(true){
		int resultCode = networkOperation.connectionLoop.exec(QEventLoop::ExcludeUserInputEvents);
		if(resultCode == 1){
			if(m_queryDataMap.contains(key)){
				responseHandler.OnReply(request, m_queryDataMap.value(key));

				m_queryDataMap.remove(key);

				return true;
			}
			continue;
		}
		else{
			break;
		}
	}

	return false;
}


// protected methods

void CSubscriptionManagerComp::ServiceManagerRegister(const imtgql::CGqlRequest& subscriptionRequest, QByteArray subscriptionId) const
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

	QByteArray queryData = QJsonDocument(registerSubscription).toJson(QJsonDocument::Compact);

	imtrest::ConstRequestPtr requestPtr(m_engineCompPtr->CreateRequestForSend(*this, 0, queryData, ""));

	SendRequestInternal(subscriptionRequest, requestPtr);
}


bool CSubscriptionManagerComp::SendRequestInternal(const imtgql::IGqlRequest& request, imtrest::ConstRequestPtr& requestPtr) const
{
	bool retVal = false;
	QByteArray clientId;

	const imtgql::CGqlRequest* requestImplPtr = dynamic_cast<const imtgql::CGqlRequest*>(&request);
	if (requestImplPtr != nullptr){
		requestImplPtr->GetParams();
		const imtgql::CGqlObject* input = requestImplPtr->GetParam("input");
		if (input != nullptr){
			const imtgql::CGqlObject* addition = input->GetFieldArgumentObjectPtr("addition");
			if (addition != nullptr){
				clientId = addition->GetFieldArgumentValue("clientId").toByteArray();
			}
		}
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

	if (m_webLoginStatusModelCompPtr.IsValid()){
		m_webLoginStatusModelCompPtr->AttachObserver(this);
	}
}


imtrest::ConstResponsePtr CSubscriptionManagerComp::CreateErrorResponse(QByteArray errorMessage, const imtrest::IRequest& request) const
{
	const imtrest::IProtocolEngine& engine = request.GetProtocolEngine();

	QString body = QString(R""(
{
	"type": "error",
	"payload": {
		"errors": [
			{
				"errorType": "ProcessRequestError",
				"message": "%1"
			}
		]
	}
}
	)"" ).arg(QString(errorMessage));

	QByteArray reponseTypeId = QByteArray("text/html; charset=utf-8");

	imtrest::ConstResponsePtr responsePtr(
				engine.CreateResponse(
							request,
							imtrest::IProtocolEngine::SC_OPERATION_NOT_AVAILABLE,
							body.toUtf8(),
							reponseTypeId));

	SendErrorMessage(0, QString(errorMessage));

	return responsePtr;
}


// public methods of the embedded class NetworkOperation

CSubscriptionManagerComp::NetworkOperation::NetworkOperation(int timeout, const CSubscriptionManagerComp* parent)
{
	Q_ASSERT(parent != nullptr);

	timerFlag = false;

	// If the network reply is finished, the internal event loop will be finished:
	QObject::connect(parent, &CSubscriptionManagerComp::OnQueryDataReceived, &connectionLoop, &QEventLoop::exit, Qt::DirectConnection);

	// If the application will be finished, the internal event loop will be also finished:
	QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, &connectionLoop, &QEventLoop::quit, Qt::DirectConnection);

	// If a timeout for the request was defined, start the timer:
	if (timeout > 0){
		timer.setSingleShot(true);

		// If the timer is running out, the internal event loop will be finished:
		QObject::connect(&timer, &QTimer::timeout, &connectionLoop, &QEventLoop::quit, Qt::DirectConnection);

		timer.start(timeout);
	}
}


CSubscriptionManagerComp::NetworkOperation::~NetworkOperation()
{
	timer.stop();
}


} // namespace imtclientgql


