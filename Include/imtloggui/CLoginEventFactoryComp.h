#pragma once


// ImtCore includes
#include <imtloggui/CEventFactoryCompBase.h>


namespace imtloggui
{


class CLoginEventFactoryComp: public CEventFactoryCompBase
{
public:
	typedef CEventFactoryCompBase BaseClass;

	I_BEGIN_COMPONENT(CLoginEventFactoryComp)
		I_ASSIGN(m_loginIconAttrPtr, "LoginIcon", "Icon for login event", true, "");
		I_ASSIGN(m_logoutIconAttrPtr, "LogoutIcon", "Icon for logout event", true, "");
		I_ASSIGN(m_iconSizeAttrPtr, "IconSize", "Size of icons", true, 24);
	I_END_COMPONENT

	// reimplemented (imtloggui::IEventItemFactory)
	virtual IEventItem* CreateInstance(const ilog::IMessageConsumer::MessagePtr& message) const override;

private:
	I_ATTR(QByteArray, m_loginIconAttrPtr);
	I_ATTR(QByteArray, m_logoutIconAttrPtr);
	I_ATTR(int, m_iconSizeAttrPtr);
};


} // namespace imtloggui


