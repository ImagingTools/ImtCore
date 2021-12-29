#include <imtbase/CTimeFilterParam.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtbase
{


// public methods

CTimeFilterParam::CTimeFilterParam()
	:m_timeInterval(TI__CUSTOM)
{
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


ITimeFilterParam::TimeInterval CTimeFilterParam::GetTimeInterval() const
{
	return m_timeInterval;
}


void CTimeFilterParam::SetTimeInterval(TimeInterval timeInterval)
{
	if (m_timeInterval != timeInterval){
		istd::CChangeNotifier changeNotifier(this);

		m_timeInterval = timeInterval;
	}
}


// reimplemented (iser::ISerializable)

bool CTimeFilterParam::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	static iser::CArchiveTag timeRangeTag("TimeRange", "User-defined time range", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(timeRangeTag);
	retVal = retVal && m_timeRange.Serialize(archive);
	retVal = retVal && archive.EndTag(timeRangeTag);

	static iser::CArchiveTag timeIntervalTag("TimeInterval", "Predefined time interval", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(timeIntervalTag);
	retVal = retVal && I_SERIALIZE_ENUM(TimeInterval, archive, m_timeInterval);
	retVal = retVal && archive.EndTag(timeIntervalTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CTimeFilterParam::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE | SO_COMPARE | SO_RESET;
}


bool CTimeFilterParam::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CTimeFilterParam* implPtr = dynamic_cast<const CTimeFilterParam*>(&object);
	if (implPtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_timeRange = implPtr->m_timeRange;
		m_timeInterval = implPtr->m_timeInterval;

		return true;
	}

	return false;
}


bool CTimeFilterParam::IsEqual(const IChangeable& object) const
{
	const CTimeFilterParam* implPtr = dynamic_cast<const CTimeFilterParam*>(&object);
	if (implPtr != nullptr){
		return (m_timeRange == implPtr->m_timeRange) && (m_timeInterval == implPtr->m_timeInterval);
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

	m_timeInterval = TI__CUSTOM;

	return true;
}


}


