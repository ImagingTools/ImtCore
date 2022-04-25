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
	:m_timeUnit(TU_CUSTOM),
	m_interpretationMode(IM_FOR),
	m_timeUnitMultiplier(1),
	m_timeRange(QDateTime(), QDateTime())
{
}


imtbase::CTimeRange CTimeFilterParam::GetTimeRange() const
{
	switch (m_timeUnit){
	case TU_CUSTOM:
		return m_timeRange;
	case TU_HOUR:
		switch (m_interpretationMode){
		case IM_CURRENT:
			Q_ASSERT(false);
			break;
		case IM_FOR:
			return imtbase::CTimeRange(QDateTime::currentDateTime().addSecs(-60 * 60 * m_timeUnitMultiplier), QDateTime::currentDateTime());
			break;
		case IM_LAST:
			Q_ASSERT(false);
			break;
		}
		break;
	case TU_DAY:
		switch (m_interpretationMode){
		case IM_CURRENT:
			Q_ASSERT(false);
			break;
		case IM_FOR:
			return imtbase::CTimeRange(QDateTime::currentDateTime().addDays(-m_timeUnitMultiplier), QDateTime::currentDateTime());
			break;
		case IM_LAST:
			Q_ASSERT(false);
			break;
		}
		break;
	case TU_WEEK:
		switch (m_interpretationMode){
		case IM_CURRENT:
			Q_ASSERT(false);
			break;
		case IM_FOR:
			return imtbase::CTimeRange(QDateTime::currentDateTime().addDays(-7 * m_timeUnitMultiplier), QDateTime::currentDateTime());
			break;
		case IM_LAST:
			Q_ASSERT(false);
			break;
		}
		break;
	case TU_MONTH:
		switch (m_interpretationMode){
		case IM_CURRENT:
			Q_ASSERT(false);
			break;
		case IM_FOR:
			return imtbase::CTimeRange(QDateTime::currentDateTime().addMonths(-m_timeUnitMultiplier), QDateTime::currentDateTime());
			break;
		case IM_LAST:
			Q_ASSERT(false);
			break;
		}
		break;
	case TU_YEAR:
		switch (m_interpretationMode){
		case IM_CURRENT:
			Q_ASSERT(false);
			break;
		case IM_FOR:
			return imtbase::CTimeRange(QDateTime::currentDateTime().addYears(-m_timeUnitMultiplier), QDateTime::currentDateTime());
			break;
		case IM_LAST:
			Q_ASSERT(false);
			break;
		}
		break;
	}

	return CTimeRange();
}


void CTimeFilterParam::SetTimeRange(const imtbase::CTimeRange& timeRange)
{
	if (m_timeRange != timeRange){
		istd::CChangeNotifier changeNotifier(this);

		m_timeRange = timeRange;
	}
}


ITimeFilterParam::TimeUnit CTimeFilterParam::GetTimeUnit() const
{
	return m_timeUnit;
}


void CTimeFilterParam::SetTimeUnit(TimeUnit timeUnit, InterpretationMode interpretationMode, int multiplier)
{
	if ((m_timeUnit != timeUnit) || (m_interpretationMode != interpretationMode) || (m_timeUnitMultiplier != multiplier)){
		istd::CChangeNotifier changeNotifier(this);

		m_timeUnit = timeUnit;
		m_interpretationMode = interpretationMode;
		m_timeUnitMultiplier = multiplier;
	}
}


ITimeFilterParam::InterpretationMode CTimeFilterParam::GetInterpretationMode() const
{
	return m_interpretationMode;
}


int CTimeFilterParam::GetUnitMultiplier() const
{
	return m_timeUnitMultiplier;
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

	static iser::CArchiveTag timeUnitTag("TimeUnit", "Predefined time unit", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(timeUnitTag);
	retVal = retVal && I_SERIALIZE_ENUM(TimeUnit, archive, m_timeUnit);
	retVal = retVal && archive.EndTag(timeUnitTag);

	static iser::CArchiveTag interpretationModeTag("InterpretationMode", "Interpretation of the time unit", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(interpretationModeTag);
	retVal = retVal && I_SERIALIZE_ENUM(InterpretationMode, archive, m_interpretationMode);
	retVal = retVal && archive.EndTag(interpretationModeTag);

	static iser::CArchiveTag timeUnitMultiplierTag("UnitMulitplier", "Unit multiplication factor", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(timeUnitMultiplierTag);
	retVal = retVal && archive.Process(m_timeUnitMultiplier);
	retVal = retVal && archive.EndTag(timeUnitMultiplierTag);

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
		m_timeUnit = implPtr->m_timeUnit;
		m_interpretationMode = implPtr->m_interpretationMode;
		m_timeUnitMultiplier = implPtr->m_timeUnitMultiplier;

		return true;
	}

	return false;
}


bool CTimeFilterParam::IsEqual(const IChangeable& object) const
{
	const CTimeFilterParam* implPtr = dynamic_cast<const CTimeFilterParam*>(&object);
	if (implPtr != nullptr){
		return 
					(m_timeRange == implPtr->m_timeRange) &&
					(m_timeUnit == implPtr->m_timeUnit) &&
					(m_interpretationMode == implPtr->m_interpretationMode) &&
					(m_timeUnitMultiplier == implPtr->m_timeUnitMultiplier);
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

	m_timeUnit = TU_CUSTOM;
	m_interpretationMode = IM_FOR;
	m_timeUnitMultiplier = 1;

	return true;
}


}


