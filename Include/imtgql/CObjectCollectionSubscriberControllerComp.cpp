#include <imtgql/CObjectCollectionSubscriberControllerComp.h>


// ImtCore includes
#include<imtrest/IProtocolEngine.h>


namespace imtgql
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CObjectCollectionSubscriberControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_objectCollectionModelCompPtr.IsValid()) {
		m_objectCollectionModelCompPtr->AttachObserver(this);
	}
}


void CObjectCollectionSubscriberControllerComp::OnTimeout()
{
	SetSubscriptions();
}


void CObjectCollectionSubscriberControllerComp::OnComponentDestroyed()
{
	if (m_objectCollectionModelCompPtr.IsValid()){
		m_objectCollectionModelCompPtr->DetachObserver(this);
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CSingleModelObserverBase)

void CObjectCollectionSubscriberControllerComp::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	SetSubscriptions();
}


bool CObjectCollectionSubscriberControllerComp::SetSubscriptions()
{
	if (!m_requestManagerCompPtr.IsValid()){
		return false;
	}

	for (RequestNetworks& requestNetworks: m_registeredSubscribers){
		for (const QByteArray& id: requestNetworks.networkRequests.keys()){
			const imtrest::IRequest* networkRequest = requestNetworks.networkRequests[id];
			QByteArray body = QString(R"(
{
"type": "data",
"id": %1,
"payload": {
	"data": %2
}
})")
								.arg(QString(id))
								.arg(QString("Test")).toUtf8();
			QByteArray reponseTypeId = QByteArray("application/json; charset=utf-8");
			const imtrest::IProtocolEngine& engine = networkRequest->GetProtocolEngine();

			imtrest::ConstResponsePtr responsePtr(engine.CreateResponse(*networkRequest, imtrest::IProtocolEngine::SC_OPERATION_NOT_AVAILABLE, body, reponseTypeId));
			if (responsePtr.IsValid()){
				const imtrest::ISender* sender = m_requestManagerCompPtr->GetSender(networkRequest->GetRequestId());
				if (sender != nullptr){
					sender->SendResponse(responsePtr);
				}
			}
		}
	}

	return true;
}


} // namespace imtgql


