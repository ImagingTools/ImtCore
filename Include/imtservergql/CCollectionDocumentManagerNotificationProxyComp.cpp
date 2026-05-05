// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CCollectionDocumentManagerNotificationProxyComp.h>


// ImtCore includes
#include <imtclientgql/CCollectionDocumentManagerSubscriberComp.h>


namespace imtservergql
{


// protected methods

// reimplemented (imtgql::IGqlSubscriberController)

bool CCollectionDocumentManagerNotificationProxyComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	if (m_collectionIdAttrPtr.IsValid()){
		QByteArray collectionId = *m_collectionIdAttrPtr;
		QByteArray gqlCommandId = gqlRequest.GetCommandId();

		if (gqlCommandId == QByteArrayLiteral("On") + collectionId + QByteArrayLiteral("DocumentChanged")){
			return true;
		}
		if (gqlCommandId == QByteArrayLiteral("On") + collectionId + QByteArrayLiteral("UndoChanged")){
			return true;
		}
	}

	return BaseClass::IsRequestSupported(gqlRequest);
}


// reimplemented (imod::CSingleModelObserverBase)

void CCollectionDocumentManagerNotificationProxyComp::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (m_commandIdsAttrPtr.GetCount() <= 0){
		return;
	}

	const imtclientgql::CCollectionDocumentManagerSubscriberComp* subscriberPtr =
		dynamic_cast<const imtclientgql::CCollectionDocumentManagerSubscriberComp*>(m_subscriberCompPtr.GetPtr());
	if (subscriberPtr == nullptr){
		return;
	}

	QByteArray notificationData;

	PublishData(m_commandIdsAttrPtr[0], notificationData);
}


// reimplemented (icomp::CComponentBase)

void CCollectionDocumentManagerNotificationProxyComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_subscriberModelCompPtr.IsValid()){
		m_subscriberModelCompPtr->AttachObserver(this);
	}
}


void CCollectionDocumentManagerNotificationProxyComp::OnComponentDestroyed()
{
	if (m_subscriberModelCompPtr.IsValid()){
		m_subscriberModelCompPtr->DetachObserver(this);
	}

	BaseClass::OnComponentDestroyed();
}


} // namespace imtservergql


