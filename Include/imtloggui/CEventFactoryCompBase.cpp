#include <imtloggui/CEventFactoryCompBase.h>


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


void CEventFactoryCompBase::SetItemMetaInfo(IEventItem* eventItem) const
{
	Q_ASSERT(eventItem != nullptr);

	const istd::IInformationProvider* informationProviderPtr = eventItem->GetInformationProvider();
	Q_ASSERT(informationProviderPtr != nullptr);

	QString status;

	switch (informationProviderPtr->GetInformationCategory()){
	case istd::IInformationProvider::IC_NONE:
		status = QObject::tr("UNKNOWN");
		break;
	case istd::IInformationProvider::IC_INFO:
		status = QObject::tr("OK");
		break;
	case istd::IInformationProvider::IC_WARNING:
		status = QObject::tr("WARNING");
		break;
	case istd::IInformationProvider::IC_ERROR:
		status = QObject::tr("ERROR");
		break;
	case istd::IInformationProvider::IC_CRITICAL:
		status = QObject::tr("CRITICAL");
		break;
	}

	QString timestamp;
	if((*m_timestampFormatCompPtr).isEmpty()){
		timestamp = informationProviderPtr->GetInformationTimeStamp().toString();
	}
	else{
		timestamp = informationProviderPtr->GetInformationTimeStamp().toString(*m_timestampFormatCompPtr);
	}

	eventItem->SetMetaInfo(QObject::tr("Timestamp"), timestamp);
	eventItem->SetMetaInfo(QObject::tr("Source"), informationProviderPtr->GetInformationSource());
	eventItem->SetMetaInfo(QObject::tr("Message"), informationProviderPtr->GetInformationDescription());
	eventItem->SetMetaInfo(QObject::tr("Status"), status);
}


} // namespace imtloggui


