// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CModelChangeNotifierComp.h>


// ImtCore includes
#include<imtrest/IProtocolEngine.h>


namespace imtservergql
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CModelChangeNotifierComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_modelCompPtr.IsValid()){
		m_modelCompPtr->AttachObserver(this);
	}
}


void CModelChangeNotifierComp::OnComponentDestroyed()
{
	EnsureModelDetached();

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CSingleModelObserverBase)

void CModelChangeNotifierComp::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (!m_requestManagerCompPtr.IsValid()){
		return;
	}

	for (RequestNetworks& requestNetworks : m_registeredSubscribers){
		for (const QByteArray& id : requestNetworks.networkRequests.keys()){
			const imtrest::IRequest* networkRequest = requestNetworks.networkRequests[id];
			if (networkRequest == nullptr){
				continue;
			}

			QByteArray body = QString(R"({"type": "data","id": "%1","payload": {"data": {"%2": {}}}})")
				.arg(qPrintable(id))
				.arg(qPrintable(networkRequest->GetCommandId()))
				.toUtf8();
			QByteArray reponseTypeId = QByteArray("application/json; charset=utf-8");
			const imtrest::IProtocolEngine& engine = networkRequest->GetProtocolEngine();

			imtrest::ConstResponsePtr responsePtr(engine.CreateResponse(*networkRequest, imtrest::IProtocolEngine::SC_OK, body, reponseTypeId).PopInterfacePtr());
			if (responsePtr.IsValid()){
				const imtrest::ISender* sender = m_requestManagerCompPtr->GetSender(networkRequest->GetRequestId());
				if (sender != nullptr){
					sender->SendResponse(responsePtr);
				}
			}
		}
	}
}


} // namespace imtservergql


