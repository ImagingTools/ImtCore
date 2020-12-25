#pragma once


// Qt includes
#include <QtCore/QDateTime.h>


namespace imtlog
{


class CTimeRange
{
public:
	CTimeRange();
	CTimeRange(const QDateTime& begin, const QDateTime& end);

	bool IsValid() const;
	const QDateTime& GetBeginTime() const;
	const QDateTime& GetEndTime() const;
	bool SetBeginTime(const QDateTime& time);
	bool SetEndTime(const QDateTime& time);
	bool SetTimeRange(const QDateTime& begin, const QDateTime& end);
	bool Contains(const QDateTime& time) const;
	CTimeRange Intersect(const CTimeRange& other) const;

private:
	QDateTime m_begin;
	QDateTime m_end;
};


} // namespace imtlog


