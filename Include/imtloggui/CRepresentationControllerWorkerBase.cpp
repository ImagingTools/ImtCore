#include <imtloggui/CRepresentationControllerWorkerBase.h>


namespace imtloggui
{


// public methods

CWorker::CWorker()
	:m_state(TS_IDLE)
{
	connect(this, &QThread::finished, this, &CWorker::OnFinished);
}


void CWorker::Start()
{
	if (m_state == TS_IDLE){
		m_state = TS_RUNNING;
		start();
	}
	else{
		m_state = TS_PENDING;
	}
}



// private slots:

void CWorker::OnFinished()
{
	if (m_state == TS_PENDING){
		m_state = TS_IDLE;
		Start();
	}
	else{
		m_state = TS_IDLE;
	}
}


} // namespace imtloggui


