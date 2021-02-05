#pragma once


// Qt includes
#include <QtGui/QPixmap>

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
	virtual IEventItem* CreateInstance(const ilog::IMessageConsumer::MessagePtr& messagePtr) const override;

private:
	I_ATTR(int, m_iconSizeAttrPtr);

private:
	mutable QPixmap m_noneIcon;
	mutable QPixmap m_okIcon;
	mutable QPixmap m_warningIcon;
	mutable QPixmap m_errorIcon;
	mutable QPixmap m_invalidIcon;
};


} // namespace imtloggui


