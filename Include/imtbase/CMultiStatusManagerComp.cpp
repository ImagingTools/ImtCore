#include <imtbase/CMultiStatusManagerComp.h>


namespace imtbase
{


// public methods

// reimplemented (imtbase::IMultiStatusManager)

void CMultiStatusManagerComp::SetStatus(const QByteArray& statusId, const istd::IInformationProvider& status)
{
	istd::CChangeNotifier changeNotifier(this);

	int statusIndex = m_statusInfo.GetOptionIndexById(statusId);
	if (statusIndex >= 0 && statusIndex < m_informationProvidersCompPtr.GetCount()){
		istd::IInformationProvider* statusPtr = m_informationProvidersCompPtr[statusIndex];
		Q_ASSERT(statusPtr != nullptr);

		statusPtr->CopyFrom(status);
	}
}


// reimplemented (imtbase::IMultiStatusProvider)

const iprm::IOptionsList& CMultiStatusManagerComp::GetStatusInfoList() const
{
	return m_statusInfo;
}


const istd::IInformationProvider* CMultiStatusManagerComp::GetStatusInfo(const QByteArray& id) const
{
	int statusIndex = m_statusInfo.GetOptionIndexById(id);

	if (statusIndex >= 0 && statusIndex < m_informationProvidersCompPtr.GetCount()){
		return m_informationProvidersCompPtr[statusIndex];
	}

	return nullptr;
}


// reimplemented (iinsp::IInformationProvider)

QDateTime CMultiStatusManagerComp::GetInformationTimeStamp() const
{
	QDateTime retVal;

	for (int i = 0; i < m_informationProvidersCompPtr.GetCount(); ++i){
		const istd::IInformationProvider* informationProviderPtr = m_informationProvidersCompPtr[i];

		if (informationProviderPtr){
			QDateTime timeStamp = informationProviderPtr->GetInformationTimeStamp();

			if (!retVal.isNull()){
				retVal = timeStamp; 
			}
			else if (retVal < timeStamp){
				retVal = timeStamp;
			}
		}
	}

	return retVal;
}


istd::IInformationProvider::InformationCategory CMultiStatusManagerComp::GetInformationCategory() const
{
	InformationCategory retVal = IC_NONE;

	for (int i = 0; i < m_informationProvidersCompPtr.GetCount(); ++i){
		const istd::IInformationProvider* informationProviderPtr = m_informationProvidersCompPtr[i];

		if (informationProviderPtr){
			retVal = qMax(retVal, informationProviderPtr->GetInformationCategory());
		}
	}

	return retVal;
}


int CMultiStatusManagerComp::GetInformationId() const
{
	return -1;
}


QString CMultiStatusManagerComp::GetInformationDescription() const
{
	return m_defaultDescriptionAttrPtr.IsValid() ? *m_defaultDescriptionAttrPtr : QString();
}


QString CMultiStatusManagerComp::GetInformationSource() const
{
	return m_defaultSourceAttrPtr.IsValid() ? *m_defaultSourceAttrPtr : QString();
}


int CMultiStatusManagerComp::GetInformationFlags() const
{
	return -1;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CMultiStatusManagerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	Q_ASSERT(m_informationProvidersCompPtr.GetCount() == m_statusIdsAttrPtr.GetCount());
	Q_ASSERT(m_informationProvidersCompPtr.GetCount() == m_statusNamesAttrPtr.GetCount());
	Q_ASSERT(m_informationProvidersCompPtr.GetCount() == m_statusDescriptionsAttrPtr.GetCount());

	for (int i = 0; i < m_informationProvidersCompPtr.GetCount(); ++i){
		m_statusInfo.InsertOption(m_statusNamesAttrPtr[i], m_statusIdsAttrPtr[i], m_statusDescriptionsAttrPtr[i]);
	}
}


} // namespace imtbase
