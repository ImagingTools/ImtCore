// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QElapsedTimer>
#include <QtCore/QString>

// ImtCore includes
#include <imtcache/CProgressMilestones.h>
#include <imtcache/imtcache.h>


namespace imtcache
{


/**
	Reports how a table load is going to the debug output: progress every 10% with an estimate of the
	time left, and the time spent reading, mapping and flushing. Loads of only a few rows are not
	reported, so routine incremental runs stay quiet.
*/
class CLoadProgress
{
public:
	/// \param readMs time already spent waiting for the source to return its rows.
	CLoadProgress(const QString& tableName, int totalCount, qint64 readMs)
		:m_tableName(tableName),
		m_totalCount(totalCount),
		m_readMs(readMs),
		m_isReported(totalCount >= MIN_ROWS_FOR_PROGRESS),
		m_milestones(totalCount, PROGRESS_STEP_PERCENT)
	{
		if (m_isReported){
			qDebug().noquote() << QStringLiteral("%1: read %2 source rows in %3").arg(m_tableName).arg(m_totalCount).arg(FormatDuration(m_readMs));
		}

		m_timer.start();
	}

	/// Call once for every source row, before mapping it.
	void RowProcessed()
	{
		++m_processedCount;

		const int milestone = m_milestones.Advance(m_processedCount);
		if (!m_isReported || milestone < 0){
			return;
		}

		const qint64 elapsedMs = m_timer.elapsed();
		const qint64 remainingMs = elapsedMs * (m_totalCount - m_processedCount) / m_processedCount;

		qDebug().noquote() << QStringLiteral("%1: %2% (%3 of %4 rows), %5 elapsed, about %6 left")
					.arg(m_tableName)
					.arg(milestone)
					.arg(m_processedCount)
					.arg(m_totalCount)
					.arg(FormatDuration(elapsedMs), FormatDuration(remainingMs));
	}

	void RowSkipped()
	{
		++m_skippedCount;
	}

	/// Call when the last row has been handed over, before flushing.
	void MappingDone()
	{
		m_mapMs = m_timer.restart();
	}

	/// Call after flushing.
	void Finish(int rowsWritten)
	{
		if (!m_isReported){
			return;
		}

		qDebug().noquote() << QStringLiteral("%1: loaded %2 rows (%3 skipped): read %4, mapped %5, flushed %6")
					.arg(m_tableName)
					.arg(rowsWritten)
					.arg(m_skippedCount)
					.arg(FormatDuration(m_readMs), FormatDuration(m_mapMs), FormatDuration(m_timer.elapsed()));
	}

private:
	static constexpr int PROGRESS_STEP_PERCENT = 10;
	static constexpr int MIN_ROWS_FOR_PROGRESS = 5000;

	QString m_tableName;
	int m_totalCount;
	qint64 m_readMs;
	bool m_isReported;
	CProgressMilestones m_milestones;
	QElapsedTimer m_timer;
	int m_processedCount = 0;
	int m_skippedCount = 0;
	qint64 m_mapMs = 0;
};


} // namespace imtcache
