#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtservergql/CGqlSubscriberControllerCompBase.h>


namespace imtservergql
{


class CObjectCollectionSubscriberControllerComp:
			public imtservergql::CGqlSubscriberControllerCompBase,
			public imod::TSingleModelObserverBase<imtbase::IObjectCollection>
{
public:
	typedef imtservergql::CGqlSubscriberControllerCompBase BaseClass;
	typedef imod::TSingleModelObserverBase<imtbase::IObjectCollection> BaseClass2;

	I_BEGIN_COMPONENT(CObjectCollectionSubscriberControllerComp);
		I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection", true, "ObjectCollection");
		I_ASSIGN_TO(m_objectCollectionModelCompPtr, m_objectCollectionCompPtr, true);
		I_ASSIGN(m_isSendItemSource, "IsSendItemSource", "Object collection", false, false);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual bool SetSubscriptions() override;

protected:
	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
	I_REF(imod::IModel, m_objectCollectionModelCompPtr);
	I_ATTR(bool, m_isSendItemSource);
};


} // namespace imtservergql


