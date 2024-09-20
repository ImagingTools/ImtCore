#pragma once


// ACF includes
#include <ibase/TModelObserverCompWrap.h>
#include <icomp/TMakeComponentWrap.h>
#include <imod/TSingleModelObserverBase.h>


namespace imtbase
{


class CModelUpdateBridgeComp: public icomp::CComponentBase, public imod::TSingleModelObserverBase<istd::IChangeable>
{
public:
	typedef icomp::CComponentBase BaseClass;
	
	I_BEGIN_COMPONENT(CModelUpdateBridgeComp);
		I_ASSIGN_MULTI_0(m_slaveModelCompPtr, "SlaveModels", "Slave models", false);
	I_END_COMPONENT

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void BeforeUpdate(imod::IModel* modelPtr) override;
	virtual void AfterUpdate(imod::IModel* modelPtr, const istd::IChangeable::ChangeSet& changeSet) override;

private:
	I_MULTIREF(istd::IChangeable, m_slaveModelCompPtr);
};


} // namespace imtbase


