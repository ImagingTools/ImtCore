#include <imtbase/CMessageFilterComp.h>


namespace imtbase
{


// public methods

// reimplemented (ilog::IMessageConsumer)

bool CMessageFilterComp::IsMessageSupported(int messageCategory, int messageId, const istd::IInformationProvider* messagePtr) const
{
	bool retVal = false;

	if (!m_filteredMessageConsumerCompPtr.IsValid() && !m_slaveMessageConsumerCompPtr.IsValid()){
		retVal = true;
	}

	if (!retVal && m_filteredMessageConsumerCompPtr.IsValid()){
		if (m_filteredMessageConsumerCompPtr->IsMessageSupported(messageCategory, messageId, messagePtr)){
			retVal = true;
		}
	}

	if (!retVal && BaseClass::IsMessageSupported(messageCategory, messageId, messagePtr)){
		retVal = true;
	}

	return retVal;
}


void CMessageFilterComp::AddMessage(const MessagePtr& messagePtr)
{
	BaseClass::AddMessage(messagePtr);

	if (m_filteredMessageConsumerCompPtr.IsValid()){
		if (IsMessageAccepted(messagePtr->GetInformationId())){
			m_filteredMessageConsumerCompPtr->AddMessage(messagePtr);
		}
	}
}


// private methods

bool CMessageFilterComp::IsMessageAccepted(int messageId) const
{
	QSet<int> rejectedIds;
	if (m_rejectMessageIdsAttrPtr.IsValid()){
		for (int i = 0; i < m_rejectMessageIdsAttrPtr.GetCount(); i++){
			rejectedIds.insert(m_rejectMessageIdsAttrPtr[i]);
		}
	}

	if (m_acceptMessageIdsAttrPtr.IsValid()){
		QSet<int> acceptedIds;
		if (m_acceptMessageIdsAttrPtr.IsValid()){
			for (int i = 0; i < m_acceptMessageIdsAttrPtr.GetCount(); i++){
				acceptedIds.insert(m_acceptMessageIdsAttrPtr[i]);
			}
		}

		QSet<int> resultIds = acceptedIds - rejectedIds;

		return resultIds.contains(messageId);
	}
	else{
		return !rejectedIds.contains(messageId);
	}
}


} // namespace imtbase


