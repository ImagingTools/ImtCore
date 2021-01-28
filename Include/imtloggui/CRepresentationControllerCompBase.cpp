#include <imtloggui/CRepresentationControllerCompBase.h>


namespace imtloggui
{


// public methods

CRepresentationControllerCompBase::CRepresentationControllerCompBase()
	:m_eventProviderObserver(*this),
	m_workerObjectPtr(nullptr)
{
	connect(&m_workerThread, &QThread::started, this, & CRepresentationControllerCompBase::OnWorkerThreadStarted);
}


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

void CRepresentationControllerCompBase::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (m_representationCompPtr.IsValid() && m_eventProviderCompPtr.IsValid()){
		if (m_workerObjectPtr != nullptr){
			m_workerObjectPtr->AddJob(GetObservedObject()->GetTimeRange());
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

	m_workerThread.start();
}


void CRepresentationControllerCompBase::OnComponentDestroyed()
{
	m_workerObjectPtr = nullptr;
	m_workerObjectPtr->deleteLater();
	m_workerThread.quit();
	m_workerThread.wait();

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
			if (m_parent.m_workerObjectPtr != nullptr){
				m_parent.m_workerObjectPtr->AddJob(m_parent.m_timeRangeProviderCompPtr->GetTimeRange());
			}
		}
	}
}

// protected slots:

void CRepresentationControllerCompBase::OnWorkerThreadStarted()
{
	m_workerObjectPtr = new Worker(*this);
	m_workerObjectPtr->moveToThread(&m_workerThread);
}


// public methods of the embedded class Worker

CRepresentationControllerCompBase::Worker::Worker(CRepresentationControllerCompBase& parent)
	:m_parent(parent)
{
}

void CRepresentationControllerCompBase::Worker::AddJob(const imtlog::CTimeRange& timeRange)
{
	if (timeRange.IsClosed()){
		QMutexLocker locker(&m_jobsMutex);
		m_jobs.push_back(timeRange);

		Q_EMIT EmitNewJobAdded();
	}
}


// protected methods of the embedded class Worker

// reimplemented (imtloggui::CRepresentationControllerWorkerBase)

void CRepresentationControllerCompBase::Worker::OnNewJobAdded()
{
	QMutexLocker locker(&m_jobsMutex);
	if (!m_jobContainerPtr.IsValid()){
		if (!m_jobs.isEmpty()){
			m_jobTimeRange = m_jobs.back();

			if (m_parent.m_eventFilterCompPtr.IsValid() && m_parent.m_messageFilterParamsCompPtr.IsValid()){
				m_parent.m_messageFilterParamsCompPtr->SetFilterTimeRange(m_jobTimeRange);
				if (m_parent.m_eventProviderCompPtr.IsValid()){
					m_jobContainerPtr = m_parent.m_eventProviderCompPtr->GetEvents(
						m_parent.m_eventFilterCompPtr.GetPtr(),
						m_parent.m_messageFilterParamsCompPtr.GetPtr());

					imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(m_jobContainerPtr.GetPtr());
					if (modelPtr != nullptr){
						modelPtr->AttachObserver(this);
					}
					else{
						m_jobContainerPtr.SetPtr(nullptr);
					}
				}
			}

			m_jobs.clear();
		}
	}
}


void CRepresentationControllerCompBase::Worker::OnResultReady()
{
	m_parent.BuildRepresentation(*m_parent.m_representationCompPtr, m_jobContainerPtr, m_jobTimeRange);

	QMutexLocker locker(&m_jobsMutex);
	m_jobContainerPtr.SetPtr(nullptr);
	Q_EMIT EmitNewJobAdded();
}


// reimplemented (imod::CSingleModelObserverBase)

void CRepresentationControllerCompBase::Worker::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	Q_EMIT EmitResultReady();
}


} // namespace imtloggui


