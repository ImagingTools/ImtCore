#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtgql/CGqlSubscriberControllerCompBase.h>


namespace imtgql
{


class CModelSubscriberControllerComp:
			public imtgql::CGqlSubscriberControllerCompBase,
			public imod::TSingleModelObserverBase<istd::IChangeable>
{
public:
	typedef imtgql::CGqlSubscriberControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CModelSubscriberControllerComp);
		I_ASSIGN(m_modelCompPtr, "Model", "Model", true, "Model");
	I_END_COMPONENT;

protected:
	virtual bool SetSubscriptions() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

protected:
	I_REF(imod::IModel, m_modelCompPtr);
};


} // namespace imtgql


