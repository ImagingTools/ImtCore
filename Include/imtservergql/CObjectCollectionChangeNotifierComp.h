// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtservergql/CGqlPublisherCompBase.h>


namespace imtservergql
{


/**
	Notifier about changes in the underlaying object collection.
*/
class CObjectCollectionChangeNotifierComp:
			public imtservergql::CGqlPublisherCompBase,
			protected imod::TSingleModelObserverBase<imtbase::IObjectCollection>
{
public:
	typedef imtservergql::CGqlPublisherCompBase BaseClass;
	typedef imod::TSingleModelObserverBase<imtbase::IObjectCollection> BaseClass2;

	I_BEGIN_COMPONENT(CObjectCollectionChangeNotifierComp);
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection", true, "ObjectCollection");
		I_ASSIGN_TO(m_objectCollectionModelCompPtr, m_objectCollectionCompPtr, true);
		I_ASSIGN(m_isSendItemSource, "IsSendItemSource", "Object collection", false, false);
		I_ASSIGN(m_collectionIdAttrPtr, "CollectionId", "Collection-ID", false, "");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IGqlSubscriberController)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

protected:
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
	I_REF(imod::IModel, m_objectCollectionModelCompPtr);
	I_ATTR(bool, m_isSendItemSource);
	I_ATTR(QByteArray, m_collectionIdAttrPtr);
};


} // namespace imtservergql


