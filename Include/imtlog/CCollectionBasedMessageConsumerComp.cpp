// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlog/CCollectionBasedMessageConsumerComp.h>


namespace imtlog
{


CCollectionBasedMessageConsumerComp::CCollectionBasedMessageConsumerComp()
	:m_isSystemStarted(false)
{
}


// protected methods

// reimplemented (ibase::TRuntimeStatusHanderCompWrap)

void CCollectionBasedMessageConsumerComp::OnSystemStarted()
{
	BaseClass::OnSystemStarted();

	m_isSystemStarted = true;
}


// reimplemented (ilog::CLogCompBase)

void CCollectionBasedMessageConsumerComp::WriteMessageToLog(const MessagePtr& messagePtr)
{
	if (m_isSystemStarted){
		if (!m_messageCollectionCompPtr.IsValid()){
			Q_ASSERT(false);

			return;
		}
		while (!m_startMessages.isEmpty()){
			const MessagePtr startMessage = m_startMessages.takeFirst();
			m_messageCollectionCompPtr->InsertNewObject("MessageInfo", "", "", startMessage.GetPtr());
		}

		m_messageCollectionCompPtr->InsertNewObject("MessageInfo", "", "", messagePtr.GetPtr());
	}
	else {
		m_startMessages.append(messagePtr);
	}
}


} // namespace imtlog


