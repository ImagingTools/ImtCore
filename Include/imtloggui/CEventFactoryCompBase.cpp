#include <imtloggui/CEventFactoryCompBase.h>


namespace imtloggui
{


// protected methods

bool CEventFactoryCompBase::IsSupportedMessageId(int messageId) const
{
	if (!m_idsAttrPtr.IsValid()){
		return true;
	}

	if (m_idsAttrPtr.GetCount() == 0){
		return true;
	}

	for (int i = 0; i < m_idsAttrPtr.GetCount(); i++){
		if (messageId == m_idsAttrPtr[i]){
			return true;
		}
	}

	return false;
}

CEventItemBase* CEventFactoryCompBase::CreateInstanceWithSlaveFactory(const ilog::IMessageConsumer::MessagePtr& message) const
{
	if (m_slaveEventFactoryCompPtr.IsValid()){
		return m_slaveEventFactoryCompPtr->CreateInstance(message);
	}

	return nullptr;
}


// reimplemented (imtloggui::IEventItemFactory)

QVector<int> CEventFactoryCompBase::GetSupportedMessageIds() const
{
	QVector<int> ids;

	if (m_idsAttrPtr.IsValid()){
		for (int i = 0; i < m_idsAttrPtr.GetCount(); i++){
			ids.append(m_idsAttrPtr[i]);
		}
	}

	if (m_slaveEventFactoryCompPtr.IsValid()){
		ids.append(m_slaveEventFactoryCompPtr->GetSupportedMessageIds());
	}

	return ids;
}


} // namespace imtloggui


