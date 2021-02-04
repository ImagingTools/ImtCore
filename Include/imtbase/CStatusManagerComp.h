#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/CSimpleStatus.h>


namespace imtbase
{


/**
	Status manager component-based implementation.
*/
class CStatusManagerComp:
			public icomp::CComponentBase,
			virtual public CSimpleStatus
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CStatusManagerComp);
		I_REGISTER_INTERFACE(imtbase::IStatusManager);
		I_REGISTER_INTERFACE(istd::IInformationProvider);
		I_ASSIGN(m_defaultDescriptionAttrPtr, "DefaultDescription", "Default information description used if no information description is set", false, "");
		I_ASSIGN(m_defaultSourceAttrPtr, "DefaultSource", "Default information source used if no information source is set", false, "Unknown");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated () override;

private:
	I_TEXTATTR(m_defaultDescriptionAttrPtr);
	I_TEXTATTR(m_defaultSourceAttrPtr);
};


} // namespace imtbase


