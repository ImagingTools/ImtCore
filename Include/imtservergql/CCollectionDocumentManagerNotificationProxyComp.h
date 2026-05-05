// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtservergql/CGqlPublisherCompBase.h>


namespace imtservergql
{


class CCollectionDocumentManagerNotificationProxyComp:
			public CGqlPublisherCompBase,
			public imod::TSingleModelObserverBase<istd::IChangeable>
{
public:
	typedef CGqlPublisherCompBase BaseClass;
	typedef imod::TSingleModelObserverBase<istd::IChangeable> BaseClass2;

	I_BEGIN_COMPONENT(CCollectionDocumentManagerNotificationProxyComp);
		I_ASSIGN(m_subscriberCompPtr, "Subscriber", "Document manager subscriber that receives notifications from remote server", true, "Subscriber");
		I_ASSIGN_TO(m_subscriberModelCompPtr, m_subscriberCompPtr, true);
		I_ASSIGN(m_collectionIdAttrPtr, "CollectionId", "Collection ID for routing notifications", true, "DummyCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IGqlSubscriberController)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	I_REF(istd::IChangeable, m_subscriberCompPtr);
	I_REF(imod::IModel, m_subscriberModelCompPtr);
	I_ATTR(QByteArray, m_collectionIdAttrPtr);
};


} // namespace imtservergql


