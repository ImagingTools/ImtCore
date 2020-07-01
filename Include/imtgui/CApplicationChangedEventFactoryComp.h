#pragma once


// ImtCore includes
#include <imtloggui/CIconBasedEventFactoryComp.h>


namespace imtgui
{


class CApplicationChangedEventFactoryComp: public imtloggui::CIconBasedEventFactoryComp
{
public:
	typedef imtloggui::CIconBasedEventFactoryComp BaseClass;

	I_BEGIN_COMPONENT(CApplicationChangedEventFactoryComp)
	I_END_COMPONENT

	// reimplemented (imtloggui::IEventItemFactory)
	virtual imtloggui::CEventItemBase* CreateInstance(const ilog::IMessageConsumer::MessagePtr& message) const override;
};


} // namespace imtgui


