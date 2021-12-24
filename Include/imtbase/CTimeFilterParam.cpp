#include <imtbase/CTimeFilterParam.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>


namespace imtbase
{


// public methods

CTimeFilterParam::CTimeFilterParam()
{
	ResetData();
}


const imtbase::CTimeRange& CTimeFilterParam::GetTimeRange() const
{
	return m_timeRange;
}


void CTimeFilterParam::SetTimeRange(const imtbase::CTimeRange& timeRange)
{
	if (m_timeRange != timeRange){
		istd::CChangeNotifier changeNotifier(this);

		m_timeRange = timeRange;
	}
}


// reimplemented (iser::ISerializable)

bool CTimeFilterParam::Serialize(iser::IArchive& /*archive*/)
{
	return false;
}


// reimplemented (istd::IChangeable)

int CTimeFilterParam::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE | SO_COMPARE | SO_RESET;
}


bool CTimeFilterParam::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CTimeFilterParam* messageFilterParamsPtr = dynamic_cast<const CTimeFilterParam*>(&object);
	if (messageFilterParamsPtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_timeRange = messageFilterParamsPtr->m_timeRange;

		return true;
	}

	return false;
}


bool CTimeFilterParam::IsEqual(const IChangeable& object) const
{
	const CTimeFilterParam* messageFilterParamsPtr = dynamic_cast<const CTimeFilterParam*>(&object);
	if (messageFilterParamsPtr != nullptr){
		return m_timeRange == messageFilterParamsPtr->m_timeRange;
	}

	return false;
}


istd::IChangeable* CTimeFilterParam::CloneMe(istd::IChangeable::CompatibilityMode mode) const
{
	istd::TDelPtr<CTimeFilterParam> clonePtr(new CTimeFilterParam());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CTimeFilterParam::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_timeRange = imtbase::CTimeRange();

	return true;
}


}


