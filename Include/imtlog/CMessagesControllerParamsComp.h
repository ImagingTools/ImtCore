#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlog/IMessagesControllerParamsProvider.h>


namespace imtlog
{


class CMessagesControllerParamsComp:
			public icomp::CComponentBase,
			virtual public IMessagesControllerParamsProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CMessagesControllerParamsComp)
		I_REGISTER_INTERFACE(IMessagesControllerParamsProvider);
		I_ASSIGN(m_messageIdAttrPtr, "MessageId", "Message id", true, 0);
		I_ASSIGN(m_maxContainerMessageCountAttrPtr, "MaxContainerMessagesCount", "Max message count stored in container", true, 1000);
		I_ASSIGN(m_containerSavingPeriodAttrPtr, "ContainerSavingPeriod", "Period in seconds of saving the container until the maximum number of messages is reached", true, 60);
		I_ASSIGN(m_containerCachingTimeAttrPtr, "ContainerCachingTime", "The time the container was in the cache after closing", true, 3600);
		I_ASSIGN(m_slaveCompPtr, "Slave", "Slave", false, "");
	I_END_COMPONENT

	// reimplemented (imtlog::IMessagesControllerParamsProvider)
	virtual int GetMaxContainerMessageCount(int messageId) override;
	virtual int GetContainerSavingPeriod(int messageId) override;
	virtual int GetContainerCachingTime(int messageId) override;

private:
	I_ATTR(int, m_messageIdAttrPtr);
	I_ATTR(int, m_maxContainerMessageCountAttrPtr);
	I_ATTR(int, m_containerSavingPeriodAttrPtr);
	I_ATTR(int, m_containerCachingTimeAttrPtr);
	I_REF(IMessagesControllerParamsProvider, m_slaveCompPtr);
};


} // namespace imtlog


