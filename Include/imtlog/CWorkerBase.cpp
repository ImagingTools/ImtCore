#include <imtlog/CWorkerBase.h>


namespace imtlog
{


// public methods

CWorkerBase::CWorkerBase()
	:m_state(TS_IDLE)
{
	connect(this, &QThread::finished, this, &CWorkerBase::OnFinished);
}


void CWorkerBase::Start()
{
	QMutexLocker locker(&m_stateMutex);
	if (m_state == TS_IDLE){
		m_state = TS_RUNNING;
		start();
	}
	else{
		m_state = TS_PENDING;
	}
}



// private slots:

void CWorkerBase::OnFinished()
{
	QMutexLocker locker(&m_stateMutex);
	if (m_state == TS_PENDING){
		m_state = TS_RUNNING;
		start();
	}
	else{
		m_state = TS_IDLE;
	}
}


} // namespace imtlog


