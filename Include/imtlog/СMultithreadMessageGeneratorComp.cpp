#include <imtlog/ÑMultithreadMessageGeneratorComp.h>


// Qt includes
#include <QtCore/QRandomGenerator>

// Acula includes
//#include <aculaprod/CProducingPartEvent.h>


namespace imtlog
{


// public methods

void ÑMultithreadMessageGeneratorComp::Start()
{
	int threadCount = *m_threadCountAttrPtr;
	int frequency = *m_messagesFrequencyAttrPtr;

	if (threadCount < 1 || threadCount > 10){
		threadCount = 1;
	}

	if (frequency < 1 || frequency > 10000){
		frequency = 1;
	}

	for (int i = 0; i < threadCount; i++){
		MessageGeneratorThread* threadPtr = new MessageGeneratorThread(this);
		threadPtr->SetFrequency(frequency);
		threadPtr->Start();
		m_threads.append(threadPtr);
	}
}


void ÑMultithreadMessageGeneratorComp::Stop()
{
	for (int i = 0; i < m_threads.count(); i++){
		m_threads[i]->Interrupt();
		m_threads[i]->wait();
		delete m_threads[i];
	}

	m_threads.clear();
}


// protected methods

// reimplemented (icomp::CComponentBase)
void ÑMultithreadMessageGeneratorComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	Start();
}


void ÑMultithreadMessageGeneratorComp::OnComponentDestroyed()
{
	Stop();

	BaseClass::OnComponentDestroyed();
}


// public methods of embedded class ÑMessageGeneratorThread

ÑMultithreadMessageGeneratorComp::MessageGeneratorThread::MessageGeneratorThread(ÑMultithreadMessageGeneratorComp* parentPtr)
	:m_parentPtr(parentPtr),
	m_state(TS_IDLE)
{
}

void ÑMultithreadMessageGeneratorComp::MessageGeneratorThread::Start()
{
	QMutexLocker statusLocker(&m_mutex);

	if (m_state == TS_IDLE){
		wait();
		m_state = TS_RUNNING;
		start();
	}
}


void ÑMultithreadMessageGeneratorComp::MessageGeneratorThread::Interrupt()
{
	requestInterruption();
}


void ÑMultithreadMessageGeneratorComp::MessageGeneratorThread::SetFrequency(double frequency)
{
	m_frequency = frequency;
}


// protected methods of embedded class MessageGeneratorThread

// reimplemented (QThread)

void ÑMultithreadMessageGeneratorComp::MessageGeneratorThread::run()
{
	while (!isInterruptionRequested()){
		Timings timings;
		GenerateTimings(timings);
		
		for (double time : timings.keys()){
			if (isInterruptionRequested()){
				QMutexLocker statusLocker(&m_mutex);
				m_state = TS_IDLE;
				exit();
			}

			msleep(time);

			for (int i = 0; i < timings[time]; i++){
				m_parentPtr->SendInfoMessage(0, tr("Automatic generated message"), "");
			}
		}

		double delay = 1000 - timings.lastKey();

		msleep(delay);
	}

	QMutexLocker statusLocker(&m_mutex);
	m_state = TS_IDLE;
}


// protected methods of embedded class MessageGeneratorThread

void ÑMultithreadMessageGeneratorComp::MessageGeneratorThread::GenerateTimings(ÑMultithreadMessageGeneratorComp::MessageGeneratorThread::Timings& timings)
{
	timings.clear();

	QRandomGenerator randomGenerator = QRandomGenerator::securelySeeded();
	for (int i = 0; i < m_frequency; i++){
		timings[randomGenerator.generateDouble() * 1000]++;
	}
}


} // namespace imtlog


