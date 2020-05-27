#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItem>

// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IEventItemFactory.h>


namespace imtloggui
{


class CLoginEventFactoryComp: public icomp::CComponentBase, virtual public IEventItemFactory
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CLoginEventFactoryComp)
		I_REGISTER_INTERFACE(IEventItemFactory)
		I_ASSIGN(m_loginIconAttrPtr, "LoginIcon", "Icon for login event", true, "");
		I_ASSIGN(m_logoutIconAttrPtr, "LogoutIcon", "Icon for logout event", true, "");
		I_ASSIGN(m_iconSizeAttrPtr, "IconSize", "Size of icons", true, 24);
		I_ASSIGN(m_slaveEventFactoryCompPtr, "SlaveEventFactory", "Slave event item factory", false, "");
	I_END_COMPONENT

	// reimplemented (imtloggui::IEventItemFactory)
	virtual CEventItemBase* CreateInstance(const ilog::IMessageConsumer::MessagePtr& message) const override;

private:
	I_ATTR(QByteArray, m_loginIconAttrPtr);
	I_ATTR(QByteArray, m_logoutIconAttrPtr);
	I_ATTR(int, m_iconSizeAttrPtr);
	I_REF(IEventItemFactory, m_slaveEventFactoryCompPtr);
};


} // namespace imtloggui


