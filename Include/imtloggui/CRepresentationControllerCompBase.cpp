#include <imtloggui/CRepresentationControllerCompBase.h>


// Qt includes
#include <QtCore/QDebug>


namespace imtloggui
{


// public methods

CRepresentationControllerCompBase::CRepresentationControllerCompBase()
	:m_eventProviderObserver(*this)
{
	connect(&m_workerObject, &CWorker::TaskFinished, this, &CRepresentationControllerCompBase::OnTaskFinished);
}


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

void CRepresentationControllerCompBase::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (m_representationCompPtr.IsValid() && m_eventProviderCompPtr.IsValid()){
		//if (m_workerObjectPtr != nullptr){
		//	m_workerObjectPtr->AddJob(GetObservedObject()->GetTimeRange());
		//}

		CreateRepresentationTask* taskPtr = new CreateRepresentationTask();
		taskPtr->timeRange = GetObservedObject()->GetTimeRange();
		taskPtr->eventProviderPtr = m_eventProviderCompPtr.GetPtr();
		taskPtr->eventFilterPtr = m_eventFilterCompPtr.GetPtr();
		taskPtr->messageFilterParamsPtr = m_messageFilterParamsCompPtr.GetPtr();
		taskPtr->controllerPtr = this;
		taskPtr->representationPtr = m_representationCompPtr.GetPtr();
		taskPtr->updateCount = 0;

		m_workerObject.CancelAll();
		m_workerObject.AddJob(istd::TSmartPtr<istd::IChangeable>(taskPtr));
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
	m_workerObject.CancelAll();
	m_workerObject.Wait();

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
			CreateRepresentationTask* taskPtr = new CreateRepresentationTask();
			taskPtr->timeRange = m_parent.GetObservedObject()->GetTimeRange();
			taskPtr->eventProviderPtr = m_parent.m_eventProviderCompPtr.GetPtr();
			taskPtr->eventFilterPtr = m_parent.m_eventFilterCompPtr.GetPtr();
			taskPtr->messageFilterParamsPtr = m_parent.m_messageFilterParamsCompPtr.GetPtr();
			taskPtr->controllerPtr = &m_parent;
			taskPtr->representationPtr = m_parent.m_representationCompPtr.GetPtr();
			taskPtr->updateCount = 0;

			m_parent.m_workerObject.CancelAll();
			m_parent.m_workerObject.AddJob(istd::TSmartPtr<istd::IChangeable>(taskPtr));
		}
	}
}


// protected methods of the embedded class Worker

bool CRepresentationControllerCompBase::Worker::DoJob(TaskPtr& taskPtr)
{
	CreateRepresentationTask* createRepresentationTaskPtr =
				dynamic_cast<CreateRepresentationTask*>(taskPtr.GetPtr());

	imtlog::IEventProvider::EventContainerPtr containerPtr;

	if (createRepresentationTaskPtr->timeRange.IsClosed() &&
		createRepresentationTaskPtr->eventProviderPtr != nullptr &&
		createRepresentationTaskPtr->eventFilterPtr != nullptr &&
		createRepresentationTaskPtr->messageFilterParamsPtr != nullptr)
	{
		createRepresentationTaskPtr->messageFilterParamsPtr->SetFilterTimeRange(
					createRepresentationTaskPtr->timeRange);
		containerPtr = createRepresentationTaskPtr->eventProviderPtr->GetEvents(
					createRepresentationTaskPtr->eventFilterPtr,
					createRepresentationTaskPtr->messageFilterParamsPtr);
	}

	if (containerPtr.IsValid()){
		imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(containerPtr.GetPtr());
		modelPtr->AttachObserver(createRepresentationTaskPtr);

		while (createRepresentationTaskPtr->updateCount == 0){
			sleep(1);
		}

		createRepresentationTaskPtr->controllerPtr->BuildRepresentation(
					*createRepresentationTaskPtr->representationPtr,
					containerPtr,
					createRepresentationTaskPtr->timeRange);

		return true;
	}

	return false;

	//if (!m_jobContainerPtr.IsValid()){
	//	if (!m_jobs.isEmpty()){
	//		m_jobTimeRange = m_jobs.back();

	//		if (m_parent.m_eventFilterCompPtr.IsValid() && m_parent.m_messageFilterParamsCompPtr.IsValid()){
	//			m_parent.m_messageFilterParamsCompPtr->SetFilterTimeRange(m_jobTimeRange);
	//			if (m_parent.m_eventProviderCompPtr.IsValid()){
	//				m_jobContainerPtr = m_parent.m_eventProviderCompPtr->GetEvents(
	//					m_parent.m_eventFilterCompPtr.GetPtr(),
	//					m_parent.m_messageFilterParamsCompPtr.GetPtr());

	//				imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(m_jobContainerPtr.GetPtr());
	//				if (modelPtr != nullptr){
	//					modelPtr->AttachObserver(this);
	//				}
	//				else{
	//					m_jobContainerPtr.SetPtr(nullptr);
	//				}
	//			}
	//		}

	//		m_jobs.clear();
	//	}
	//}
}

// reimplemented (imtloggui::CRepresentationControllerWorkerBase)

//void CRepresentationControllerCompBase::Worker::OnNewJobAdded()
//{
//	QMutexLocker locker(&m_jobsMutex);
//	if (!m_jobContainerPtr.IsValid()){
//		if (!m_jobs.isEmpty()){
//			m_jobTimeRange = m_jobs.back();
//
//			if (m_parent.m_eventFilterCompPtr.IsValid() && m_parent.m_messageFilterParamsCompPtr.IsValid()){
//				m_parent.m_messageFilterParamsCompPtr->SetFilterTimeRange(m_jobTimeRange);
//				if (m_parent.m_eventProviderCompPtr.IsValid()){
//					m_jobContainerPtr = m_parent.m_eventProviderCompPtr->GetEvents(
//						m_parent.m_eventFilterCompPtr.GetPtr(),
//						m_parent.m_messageFilterParamsCompPtr.GetPtr());
//
//					imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(m_jobContainerPtr.GetPtr());
//					if (modelPtr != nullptr){
//						modelPtr->AttachObserver(this);
//					}
//					else{
//						m_jobContainerPtr.SetPtr(nullptr);
//					}
//				}
//			}
//
//			m_jobs.clear();
//		}
//	}
//}


//void CRepresentationControllerCompBase::Worker::OnResultReady()
//{
//	m_parent.BuildRepresentation(*m_parent.m_representationCompPtr, m_jobContainerPtr, m_jobTimeRange);
//
//	QMutexLocker locker(&m_jobsMutex);
//	m_jobContainerPtr.SetPtr(nullptr);
//	Q_EMIT EmitNewJobAdded();
//}
//
//
//// reimplemented (imod::CSingleModelObserverBase)
//
//void CRepresentationControllerCompBase::Worker::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
//{
//	Q_EMIT EmitResultReady();
//}


} // namespace imtloggui


