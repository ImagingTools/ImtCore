#include <imtlog/CTimeRange.h>


namespace imtlog
{


// public methods

CTimeRange::CTimeRange()
{
}


CTimeRange::CTimeRange(const QDateTime& begin, const QDateTime& end)
{
	if (begin.isValid() && end.isValid()){
		if (begin <= end){
			m_begin = begin;
			m_end = end;
		}

		return;
	}

	m_begin = begin;
	m_end = end;

	return;
}

bool CTimeRange::IsValid() const
{
	if (!m_begin.isValid() && !m_end.isValid()){
		return false;
	}

	return true;
}


const QDateTime& CTimeRange::GetBeginTime() const
{
	return m_begin;
}


const QDateTime& CTimeRange::GetEndTime() const
{
	return m_end;
}


bool CTimeRange::SetBeginTime(const QDateTime& time)
{
	if (m_begin.isValid()){
		if (m_begin > time){
			m_begin = time;
			return true;
		}

		return false;
	}
	
	m_begin = time;

	return true;
}


bool CTimeRange::SetEndTime(const QDateTime& time)
{
	if (m_end.isValid()){
		if (m_end < time){
			m_end = time;
			return true;
		}

		return false;
	}

	m_end = time;

	return true;
}


bool CTimeRange::SetTimeRange(const QDateTime& begin, const QDateTime& end)
{
	if (begin.isValid() && end.isValid()){
		if (begin <= end){
			m_begin = begin;
			m_end = end;
			return true;
		}

		return false;
	}

	m_begin = begin;
	m_end = end;

	return true;
}


CTimeRange CTimeRange::Intersect(const CTimeRange& /*other*/) const
{
	return CTimeRange();

	//if (!IsValid() || !other.IsValid()){
	//	return CTimeRange();
	//}

	//if (m_begin.isValid && m_end.isValid() && other.m_begin.isValid() && other.m_end.isValid()){

	//}
}


} // namespace imtlog


