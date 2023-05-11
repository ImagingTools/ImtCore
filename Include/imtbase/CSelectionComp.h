#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/CSelection.h>


namespace imtbase
{


class CSelectionComp:
			public icomp::CComponentBase,
			public CSelection
{
public:
	typedef icomp::CComponentBase BaseClass;
		
	I_BEGIN_COMPONENT(CSelectionComp);
		I_REGISTER_INTERFACE(ISelection);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_ASSIGN(m_constraintsCompPtr, "Constraints", "Selection constraints", false, "");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	I_REF(ICollectionInfo, m_constraintsCompPtr);
};


} // namespace imtbase


