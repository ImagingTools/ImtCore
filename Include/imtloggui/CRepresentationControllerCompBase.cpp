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
	connect(this, &CRepresentationControllerCompBase::EmitRepresentationCreated, this, &CRepresentationControllerCompBase::OnRepresentationCreated, Qt::QueuedConnection);
}


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

void CRepresentationControllerCompBase::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (m_representationCompPtr.IsValid() && m_eventProviderCompPtr.IsValid()){
		imtlog::CTimeRange timeRange = GetObservedObject()->GetTimeRange();

		if (timeRange.IsClosed()){
			QMutexLocker locker(&m_workerQueueMutex);
			m_workerQueue.enqueue(timeRange);
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

	if (m_representationFactPtr.IsValid()){
		m_worker.start();
	}
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


// private slots

void CRepresentationControllerCompBase::OnRepresentationCreated()
{
	if (m_representationCompPtr.IsValid()){
		QMutexLocker locker(&m_representationQueueMutex);
		if (!m_representationQueue.isEmpty()){
			RepresentationCompPtr representationCompPtr = m_representationQueue.back();
			m_representationQueue.clear();
			locker.unlock();			

			istd::IChangeable* representationPtr = m_representationFactPtr.ExtractInterface(representationCompPtr.GetPtr());
			if (representationPtr != nullptr){
				m_representationCompPtr->CopyFrom(*representationPtr);
			}

		}
	}
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
				QMutexLocker locker(&m_parent.m_workerQueueMutex);
				m_parent.m_workerQueue.enqueue(timeRange);
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
	while (!isInterruptionRequested()){
		QMutexLocker locker(&m_parent.m_workerQueueMutex);
		if (!m_parent.m_workerQueue.isEmpty()){
			imtlog::CTimeRange jobTimeRange = m_parent.m_workerQueue.back();
			m_parent.m_workerQueue.clear();
			locker.unlock();

			if (jobTimeRange.IsClosed()){
				RepresentationCompPtr representationCompPtr(m_parent.m_representationFactPtr.CreateComponent());
				if (representationCompPtr.IsValid()){
					istd::IChangeable* representationPtr = m_parent.m_representationFactPtr.ExtractInterface(representationCompPtr.GetPtr());
					if (representationPtr != nullptr){
						m_parent.m_messageFilterParamsCompPtr->SetFilterTimeRange(jobTimeRange);

						imtlog::IEventProvider::EventContainerPtr containerPtr = m_parent.m_eventProviderCompPtr->GetEvents(
							m_parent.m_eventFilterCompPtr.GetPtr(),
							m_parent.m_messageFilterParamsCompPtr.GetPtr());

						if (containerPtr.IsValid()){
							imtlog::CEventHistoryResultContainer* eventHistoryResultContainerPtr = dynamic_cast<imtlog::CEventHistoryResultContainer*>(containerPtr.GetPtr());
							Q_ASSERT(eventHistoryResultContainerPtr != nullptr);

							while (!eventHistoryResultContainerPtr->IsClosed() && !isInterruptionRequested()){
								msleep(1);
							}

							if (!isInterruptionRequested()){
								m_parent.BuildRepresentation(
											*representationPtr,
											containerPtr,
											jobTimeRange);

								QMutexLocker representationQueueLocker(&m_parent.m_representationQueueMutex);
								m_parent.m_representationQueue.enqueue(representationCompPtr);
								representationQueueLocker.unlock();

								Q_EMIT m_parent.EmitRepresentationCreated();
							}
						}
					}
				}
			}
		}
		else{
			locker.unlock();
		}

		msleep(1);
	}
}


} // namespace imtloggui


