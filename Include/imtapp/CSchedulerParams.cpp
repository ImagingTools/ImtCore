#include <imtapp/CSchedulerParams.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtapp
{


// public methods

// reimplemented (ISchedulerParams)

const QDateTime& CSchedulerParams::GetStartTime() const
{
	return m_startTime;
}


void CSchedulerParams::SetStartTime(const QDateTime& startTime)
{
	if (m_startTime != startTime){
		istd::CChangeNotifier notifier(this);

		m_startTime = startTime;
	}
}


int CSchedulerParams::GetInterval() const
{
	return m_interval;
}


void CSchedulerParams::SetInterval(int interval)
{
	if (m_interval != interval){
		istd::CChangeNotifier notifier(this);

		m_interval = interval;
	}
}


// reimplemented (iser::ISerializable)

bool CSchedulerParams::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	iser::CArchiveTag intervalTag("Interval", "Interval", iser::CArchiveTag::TT_LEAF);
	bool retVal = archive.BeginTag(intervalTag);
	retVal = retVal && archive.Process(m_interval);
	retVal = retVal && archive.EndTag(intervalTag);

	iser::CArchiveTag startTimeTag("StartTime", "Start time", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(startTimeTag);
	QString startTime = m_startTime.toString("dd-MM-yyyy HH:mm");
	retVal = retVal && archive.Process(startTime);
	retVal = retVal && archive.EndTag(startTimeTag);

	if (!archive.IsStoring()){
		m_startTime = QDateTime::fromString(startTime, "dd-MM-yyyy HH:mm");
	}

	return retVal;
}


// reimplemented (istd::IChangeable)

int CSchedulerParams::GetSupportedOperations() const
{
	return SO_COPY | SO_COMPARE | SO_CLONE | SO_RESET;
}


bool CSchedulerParams::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	const CSchedulerParams* sourcePtr = dynamic_cast<const CSchedulerParams*>(&object);
	if (sourcePtr != nullptr){
		m_startTime = sourcePtr->m_startTime;
		m_interval = sourcePtr->m_interval;

		return true;
	}

	return true;
}


bool CSchedulerParams::IsEqual(const IChangeable& object) const
{
	const ISchedulerParams* sourcePtr = dynamic_cast<const ISchedulerParams*>(&object);
	if (sourcePtr != nullptr){
		return	m_startTime == sourcePtr->GetStartTime() &&
				m_interval == sourcePtr->GetInterval();
	}

	return false;
}


istd::IChangeable* CSchedulerParams::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CSchedulerParams> clonePtr(new CSchedulerParams);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CSchedulerParams::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_interval = 0;

	return true;
}


} // namespace imtapp


