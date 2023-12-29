#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtgql/CGqlSubscriberControllerCompBase.h>
#include <imtbase/ICollectionStructure.h>


namespace imtgql
{


class CStructureSubscriberControllerComp:
			public imtgql::CGqlSubscriberControllerCompBase,
			public imod::TSingleModelObserverBase<istd::IChangeable>
{
public:
	typedef imtgql::CGqlSubscriberControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CStructureSubscriberControllerComp);
		I_ASSIGN(m_collectionStructureCompPtr, "CollectionStructure", "Collection structure", true, "CollectionStructure");
		I_ASSIGN_TO(m_modelCompPtr, m_collectionStructureCompPtr, true);
	I_END_COMPONENT;

protected:
	virtual bool SetSubscriptions() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

protected:
	I_REF(imtbase::ICollectionStructure, m_collectionStructureCompPtr);
	I_REF(imod::IModel, m_modelCompPtr);

};


} // namespace imtgql


