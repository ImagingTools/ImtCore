#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <ilog/CMessageContainer.h>


namespace imtloggui
{


class CRepresentationEventsModelComp:
			public icomp::CComponentBase,
			public ilog::CMessageContainer
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRepresentationEventsModelComp)
		I_REGISTER_INTERFACE(ilog::IMessageConsumer);
		I_REGISTER_INTERFACE(ilog::IMessageContainer);
	I_END_COMPONENT
};


} // namespace imtloggui


