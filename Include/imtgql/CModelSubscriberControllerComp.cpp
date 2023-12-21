#include <imtgql/CModelSubscriberControllerComp.h>


// ImtCore includes
#include<imtbase/IOperationContext.h>
#include<imtrest/IProtocolEngine.h>


namespace imtgql
{


// protected methods

bool CModelSubscriberControllerComp::SetSubscriptions()
{
	if (!m_requestManagerCompPtr.IsValid()){
		return false;
	}

	for (RequestNetworks& requestNetworks: m_registeredSubscribers){
		for (const QByteArray& id: requestNetworks.networkRequests.keys()){
			const imtrest::IRequest* networkRequest = requestNetworks.networkRequests[id];
			QByteArray body = QString(R"({"type": "data","id": "%1","payload": {"data": "%2"}})").arg(qPrintable(id)).arg(qPrintable("Change")).toUtf8();
			QByteArray reponseTypeId = QByteArray("application/json; charset=utf-8");
			const imtrest::IProtocolEngine& engine = networkRequest->GetProtocolEngine();

			imtrest::ConstResponsePtr responsePtr(engine.CreateResponse(*networkRequest, imtrest::IProtocolEngine::SC_OPERATION_NOT_AVAILABLE, body, reponseTypeId));
			if (responsePtr.IsValid()){
				const imtrest::ISender* sender = m_requestManagerCompPtr->GetSender(networkRequest->GetRequestId());
				if (sender != nullptr){
					qDebug() << "SetSubscriptions";

					sender->SendResponse(responsePtr);
				}
			}
		}
	}

	return true;
}


// reimplemented (icomp::CComponentBase)

void CModelSubscriberControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_modelCompPtr.IsValid()) {
		m_modelCompPtr->AttachObserver(this);
	}
}

void CModelSubscriberControllerComp::OnComponentDestroyed()
{
	if (m_modelCompPtr.IsValid()){
		m_modelCompPtr->DetachObserver(this);
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CSingleModelObserverBase)

void CModelSubscriberControllerComp::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	qDebug() << "Model OnUpdate";
	SetSubscriptions();
}


} // namespace imtgql

