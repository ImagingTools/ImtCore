// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDateTime>

// ACF includes
#include <iser/IArchive.h>


namespace imtbase
{


class CTimeRange
{
public:
	CTimeRange();
	CTimeRange(const QDateTime& begin, const QDateTime& end);

	bool IsNull() const;
	bool IsClosed() const;
	bool IsOpened() const;
	const QDateTime& GetBeginTime() const;
	const QDateTime& GetEndTime() const;
	bool SetBeginTime(const QDateTime& time);
	bool SetEndTime(const QDateTime& time);
	bool SetTimeRange(const QDateTime& begin, const QDateTime& end);
	void Clear();
	bool Contains(const QDateTime& time) const;
	void Ensure(const QDateTime& time);
	CTimeRange Intersect(const CTimeRange& other) const;

	bool operator==(const CTimeRange& other) const;
	bool operator!=(const CTimeRange& other) const;

	bool Serialize(iser::IArchive& archive);

private:
	QDateTime m_begin;
	QDateTime m_end;
};


} // namespace imtbase


