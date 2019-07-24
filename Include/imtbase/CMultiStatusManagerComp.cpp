#include <imtbase/CMultiStatusManagerComp.h>


namespace imtbase
{


// public methods

// reimplemented (imtbase::IMultiStatusManager)

void CMultiStatusManagerComp::SetStatus(const QByteArray& statusId, const istd::IInformationProvider& status)
{
	int statusIndex = m_statusInfo.GetOptionIndexById(statusId);

	if (statusIndex >= 0 && statusIndex < m_informationProvidersCompPtr.GetCount()){
		istd::IInformationProvider* statusPtr = m_informationProvidersCompPtr[statusIndex];
		Q_ASSERT(statusPtr != nullptr);

		statusPtr->CopyFrom(status);
	}
}


} // namespace imtbase


