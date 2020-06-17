#pragma once


// ImtCore includes
#include <imtloggui/CEventFactoryCompBase.h>


namespace imtloggui
{


class CGeneralEventFactoryComp: public CEventFactoryCompBase
{
public:
	typedef CEventFactoryCompBase BaseClass;

	I_BEGIN_COMPONENT(CGeneralEventFactoryComp)
		I_ASSIGN(m_iconSizeAttrPtr, "IconSize", "Size of icons", true, 24);
	I_END_COMPONENT

	// reimplemented (imtloggui::IEventItemFactory)
	virtual CEventItemBase* CreateInstance(const ilog::IMessageConsumer::MessagePtr& message) const override;

private:
	I_ATTR(int, m_iconSizeAttrPtr);
};


} // namespace imtloggui


