#include <imtauthgql/CSessionModelSubscriberControllerComp.h>


// ImtCore includes
#include <imtgql/IGqlRequestProvider.h>
#include<imtrest/IProtocolEngine.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

void CSessionModelSubscriberControllerComp::OnSessionModelChanged(const istd::IChangeable::ChangeSet& changeSet, const QByteArray& sessionId)
{
	if (!m_requestManagerCompPtr.IsValid()){
		return;
	}

	if (sessionId.isEmpty()){
		return;
	}

	qDebug() << "OnSessionModelChanged" << sessionId;
	qDebug() << "Count subscribers:" << m_registeredSubscribers.count();

	for (RequestNetworks& requestNetworks: m_registeredSubscribers){
		for (const QByteArray& id: requestNetworks.networkRequests.keys()){
			const imtrest::IRequest* networkRequest = requestNetworks.networkRequests[id];
			QByteArray body = QString(R"({"type": "data", "id": "%1","payload":{"data": {"token": "%2"}}})")
								.arg(QString(id))
								.arg(QString(sessionId)).toUtf8();
			QByteArray reponseTypeId = QByteArray("application/json; charset=utf-8");
			const imtrest::IProtocolEngine& engine = networkRequest->GetProtocolEngine();

			imtrest::ConstResponsePtr responsePtr(engine.CreateResponse(*networkRequest, imtrest::IProtocolEngine::SC_OPERATION_NOT_AVAILABLE, body, reponseTypeId));
			if (responsePtr.IsValid()){
				const imtrest::ISender* sender = m_requestManagerCompPtr->GetSender(networkRequest->GetRequestId());
				if (sender != nullptr){
					qDebug() << "Send subscription" << body << "to the subscriber" << id;

					sender->SendResponse(responsePtr);
				}
			}
		}
	}
}


} // namespace imtauthgql


