#include <imtgql/CGqlSubscriberControllerCompBase.h>


// ACF includes
#include<istd/TDelPtr.h>

// ImtCore includes
#include<imtrest/IProtocolEngine.h>
#include<imtrest/ISender.h>
#include<imtrest/CWebSocketRequest.h>


namespace imtgql
{


// public methods

// reimplemented (imtgql::IGqlSubscriberController)

bool CGqlSubscriberControllerCompBase::IsRequestSupported(const CGqlRequest& gqlRequest) const
{
	const QList<CGqlObject> fieldsPtr = gqlRequest.GetFields();
	if (fieldsPtr.isEmpty()){
		return false;
	}

	QByteArray commandId = gqlRequest.GetCommandId();

	return m_commandIdsAttrPtr.FindValue(commandId) != -1;
}


bool CGqlSubscriberControllerCompBase::RegisterSubscription(
			const QByteArray& subscriptionId,
			const imtgql::CGqlRequest& gqlRequest,
			const imtrest::IRequest& networkRequest,
			QString& errorMessage)
{
	Q_ASSERT(IsRequestSupported(gqlRequest));

	if (!IsRequestSupported(gqlRequest)){
		errorMessage = QString("Request with Command-ID: '%1 'is not supported").arg(qPrintable(gqlRequest.GetCommandId()));
		SendErrorMessage(0, errorMessage, "CGqlSubscriberControllerCompBase");

		return false;
	}

	const imtrest::CWebSocketRequest* constWebSocketRequest = dynamic_cast<const imtrest::CWebSocketRequest*>(&networkRequest);
	imtrest::CWebSocketRequest* webSocketRequest = dynamic_cast<imtrest::CWebSocketRequest*>(const_cast<imtrest::CWebSocketRequest*>(constWebSocketRequest));
	if (webSocketRequest == nullptr){
		errorMessage = QString("Internal error");
		SendErrorMessage(0, errorMessage, "CGqlSubscriberControllerCompBase");

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


bool CGqlSubscriberControllerCompBase::UnRegisterSubscription(const QByteArray& subscriptionId)
{
	for (RequestNetworks& requestNetworks: m_registeredSubscribers){
		if (requestNetworks.networkRequests.contains(subscriptionId)){
			requestNetworks.networkRequests.remove(subscriptionId);

			return true;
		}
	}

	return false;
}


void CGqlSubscriberControllerCompBase::OnRequestDestroyed(imtrest::IRequest* request)
{
	imtrest::CWebSocketRequest* webSocketRequestPtr = dynamic_cast<imtrest::CWebSocketRequest*>(request);
	if (webSocketRequestPtr != nullptr){
		UnRegisterSubscription(webSocketRequestPtr->GetSubscriptionId());
	}
}


// protected methods

bool CGqlSubscriberControllerCompBase::SetSubscriptions()
{
	if (*m_requestHandlerCommandIdAtrPtr == ""){
		return false;
	}

	if (!m_requestHandlerCompPtr.IsValid()){
		return false;
	}

	if (!m_requestManagerCompPtr.IsValid()){
		return false;
	}

	for (RequestNetworks& requestNetworks: m_registeredSubscribers){
		imtgql::CGqlRequest clonedRequest;
		clonedRequest.CopyFrom(requestNetworks.gqlRequest);
		clonedRequest.SetRequestType(imtgql::CGqlRequest::RT_QUERY);
		clonedRequest.SetCommandId(*m_requestHandlerCommandIdAtrPtr);

		QString errorMessage;
		istd::TDelPtr<imtbase::CTreeItemModel> resultModelPtr = m_requestHandlerCompPtr->CreateResponse(clonedRequest, errorMessage);
		if (!resultModelPtr.IsValid()){
			errorMessage = QString("Unable to send response to the subscribers result model is invalid");
			SendErrorMessage(0, errorMessage, "CGqlSubscriberControllerCompBase");

			return false;
		}

		imtbase::CTreeItemModel* dataModelPtr = resultModelPtr->GetTreeItemModel("data");
		if (dataModelPtr == nullptr){
			errorMessage = QString("Unable to send response to the subscribers result model is invalid");
			SendErrorMessage(0, errorMessage, "CGqlSubscriberControllerCompBase");

			return false;
		}

		QByteArray data =  dataModelPtr->toJSON().toUtf8();

		for (const QByteArray& id: requestNetworks.networkRequests.keys()){
			const imtrest::IRequest* networkRequestPtr = requestNetworks.networkRequests[id];
			if (networkRequestPtr != nullptr){
				QByteArray body = QString(R""(
	{
		"type": "data",
		"id": "%1",
		"payload": {
			"data": "%2"
		}
	}
		)"" ).arg(QString(id)).arg(QString(data)).toUtf8();
				QByteArray reponseTypeId = QByteArray("application/json; charset=utf-8");
				const imtrest::IProtocolEngine&  engine = networkRequestPtr->GetProtocolEngine();

				imtrest::ConstResponsePtr responsePtr(engine.CreateResponse(*networkRequestPtr, imtrest::IProtocolEngine::SC_OPERATION_NOT_AVAILABLE, body, reponseTypeId));
				if (responsePtr.IsValid()){
					const imtrest::ISender* senderPtr = m_requestManagerCompPtr->GetSender(networkRequestPtr->GetRequestId());
					if (senderPtr != nullptr){
						senderPtr->SendResponse(responsePtr);
					}
				}
				else{
					errorMessage = QString("Unable to create a response: '%1").arg(qPrintable(body));
					SendErrorMessage(0, errorMessage, "CGqlSubscriberControllerCompBase");
				}
			}
		}
 	}

	return true;
}


bool CGqlSubscriberControllerCompBase::SetAllSubscriptions(const QByteArray& data)
{
	if (!m_requestManagerCompPtr.IsValid()){
		return false;
	}

	for (RequestNetworks& requestNetworks: m_registeredSubscribers){
		for (const QByteArray& id: requestNetworks.networkRequests.keys()){
			const imtrest::IRequest* networkRequestPtr = requestNetworks.networkRequests[id];
			if (networkRequestPtr != nullptr){
				QByteArray body = QString(R"({"type": "data","id": "%1","payload": {"data": "%2"}})").arg(qPrintable(id)).arg(qPrintable(data)).toUtf8();
				QByteArray reponseTypeId = QByteArray("application/json; charset=utf-8");
				const imtrest::IProtocolEngine& engine = networkRequestPtr->GetProtocolEngine();

				imtrest::ConstResponsePtr responsePtr(engine.CreateResponse(*networkRequestPtr, imtrest::IProtocolEngine::SC_OPERATION_NOT_AVAILABLE, body, reponseTypeId));
				if (responsePtr.IsValid()){
					const imtrest::ISender* sender = m_requestManagerCompPtr->GetSender(networkRequestPtr->GetRequestId());
					if (sender != nullptr){
						sender->SendResponse(responsePtr);
					}
				}
			}
		}
	}

	return true;
}


bool CGqlSubscriberControllerCompBase::StartInternalSubscriber(const imtgql::CGqlRequest& /*gqlRequest*/, QString& /*errorMessage*/)
{
	return true;
}


} // namespace imtgql


