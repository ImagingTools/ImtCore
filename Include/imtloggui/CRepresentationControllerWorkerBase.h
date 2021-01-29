#pragma once


// Qt includes
#include <QtCore/QThread>


namespace imtloggui
{


class CWorker: public QThread
{
	Q_OBJECT
public:
	CWorker();

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
};


} // namespace imtloggui


