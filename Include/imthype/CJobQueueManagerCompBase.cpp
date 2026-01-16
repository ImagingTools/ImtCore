#include <imthype/CJobQueueManagerCompBase.h>


// Qt includes
#include <QtCore/QUuid>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CSystem.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imthype/CJobTicket.h>

namespace imthype
{


// public methods

CJobQueueManagerCompBase::CJobQueueManagerCompBase()
	:m_mutex(QReadWriteLock::Recursive)
{
}


// protected methods

IJobTicketSharedPtr CJobQueueManagerCompBase::GetJobTicket(const QByteArray& jobId) const
{
	if (!m_jobTicketsCollectionCompPtr.IsValid()){
		return IJobTicketSharedPtr();
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_jobTicketsCollectionCompPtr->GetObjectData(jobId, dataPtr)){
		IJobTicket* ticketPtr = dynamic_cast<IJobTicket*>(dataPtr.GetPtr());
		if (ticketPtr){
			iprm::IParamsSetSharedPtr retVal;
			// Set params factory for proper deserialization
			ticketPtr->SetParamsFactory([this](const QByteArray& ctx, const QByteArray& type) -> iprm::IParamsSetSharedPtr {
				iprm::IParamsSetUniquePtr uniquePtr = CreateJobParameters(ctx, type, nullptr);
				iprm::IParamsSetSharedPtr retVal;
				retVal.MoveCastedPtr(uniquePtr);
				
				return retVal;
			});

			return IJobTicketSharedPtr(ticketPtr);
		}
	}

	return IJobTicketSharedPtr();
}


// reimplemented (IJobQueueManager)

QByteArray CJobQueueManagerCompBase::GetTaskTypeId(const QByteArray& jobId) const
{
	QReadLocker lock(&m_mutex);

	IJobTicketSharedPtr ticketPtr = GetJobTicket(jobId);
	if (ticketPtr){
		return ticketPtr->GetTypeId();
	}

	return QByteArray();
}


QByteArray CJobQueueManagerCompBase::InsertNewJobIntoQueue(
			const QByteArray& contextId,
			const QByteArray& typeId,
			const imtbase::IReferenceCollection& input,
			const iprm::IParamsSet* jobProcessingParamsPtr,
			const IJobSchedulerParams* /*schedulerParamsPtr*/,
			ilog::IMessageConsumer* /*logPtr*/)
{
	if (!m_jobTicketsCollectionCompPtr.IsValid()){
		SendErrorMessage(0, "Job tickets collection is not set");
		return QByteArray();
	}

	QByteArray jobId = QUuid::createUuid().toByteArray();

	// Create a new job ticket
	CJobTicket jobTicket;
	jobTicket.SetUuid(jobId);
	jobTicket.SetContextId(contextId);
	jobTicket.SetTypeId(typeId);
	jobTicket.SetProcessingStatus(PS_WAITING_FOR_ACCEPTING);
	jobTicket.SetInput(input);

	// Set params factory for proper deserialization
	jobTicket.SetParamsFactory([this](const QByteArray& ctx, const QByteArray& type) -> iprm::IParamsSetSharedPtr {
		iprm::IParamsSetUniquePtr uniquePtr = CreateJobParameters(ctx, type, nullptr);
		iprm::IParamsSetSharedPtr retVal;
		retVal.MoveCastedPtr(uniquePtr);

		return retVal;
	});

	if (jobProcessingParamsPtr != nullptr){
		iprm::IParamsSetUniquePtr paramsPtr = CreateJobParameters(contextId, typeId, jobProcessingParamsPtr);
		if (!paramsPtr){
			SendErrorMessage(0, "Job parameters could not be created");

			return QByteArray();
		}
		iprm::IParamsSetSharedPtr paramsSetSharedPtr;
		paramsSetSharedPtr.MoveCastedPtr(paramsPtr);

		jobTicket.SetParams(iprm::IParamsSetSharedPtr(paramsSetSharedPtr));
	}

	QWriteLocker lock(&m_mutex);

	imtbase::IObjectCollection::Id insertedId = m_jobTicketsCollectionCompPtr->InsertNewObject(
				typeId,
				QString(),
				QString(),
				&jobTicket,
				jobId);

	lock.unlock();

	if (insertedId.isEmpty()){
		SendErrorMessage(0, "Failed to insert job ticket into collection");
		return QByteArray();
	}

	return insertedId;
}


bool CJobQueueManagerCompBase::CancelJob(const QByteArray & jobId)
{
	if (!m_jobTicketsCollectionCompPtr.IsValid()){
		return false;
	}

	QWriteLocker lock(&m_mutex);

	IJobTicketSharedPtr ticketPtr = GetJobTicket(jobId);
	if (!ticketPtr){
		return false;
	}

	ProcessingStatus currentStatus = ticketPtr->GetProcessingStatus();
	ProcessingStatus newStatus = currentStatus;

	if (currentStatus < PS_RUNNING){
		newStatus = PS_CANCELED;
	}
	else if (currentStatus == PS_RUNNING){
		newStatus = PS_CANCELING;
	}
	else{
		return false;
	}

	istd::IChangeable::ChangeSet changeSet = istd::IChangeable::GetAnyChange();
	JobStatusInfo info;
	info.elementId = jobId;
	info.status = newStatus;

	changeSet.SetChangeInfo(IJobQueueManager::CN_JOB_STATUS_CHANGED, QVariant::fromValue(info));

	istd::CChangeNotifier changeNotifier(this, &changeSet);

	ticketPtr->SetProcessingStatus(newStatus);

	m_jobTicketsCollectionCompPtr->SetObjectData(jobId, *ticketPtr);

	return true;
}


bool CJobQueueManagerCompBase::ResumeJob(const QByteArray & jobId)
{
	if (!m_jobTicketsCollectionCompPtr.IsValid()){
		return false;
	}

	QWriteLocker lock(&m_mutex);

	IJobTicketSharedPtr ticketPtr = GetJobTicket(jobId);
	if (!ticketPtr){
		return false;
	}

	ProcessingStatus currentStatus = ticketPtr->GetProcessingStatus();

	if ((currentStatus == PS_CANCELED) || (currentStatus == PS_FINISHED)){
		istd::IChangeable::ChangeSet changeSet = istd::IChangeable::GetAnyChange();
		JobStatusInfo info;
		info.elementId = jobId;
		info.status = PS_WAITING_FOR_PROCESSING;

		changeSet.SetChangeInfo(IJobQueueManager::CN_JOB_STATUS_CHANGED, QVariant::fromValue(info));

		istd::CChangeNotifier changeNotifier(this, &changeSet);

		ticketPtr->SetProcessingStatus(PS_WAITING_FOR_PROCESSING);

		m_jobTicketsCollectionCompPtr->SetObjectData(jobId, *ticketPtr);

		return true;
	}

	return false;
}


bool CJobQueueManagerCompBase::RemoveJob(const QByteArray& jobId)
{
	if (!m_jobTicketsCollectionCompPtr.IsValid()){
		return false;
	}

	QWriteLocker lock(&m_mutex);

	IJobTicketSharedPtr ticketPtr = GetJobTicket(jobId);
	if (!ticketPtr){
		return false;
	}

	// Job is running - cancel it first:
	ProcessingStatus status = ticketPtr->GetProcessingStatus();
	if (status > PS_NONE && status < PS_CANCELED){
		lock.unlock();
		if (!CancelJob(jobId)){
			return false;
		}
		lock.relock();
		
		// Re-check that the ticket still exists after relock
		ticketPtr = GetJobTicket(jobId);
		if (!ticketPtr){
			return false;
		}
	}

	istd::IChangeable::ChangeSet changeSet = istd::IChangeable::GetAnyChange();
	imtbase::ICollectionInfo::ElementsRemoveInfo info;
	info.elementIds << jobId;

	changeSet.SetChangeInfo(imtbase::ICollectionInfo::CN_ELEMENTS_REMOVED, QVariant::fromValue(info));

	istd::CChangeNotifier changeNotifier(this, &changeSet);

	bool result = m_jobTicketsCollectionCompPtr->RemoveElements(imtbase::IObjectCollection::Ids() << jobId);

	return result;
}


bool CJobQueueManagerCompBase::GetJobConfiguration(
			const QByteArray& jobId,
			ParamsPtr& processingParamsPtr,
			imtbase::IReferenceCollection& input) const
{
	QReadLocker lock(&m_mutex);

	IJobTicketSharedPtr ticketPtr = GetJobTicket(jobId);
	if (!ticketPtr){
		return false;
	}

	const imtbase::IReferenceCollection* inputPtr = ticketPtr->GetInput();
	if (!inputPtr || !input.CopyFrom(*inputPtr)){
		return false;
	}

	iprm::IParamsSetSharedPtr paramsPtr = ticketPtr->GetParams();
	if (paramsPtr.IsValid()){
		processingParamsPtr.FromUnique(CreateJobParameters(ticketPtr->GetContextId(), ticketPtr->GetTypeId(), nullptr));
		if (!processingParamsPtr.IsValid()){
			return false;
		}
		if (!processingParamsPtr->CopyFrom(*paramsPtr)){
			return false;
		}
	}

	return true;
}


CJobQueueManagerCompBase::ProcessingStatus CJobQueueManagerCompBase::GetProcessingStatus(const QByteArray& jobId) const
{
	QReadLocker lock(&m_mutex);

	IJobTicketSharedPtr ticketPtr = GetJobTicket(jobId);
	if (ticketPtr){
		return ticketPtr->GetProcessingStatus();
	}

	return PS_NONE;
}


bool CJobQueueManagerCompBase::SetProcessingStatus(const QByteArray & jobId, ProcessingStatus status)
{
	if (!m_jobTicketsCollectionCompPtr.IsValid()){
		return false;
	}

	QWriteLocker lock(&m_mutex);

	IJobTicketSharedPtr ticketPtr = GetJobTicket(jobId);
	if (!ticketPtr){
		return false;
	}

	istd::IChangeable::ChangeSet changeSet = istd::IChangeable::GetAnyChange();
	JobStatusInfo info;
	info.elementId = jobId;
	info.status = status;

	changeSet.SetChangeInfo(IJobQueueManager::CN_JOB_STATUS_CHANGED, QVariant::fromValue(info));

	istd::CChangeNotifier changeNotifier(this, &changeSet);

	ticketPtr->SetProcessingStatus(status);

	m_jobTicketsCollectionCompPtr->SetObjectData(jobId, *ticketPtr);

	return true;
}


double CJobQueueManagerCompBase::GetProgress(const QByteArray & jobId) const
{
	QReadLocker lock(&m_mutex);

	IJobTicketSharedPtr ticketPtr = GetJobTicket(jobId);
	if (ticketPtr){
		return ticketPtr->GetProgress();
	}

	return 0.0;
}


bool CJobQueueManagerCompBase::SetProgress(const QByteArray& jobId, double progress)
{
	if (!m_jobTicketsCollectionCompPtr.IsValid()){
		return false;
	}

	QWriteLocker lock(&m_mutex);

	IJobTicketSharedPtr ticketPtr = GetJobTicket(jobId);
	if (!ticketPtr){
		return false;
	}

	if (!qFuzzyCompare(ticketPtr->GetProgress(), progress)){
		istd::IChangeable::ChangeSet changeSet = istd::IChangeable::GetAnyChange();
		JobProgressInfo info;
		info.elementId = jobId;
		info.progress = progress;

		changeSet.SetChangeInfo(IJobQueueManager::CN_JOB_PROGRESS_CHANGED, QVariant::fromValue(info));

		istd::CChangeNotifier changeNotifier(this, &changeSet);

		ticketPtr->SetProgress(progress);

		m_jobTicketsCollectionCompPtr->SetObjectData(jobId, *ticketPtr);

		return true;
	}

	return false;
}


bool CJobQueueManagerCompBase::GetJobResult(const QByteArray& jobId, IJobOutput& result) const
{
	QReadLocker lock(&m_mutex);

	IJobTicketSharedPtr ticketPtr = GetJobTicket(jobId);
	if (ticketPtr){
		const IJobOutput* resultsPtr = ticketPtr->GetResults();
		if (resultsPtr){
			return result.CopyFrom(*resultsPtr);
		}
	}

	return false;
}


bool CJobQueueManagerCompBase::SetJobResult(const QByteArray& jobId, const IJobOutput& result)
{
	if (!m_jobTicketsCollectionCompPtr.IsValid()){
		return false;
	}

	QWriteLocker lock(&m_mutex);

	IJobTicketSharedPtr ticketPtr = GetJobTicket(jobId);
	if (!ticketPtr){
		return false;
	}

	const IJobOutput* currentResultsPtr = ticketPtr->GetResults();
	if (!currentResultsPtr || !result.IsEqual(*currentResultsPtr)){
		istd::IChangeable::ChangeSet changeSet = istd::IChangeable::GetAnyChange();
		JobResultInfo info;
		info.elementId = jobId;

		changeSet.SetChangeInfo(IJobQueueManager::CN_JOB_RESULT_CHANGED, QVariant::fromValue(info));

		istd::CChangeNotifier changeNotifier(this, &changeSet);

		ticketPtr->SetResults(result);

		m_jobTicketsCollectionCompPtr->SetObjectData(jobId, *ticketPtr);

		return true;
	}

	return false;
}


// reimplemented (imtbase::ICollectionInfo)

int CJobQueueManagerCompBase::GetElementsCount(const iprm::IParamsSet* selectionParamPtr, ilog::IMessageConsumer* logPtr) const
{
	QReadLocker lock(&m_mutex);

	if (!m_jobTicketsCollectionCompPtr.IsValid()){
		return 0;
	}

	return m_jobTicketsCollectionCompPtr->GetElementsCount(selectionParamPtr, logPtr);
}



imtbase::ICollectionInfo::Ids CJobQueueManagerCompBase::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			ilog::IMessageConsumer* logPtr) const
{
	QReadLocker lock(&m_mutex);

	if (!m_jobTicketsCollectionCompPtr.IsValid()){
		return Ids();
	}

	return m_jobTicketsCollectionCompPtr->GetElementIds(offset, count, selectionParamsPtr, logPtr);
}

bool CJobQueueManagerCompBase::GetSubsetInfo(
			imtbase::ICollectionInfo& subsetInfo,
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			ilog::IMessageConsumer* logPtr) const
{
	if (!m_jobTicketsCollectionCompPtr.IsValid()){
		return false;
	}

	return m_jobTicketsCollectionCompPtr->GetSubsetInfo(subsetInfo, offset, count, selectionParamsPtr, logPtr);
}


QVariant CJobQueueManagerCompBase::GetElementInfo(const QByteArray& elementId, int infoType, ilog::IMessageConsumer* logPtr) const
{
	QReadLocker lock(&m_mutex);

	if (!m_jobTicketsCollectionCompPtr.IsValid()){
		return QVariant();
	}

	IJobTicketSharedPtr ticketPtr = GetJobTicket(elementId);
	if (ticketPtr){
		switch (infoType){
		case EIT_NAME:
			return ticketPtr->GetJobName();
		default:
			break;
		}
	}

	return m_jobTicketsCollectionCompPtr->GetElementInfo(elementId, infoType, logPtr);
}


idoc::MetaInfoPtr CJobQueueManagerCompBase::GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* logPtr) const
{
	if (!m_jobTicketsCollectionCompPtr.IsValid()){
		return idoc::MetaInfoPtr();
	}

	return m_jobTicketsCollectionCompPtr->GetElementMetaInfo(elementId, logPtr);
}


bool CJobQueueManagerCompBase::SetElementName(const Id& elementId, const QString& name, ilog::IMessageConsumer* logPtr)
{
	if (!m_jobTicketsCollectionCompPtr.IsValid()){
		return false;
	}

	return m_jobTicketsCollectionCompPtr->SetElementName(elementId, name, logPtr);
}


bool CJobQueueManagerCompBase::SetElementDescription(const Id& elementId, const QString& description, ilog::IMessageConsumer* logPtr)
{
	if (!m_jobTicketsCollectionCompPtr.IsValid()){
		return false;
	}

	return m_jobTicketsCollectionCompPtr->SetElementDescription(elementId, description, logPtr);
}


bool CJobQueueManagerCompBase::SetElementEnabled(const Id& elementId, bool isEnabled, ilog::IMessageConsumer* logPtr)
{
	if (!m_jobTicketsCollectionCompPtr.IsValid()){
		return false;
	}

	return m_jobTicketsCollectionCompPtr->SetElementEnabled(elementId, isEnabled, logPtr);
}


// reimplemented (icomp::CComponentBase)

void CJobQueueManagerCompBase::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_jobTicketsCollectionCompPtr.EnsureInitialized();
	if (!m_jobTicketsCollectionCompPtr.IsValid()){
		SendCriticalMessage(0, "Wrong component configuration. Job tickets collection component was not set");
	}
}


void CJobQueueManagerCompBase::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


} // namespace imthype


