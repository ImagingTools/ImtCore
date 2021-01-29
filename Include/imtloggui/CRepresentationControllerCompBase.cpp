#include <imtloggui/CRepresentationControllerCompBase.h>


// Qt includes
#include <QtCore/QDebug>

// ImtCore includes
#include <imtlog/CEventHistoryResultContainer.h>


namespace imtloggui
{


// public methods

CRepresentationControllerCompBase::CRepresentationControllerCompBase()
	:m_eventProviderObserver(*this),
	m_worker(*this)
{
}


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

void CRepresentationControllerCompBase::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (m_representationCompPtr.IsValid() && m_eventProviderCompPtr.IsValid()){
		imtlog::CTimeRange timeRange = GetObservedObject()->GetTimeRange();

		if (timeRange.IsClosed()){
			QMutexLocker locker(&m_workerJobMutex);
			m_worker.requestInterruption();
			m_workerJob = timeRange;
			m_worker.Start();
		}
	}
}


// reimplemented (icomp::CComponentBase)

void CRepresentationControllerCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_timeRangeProviderCompPtr.IsValid() && m_timeRangeProviderModelCompPtr.IsValid()){
		m_timeRangeProviderModelCompPtr->AttachObserver(this);
	}

	if (m_eventProviderCompPtr.IsValid() && m_eventProviderModelCompPtr.IsValid()){
		m_eventProviderModelCompPtr->AttachObserver(&m_eventProviderObserver);
	}
}


// private slots

void CRepresentationControllerCompBase::OnTaskFinished(QByteArray taskId)
{
	//qDebug() << taskId;
}


void CRepresentationControllerCompBase::OnComponentDestroyed()
{
	m_worker.requestInterruption();
	m_worker.wait();

	if (m_timeRangeProviderModelCompPtr.IsValid() && m_timeRangeProviderModelCompPtr->IsAttached(this)){
		m_timeRangeProviderModelCompPtr->DetachObserver(this);
	}

	if (m_eventProviderCompPtr.IsValid() && m_eventProviderModelCompPtr->IsAttached(this)){
		m_eventProviderModelCompPtr->DetachObserver(&m_eventProviderObserver);
	}

	BaseClass::OnComponentDestroyed();
}


// public methods of the embedded class EventProviderObserver

CRepresentationControllerCompBase::EventProviderObserver::EventProviderObserver(CRepresentationControllerCompBase & parent)
	:m_parent(parent)
{
}


// protected methods of the embedded class EventProviderObserver

// reimplemented (imod::CSingleModelObserverBase)

void CRepresentationControllerCompBase::EventProviderObserver::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (m_parent.m_representationCompPtr.IsValid() && m_parent.m_eventProviderCompPtr.IsValid()){
		if (m_parent.m_timeRangeProviderCompPtr.IsValid()){
			imtlog::CTimeRange timeRange = m_parent.GetObservedObject()->GetTimeRange();

			if (timeRange.IsClosed()){
				QMutexLocker locker(&m_parent.m_workerJobMutex);
				m_parent.m_worker.requestInterruption();
				m_parent.m_workerJob = timeRange;
				m_parent.m_worker.Start();
			}
		}
	}
}


// protected methods of the embedded class Worker

CRepresentationControllerCompBase::Worker::Worker(CRepresentationControllerCompBase& parent)
	:m_parent(parent)
{
}


void CRepresentationControllerCompBase::Worker::run()
{
	bool isFinished = false;

	while (!isFinished){
		QMutexLocker locker(&m_parent.m_workerJobMutex);
		imtlog::CTimeRange jobTimeRange = m_parent.m_workerJob;
		m_parent.m_workerJob.Clear();
		locker.unlock();

		if (jobTimeRange.IsClosed()){
			m_parent.m_messageFilterParamsCompPtr->SetFilterTimeRange(jobTimeRange);

			imtlog::IEventProvider::EventContainerPtr containerPtr = m_parent.m_eventProviderCompPtr->GetEvents(
						m_parent.m_eventFilterCompPtr.GetPtr(),
						m_parent.m_messageFilterParamsCompPtr.GetPtr());

			if (containerPtr.IsValid()){
				imtlog::CEventHistoryResultContainer* eventHistoryResultContainer =
							dynamic_cast<imtlog::CEventHistoryResultContainer*>(containerPtr.GetPtr());

				while (!eventHistoryResultContainer->IsClosed() && !isFinished){
					msleep(1);
					if (isInterruptionRequested()){
						isFinished = true;
					}
				}

				if (!isFinished){
					m_parent.BuildRepresentation(
								*m_parent.m_representationCompPtr,
								containerPtr,
								jobTimeRange);
				}
			}
		}
		else{
			isFinished = true;
		}
	}
}


} // namespace imtloggui


