#pragma once


// ACF includes
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
		I_ASSIGN_MULTI_0(m_messageIdListAttrPtr, "MessageIdList", "Supported message id's. Empty list for any", false);
		I_ASSIGN(m_slaveEventFactoryCompPtr, "SlaveEventFactory", "Slave event item factory", false, "");
		I_ASSIGN(m_timestampFormatCompPtr, "TimestampDisplayFormat", "Timestamp display format", true, "");
	I_END_COMPONENT

	// reimplemented (imtloggui::IEventItemFactory)
	virtual QVector<int> GetSupportedMessageIds() const override;

protected:
	bool IsSupportedMessageId(int messageId) const;
	IEventItem* CreateInstanceWithSlaveFactory(const ilog::IMessageConsumer::MessagePtr& message) const;
	
private:
	I_MULTIATTR(int, m_messageIdListAttrPtr);
	I_REF(IEventItemFactory, m_slaveEventFactoryCompPtr);
	I_ATTR(QByteArray, m_timestampFormatCompPtr);
};


} // namespace imtloggui


