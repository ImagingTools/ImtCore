#pragma once


// ImtCore includes
#include <imtloggui/CEventFactoryCompBase.h>


namespace imtloggui
{


class CIconBasedEventFactoryComp: public CEventFactoryCompBase
{
public:
	typedef CEventFactoryCompBase BaseClass;

	I_BEGIN_COMPONENT(CIconBasedEventFactoryComp)
		I_ASSIGN(m_iconAttrPtr, "Icon", "Icon for events", true, "");
		I_ASSIGN(m_iconSizeAttrPtr, "IconSize", "Size of icons", true, 24);
	I_END_COMPONENT

	// reimplemented (imtloggui::IEventItemFactory)
	virtual IEventItem* CreateInstance(const ilog::IMessageConsumer::MessagePtr& message) const override;

protected:
	virtual void SetItemMetaInfo(IEventItem* eventItem) const override;

protected:
	I_ATTR(QByteArray, m_iconAttrPtr);
	I_ATTR(int, m_iconSizeAttrPtr);
};


} // namespace imtloggui


