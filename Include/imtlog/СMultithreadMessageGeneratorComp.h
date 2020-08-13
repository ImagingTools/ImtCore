#pragma once


// Qt includes
#include <QtCore/QTimer>
#include <QtCore/QMutexLocker>
#include <QtCore/QThread>

// ACF includes
#include <icomp/CComponentBase.h>
#include <imod/TModelWrap.h>
#include <ilog/TLoggerCompWrap.h>


namespace imtlog
{


class ÑMultithreadMessageGeneratorComp:
			public ilog::TLoggerCompWrap<icomp::CComponentBase>
{
public:
	typedef ilog::TLoggerCompWrap<icomp::CComponentBase> BaseClass;

	I_BEGIN_COMPONENT(ÑMultithreadMessageGeneratorComp);
		I_ASSIGN(m_threadCountAttrPtr, "ThreadCount", "Thread count", true, 1);
		I_ASSIGN(m_messagesFrequencyAttrPtr, "Frequency", "Messages per second", true, 1);
	I_END_COMPONENT;

	void Start();
	void Stop();

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	class MessageGeneratorThread: public QThread
	{
	public:
		explicit MessageGeneratorThread(ÑMultithreadMessageGeneratorComp* parentPtr);
		void Start();
		void Interrupt();
		void SetFrequency(double frequency);

	protected:
		// reimplemented (QThread)
		virtual void run() override;

	private:
		enum ThreadState
		{
			TS_IDLE = 0,
			TS_RUNNING,
			TS_PENDING
		};

		typedef QMap<double, int> Timings;

	private:
		void GenerateTimings(Timings& timings);

	private:
		ÑMultithreadMessageGeneratorComp* m_parentPtr;
		ThreadState m_state;
		QMutex m_mutex;
		int m_frequency;
	};

private:
	I_ATTR(int, m_threadCountAttrPtr);
	I_ATTR(int, m_messagesFrequencyAttrPtr);

	QList<MessageGeneratorThread*> m_threads;
};


} // namespace aculaprod


