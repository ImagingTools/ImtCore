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


bool CTimeRange::IsNull() const
{
	return !(m_begin.isValid() || m_end.isValid());
}


bool CTimeRange::IsClosed() const
{
	return m_begin.isValid() && m_end.isValid();
}


bool CTimeRange::IsOpened() const
{
	return m_begin.isValid() != m_end.isValid();
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
	//if (m_begin.isValid()){
	//	if (m_begin > time){
	//		m_begin = time;
	//		return true;
	//	}

	//	return false;
	//}
	
	m_begin = time;

	return true;
}


bool CTimeRange::SetEndTime(const QDateTime& time)
{
	//if (m_end.isValid()){
	//	if (m_end < time){
	//		m_end = time;
	//		return true;
	//	}

	//	return false;
	//}

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


bool CTimeRange::Contains(const QDateTime& time) const
{
	if (m_begin.isValid() && time < m_begin){
		return false;
	}

	if (m_end.isValid() && time > m_end){
		return false;
	}

	return true;
}


void CTimeRange::Ensure(const QDateTime& time)
{
	if (IsNull()){
		m_begin = time;
		m_end = time;
		return;
	}

	if (IsClosed()){
		if (time < m_begin){
			m_begin = time;
		}

		if (time > m_end){
			m_end = time;
		}
	}
}


CTimeRange CTimeRange::Intersect(const CTimeRange& other) const
{
	Q_ASSERT(m_begin.isValid() && m_end.isValid() && other.m_begin.isValid() && other.m_end.isValid());

	if (!IsClosed() || !other.IsClosed()){
		return CTimeRange();
	}
	
	if (other.m_begin <= m_begin){
		if (other.m_end >= m_end){
			return CTimeRange(m_begin, m_end);
		}

		if (other.m_end >= m_begin){
			return CTimeRange(m_begin, other.m_end);
		}

		return CTimeRange();
	}

	if (other.m_begin <= m_end){
		if (other.m_end >= m_end){
			return CTimeRange(other.m_begin, m_end);
		}

		return CTimeRange(other.m_begin, other.m_end);
	}

	return CTimeRange();
}


bool CTimeRange::operator==(const CTimeRange& other) const
{
	return (m_begin == other.m_begin && m_end == other.m_end);
}


bool CTimeRange::operator!=(const CTimeRange& other) const
{
	return (m_begin != other.m_begin || m_end != other.m_end);
}


} // namespace imtlog


