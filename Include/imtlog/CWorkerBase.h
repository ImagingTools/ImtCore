// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QThread>
#include <QtCore/QMutex>


namespace imtlog
{


class CWorkerBase: public QThread
{
	Q_OBJECT
public:
	CWorkerBase();

	void Start();

private Q_SLOTS:
	void OnFinished();

private:
	enum ThreadState
	{
		TS_IDLE = 0,
		TS_RUNNING,
		TS_PENDING
	};

	ThreadState m_state;
	QMutex m_stateMutex;
};


} // namespace imtlog


