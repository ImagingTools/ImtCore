#pragma once


// ImtCore includes
#include <imtloggui/CIconBasedEventFactoryComp.h>


namespace imtloggui
{


class CProducingPartEventFactoryComp: public CIconBasedEventFactoryComp
{
public:
	typedef CIconBasedEventFactoryComp BaseClass;

	I_BEGIN_COMPONENT(CProducingPartEventFactoryComp)
	I_END_COMPONENT

	// reimplemented (imtloggui::IEventItemFactory)
	virtual CEventItemBase* CreateInstance(const ilog::IMessageConsumer::MessagePtr& message) const override;
};


} // namespace imtloggui


