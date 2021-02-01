#include <imtlog/CMessagesControllerParamsComp.h>


namespace imtlog
{

// public methods

// reimplemented (imtlog::IMessagesControllerParamsProvider)

int CMessagesControllerParamsComp::GetMaxContainerMessageCount(int messageId)
{
	if (*m_messageIdAttrPtr == messageId){
		return *m_maxContainerMessageCountAttrPtr;
	}

	if (m_slaveCompPtr.IsValid()){
		return m_slaveCompPtr->GetMaxContainerMessageCount(messageId);
	}

	return -1;
}


int CMessagesControllerParamsComp::GetContainerSavingPeriod(int messageId)
{
	if (*m_messageIdAttrPtr == messageId){
		return *m_containerSavingPeriodAttrPtr;
	}

	if (m_slaveCompPtr.IsValid()){
		return m_slaveCompPtr->GetContainerSavingPeriod(messageId);
	}

	return -1;
}


int CMessagesControllerParamsComp::GetContainerCachingTime(int messageId)
{
	if (*m_messageIdAttrPtr == messageId){
		return *m_containerCachingTimeAttrPtr;
	}

	if (m_slaveCompPtr.IsValid()){
		return m_slaveCompPtr->GetContainerCachingTime(messageId);
	}

	return -1;
}


} // namespace imtlog


