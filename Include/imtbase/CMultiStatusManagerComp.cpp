#include <imtbase/CMultiStatusManagerComp.h>


// ACF includes
#include <istd/CChangeNotifier.h>


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


} // namespace imtbase


