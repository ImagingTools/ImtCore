#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IEventItemFactory.h>


namespace imtloggui
{


class CEventFactoryCompBase: public icomp::CComponentBase, virtual public IEventItemFactory
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CEventFactoryCompBase)
		I_REGISTER_INTERFACE(IEventItemFactory)
		I_ASSIGN_MULTI_0(m_idsAttrPtr, "MessageIds", "Supported message id's. Empty list for any", false);
		I_ASSIGN(m_slaveEventFactoryCompPtr, "SlaveEventFactory", "Slave event item factory", false, "");
	I_END_COMPONENT

protected:
	bool IsSupportedMessageId(int messageId) const;
	CEventItemBase* CreateInstanceWithSlaveFactory(const ilog::IMessageConsumer::MessagePtr& message) const;

private:
	I_MULTIATTR(int, m_idsAttrPtr);
	I_REF(IEventItemFactory, m_slaveEventFactoryCompPtr);
};


} // namespace imtloggui


