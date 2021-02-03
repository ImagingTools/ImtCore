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
			m_worker.requestInterruption();

			{
				QMutexLocker locker(&m_workingDataMutex);

				m_workingTimeRange = timeRange;
			}

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

void CRepresentationControllerCompBase::OnRepresentationCreated()
{
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
				m_parent.m_worker.requestInterruption();

				{
					QMutexLocker locker(&m_parent.m_workingDataMutex);

					m_parent.m_workingTimeRange = timeRange;
				}

				m_parent.m_worker.Start();
			}
		}
	}
}


// protected methods of the embedded class Worker

CRepresentationControllerCompBase::Worker::Worker(CRepresentationControllerCompBase& parent)
	:m_parent(parent)
{
	m_workingRepresentationPtr.SetPtr(parent.m_representationFactPtr.CreateComponent());
}


void CRepresentationControllerCompBase::Worker::run()
{
	if (!m_workingRepresentationPtr.IsValid()){
		return;
	}

	istd::IChangeable* workingInstancePtr = m_parent.m_representationFactPtr.ExtractInterface(m_workingRepresentationPtr.GetPtr());
	if (workingInstancePtr == nullptr){
		return;
	}

	bool isFinished = false;

	while (!isFinished){
		QMutexLocker locker(&m_parent.m_workingDataMutex);

		imtlog::CTimeRange jobTimeRange = m_parent.m_workingTimeRange;

		m_parent.m_workingTimeRange.Clear();

		locker.unlock();

		if (jobTimeRange.IsClosed()){
			m_parent.m_messageFilterParamsCompPtr->SetFilterTimeRange(jobTimeRange);

			imtlog::IEventProvider::EventContainerPtr containerPtr = m_parent.m_eventProviderCompPtr->GetEvents(
						m_parent.m_eventFilterCompPtr.GetPtr(),
						m_parent.m_messageFilterParamsCompPtr.GetPtr());

			if (containerPtr.IsValid()){
				imtlog::CEventHistoryResultContainer* eventHistoryResultContainerPtr = dynamic_cast<imtlog::CEventHistoryResultContainer*>(containerPtr.GetPtr());
				Q_ASSERT(eventHistoryResultContainerPtr != nullptr);

				while (!eventHistoryResultContainerPtr->IsClosed() && !isFinished){
					msleep(1);

					if (isInterruptionRequested()){
						isFinished = true;
					}
				}

				if (!isFinished){
					m_parent.BuildRepresentation(
								*workingInstancePtr,
								containerPtr,
								jobTimeRange);

					Q_EMIT m_parent.EmitRepresentationCreated();
				}
			}
		}
		else{
			isFinished = true;
		}
	}
}


} // namespace imtloggui


