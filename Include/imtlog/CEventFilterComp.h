#pragma once


// ACF includes

#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlog/CEventFilter.h>
#include <imtlog/CMessageFilterParams.h>


namespace imtlog
{


class CEventFilterComp:
			public icomp::CComponentBase,
			public imtlog::CEventFilter,
			public imtlog::CMessageFilterParams
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CEventFilterComp)
		I_REGISTER_INTERFACE(imtlog::IEventFilter);
		I_REGISTER_INTERFACE(imtlog::IMessageFilterParams);
		I_ASSIGN_MULTI_0(m_messageIdsAttrPtr, "MessageIds", "Message ids", false);
		I_ASSIGN(m_includeModeAttrPtr, "IncludeFilterMode", "Include filter mode", true, true);
	I_END_COMPONENT


protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_MULTIATTR(int, m_messageIdsAttrPtr);
	I_ATTR(bool, m_includeModeAttrPtr);
};


} // namespace imtlog


