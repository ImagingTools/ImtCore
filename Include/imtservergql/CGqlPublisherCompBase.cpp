#include <imtservergql/CGqlPublisherCompBase.h>


// ACF includes
#include<istd/TDelPtr.h>

// ImtCore includes
#include<imtrest/IProtocolEngine.h>
#include<imtrest/ISender.h>
#include<imtrest/CWebSocketRequest.h>


namespace imtservergql
{


// public methods

// reimplemented (imtgql::IGqlSubscriberController)

bool CGqlPublisherCompBase::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	imtgql::CGqlObject fieldsPtr = gqlRequest.GetFields();
	if (fieldsPtr.GetFieldIds().isEmpty()){
		return false;
	}
	
	QByteArray commandId = gqlRequest.GetCommandId();
	
	return m_commandIdsAttrPtr.FindValue(commandId) != -1;
}


bool CGqlPublisherCompBase::RegisterSubscription(
	const QByteArray& subscriptionId,
	const imtgql::CGqlRequest& gqlRequest,
	const imtrest::IRequest& networkRequest,
	QString& errorMessage)
{
	Q_ASSERT(IsRequestSupported(gqlRequest));
	
	if (!IsRequestSupported(gqlRequest)){
		errorMessage = QString("Request with command-ID: '%1 'is not supported").arg(qPrintable(gqlRequest.GetCommandId()));
		SendErrorMessage(0, errorMessage, "CGqlPublisherCompBase");
		
		return false;
	}
	
	const imtrest::CWebSocketRequest* constWebSocketRequest = dynamic_cast<const imtrest::CWebSocketRequest*>(&networkRequest);
	imtrest::CWebSocketRequest* webSocketRequest = dynamic_cast<imtrest::CWebSocketRequest*>(const_cast<imtrest::CWebSocketRequest*>(constWebSocketRequest));
	if (webSocketRequest == nullptr){
		errorMessage = QString("Internal error");
		SendErrorMessage(0, errorMessage, "CGqlPublisherCompBase");
		
		return false;
	}
	
	for (RequestNetworks& requestNetworks: m_registeredSubscribers){
		if (requestNetworks.gqlRequest.IsEqual(gqlRequest)){
			requestNetworks.networkRequests.insert(subscriptionId, &networkRequest);
			
			webSocketRequest->RegisterRequestEventHandler(this);
			
			return true;
		}
	}
	
	RequestNetworks requestNetworks;
	requestNetworks.gqlRequest.CopyFrom(gqlRequest);
	requestNetworks.networkRequests.insert(subscriptionId, &networkRequest);
	m_registeredSubscribers.append(requestNetworks);
	
	webSocketRequest->RegisterRequestEventHandler(this);
	
	return true;
}


bool CGqlPublisherCompBase::UnregisterSubscription(const QByteArray& subscriptionId)
{
	for (int i = 0; i < m_registeredSubscribers.size(); i++){
		RequestNetworks& requestNetworks = m_registeredSubscribers[i];
		if (requestNetworks.networkRequests.contains(subscriptionId)){
			requestNetworks.networkRequests.remove(subscriptionId);
			
			if (requestNetworks.networkRequests.size() == 0){
				m_registeredSubscribers.removeAt(i);
			}
			
			return true;
		}
	}
	
	return false;
}


// reimplemented (imtrest::IRequestEventHandler)

void CGqlPublisherCompBase::OnRequestDestroyed(imtrest::IRequest* request)
{
	imtrest::CWebSocketRequest* webSocketRequestPtr = dynamic_cast<imtrest::CWebSocketRequest*>(request);
	if (webSocketRequestPtr != nullptr){
		UnregisterSubscription(webSocketRequestPtr->GetQueryId());
	}
}


// protected methods

bool CGqlPublisherCompBase::PushDataToSubscriber(
	const QByteArray& subscriptionId,
	const QByteArray& commandId,
	const QByteArray& data,
	const imtrest::IRequest& networkRequest)
{
	if (!m_requestManagerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'RequestManager' was not set", "CGqlPublisherCompBase");
		return false;
	}
	
	QByteArray body = QString(R"({"type": "data","id": "%1","payload": {"data": {"%2": %3}}})")
								.arg(qPrintable(subscriptionId), qPrintable(commandId), qPrintable(data)).toUtf8();
	
	QByteArray reponseTypeId = QByteArray("application/json; charset=utf-8");
	const imtrest::IProtocolEngine& engine = networkRequest.GetProtocolEngine();
	
	imtrest::ConstResponsePtr responsePtr(engine.CreateResponse(networkRequest, imtrest::IProtocolEngine::SC_OK, body, reponseTypeId));
	if (!responsePtr.IsValid()){
		SendErrorMessage(0, QString("Unable to send response to subscriber. Error: Response is invalid"), "CGqlPublisherCompBase");
		
		return false;
	}
	
	const imtrest::ISender* sender = m_requestManagerCompPtr->GetSender(networkRequest.GetRequestId());
	if (sender == nullptr){
		SendErrorMessage(0, QString("Unable to send response to subscriber. Error: Cannot found sender for request ID '%1'").arg(qPrintable(networkRequest.GetRequestId())), "CGqlPublisherCompBase");
		
		return false;
	}
	
	bool retVal = sender->SendResponse(responsePtr);
	if (!retVal){
		QString message = QString("Unable to send response to subscriber. Data: '%1'").arg(qPrintable(data));
		
		SendErrorMessage(0, message, "CGqlPublisherCompBase");
	}
	
	return true;
}


bool CGqlPublisherCompBase::PublishData(const QByteArray& commandId, const QByteArray& data)
{
	for (RequestNetworks& requestNetworks: m_registeredSubscribers){
		if (commandId == requestNetworks.gqlRequest.GetCommandId()){
			for (auto it = requestNetworks.networkRequests.constBegin(); it != requestNetworks.networkRequests.constEnd(); ++it){
				const imtrest::IRequest* networkRequestPtr = requestNetworks.networkRequests[it.key()];
				if (networkRequestPtr != nullptr){
					bool retVal = PushDataToSubscriber(it.key(), commandId, data, *networkRequestPtr);
					if (!retVal){
						QString message = QString("Unable to notify subscriber about the changes. Subscription-ID: '%1', '%2'").arg(qPrintable(commandId), qPrintable(data));
						SendErrorMessage(0, message, "CGqlPublisherCompBase");
					}
				}
			}
		}
	}

	return true;
}


} // namespace imtservergql


