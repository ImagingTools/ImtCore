#include <imtloggui/CEventFactoryCompBase.h>


// ImtCore includes
#include <imtloggui/CEventItemBase.h>


namespace imtloggui
{


// reimplemented (imtloggui::IEventItemFactory)

QVector<int> CEventFactoryCompBase::GetSupportedMessageIds() const
{
	QVector<int> ids;

	if (m_messageIdListAttrPtr.IsValid()){
		for (int i = 0; i < m_messageIdListAttrPtr.GetCount(); i++){
			ids.append(m_messageIdListAttrPtr[i]);
		}
	}

	if (m_slaveEventFactoryCompPtr.IsValid()){
		ids.append(m_slaveEventFactoryCompPtr->GetSupportedMessageIds());
	}

	return ids;
}


// protected methods

bool CEventFactoryCompBase::IsSupportedMessageId(int messageId) const
{
	if (!m_messageIdListAttrPtr.IsValid()){
		return true;
	}

	if (m_messageIdListAttrPtr.GetCount() == 0){
		return true;
	}

	for (int i = 0; i < m_messageIdListAttrPtr.GetCount(); i++){
		if (messageId == m_messageIdListAttrPtr[i]){
			return true;
		}
	}

	return false;
}


IEventItem* CEventFactoryCompBase::CreateInstanceWithSlaveFactory(const ilog::IMessageConsumer::MessagePtr& message) const
{
	if (m_slaveEventFactoryCompPtr.IsValid()){
		return m_slaveEventFactoryCompPtr->CreateInstance(message);
	}

	return nullptr;
}


} // namespace imtloggui


