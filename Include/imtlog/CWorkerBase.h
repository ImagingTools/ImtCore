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


