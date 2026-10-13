// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QtGlobal>


namespace imtcache
{


/// Tells a long loop when it has crossed the next N% of its work, so it can log progress that often and no more.
class CProgressMilestones
{
public:
	CProgressMilestones(qint64 totalCount, int stepPercent = 10)
		:m_totalCount(totalCount),
		m_stepPercent(qMax(1, stepPercent)),
		m_lastMilestone(0)
	{
	}

	/**
		Returns the milestone (a multiple of the step, at most 100) that \a processedCount has newly
		reached, or -1 when it has not reached a new one. A jump over several milestones reports the last.
	*/
	int Advance(qint64 processedCount)
	{
		if (m_totalCount <= 0){
			return -1;
		}

		const int percent = static_cast<int>(qMin<qint64>(100, processedCount * 100 / m_totalCount));
		const int milestone = percent / m_stepPercent * m_stepPercent;
		if (milestone <= m_lastMilestone){
			return -1;
		}

		m_lastMilestone = milestone;

		return milestone;
	}

private:
	qint64 m_totalCount;
	int m_stepPercent;
	int m_lastMilestone;
};


} // namespace imtcache
