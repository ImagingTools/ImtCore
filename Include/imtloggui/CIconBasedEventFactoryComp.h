#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItem>

// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IEventItemFactory.h>


namespace imtloggui
{


class CIconBasedEventFactoryComp: public icomp::CComponentBase, virtual public IEventItemFactory
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CIconBasedEventFactoryComp)
		I_REGISTER_INTERFACE(IEventItemFactory)
		I_ASSIGN_MULTI_0(m_idsAttrPtr, "MessageIds", "Supported message id's. Empty list for any", false);
		I_ASSIGN(m_iconAttrPtr, "Icon", "Icon for events", true, "");
		I_ASSIGN(m_iconSizeAttrPtr, "IconSize", "Size of icons", true, 24);
		I_ASSIGN(m_slaveEventFactoryCompPtr, "SlaveEventFactory", "Slave event item factory", false, "");
	I_END_COMPONENT

	// reimplemented (imtloggui::IEventItemFactory)
	virtual CEventItemBase* CreateInstance(const ilog::IMessageConsumer::MessagePtr& message) const override;

private:
	I_MULTIATTR(int, m_idsAttrPtr);
	I_ATTR(QByteArray, m_iconAttrPtr);
	I_ATTR(int, m_iconSizeAttrPtr);
	I_REF(IEventItemFactory, m_slaveEventFactoryCompPtr);
};


} // namespace imtloggui


