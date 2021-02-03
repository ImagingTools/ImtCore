#include <imtlog/CEventHistoryControllerComp.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QUuid>

// Acf includes
#include <ifile/CCompactXmlFileWriteArchive.h>
#include <ifile/CCompactXmlFileReadArchive.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <istd/CSystem.h>
#include <istd/CChangeGroup.h>


namespace imtlog
{


// public methods

CEventHistoryControllerComp::CEventHistoryControllerComp()
	:m_controllerState(CS_INIT)
{
}


// reimplemented (imtlog::ITimeRangeProvider)

CTimeRange CEventHistoryControllerComp::GetTimeRange() const
{
	return m_archiveTimeRange;
}


// reimplemented (imtlog::IEventProvider)

IEventProvider::EventContainerPtr CEventHistoryControllerComp::GetEvents(
			const IEventFilter* filterPtr,
			const IMessageFilterParams* filterParamsPtr) const
{
	if (filterPtr == nullptr || filterParamsPtr == nullptr){
		return IEventProvider::EventContainerPtr();
	}

	CEventHistoryResultContainer* containerPtr = new CEventHistoryResultContainer();

	{
		istd::CChangeGroup notifier(containerPtr);

		ilog::IMessageContainer::Messages messages = m_log.GetMessages();
		for (int i = messages.count() - 1; i >= 0; i--){
			if (filterPtr->IsMessageAccepted(*messages[i], filterParamsPtr)){
				containerPtr->AddMessage(messages[i]);
			}
		}

		containerPtr->Close();
	}

	return IEventProvider::EventContainerPtr(containerPtr);

	QSet<int> ids;

	if (filterParamsPtr->GetFilterMode() == imtlog::IMessageFilterParams::FM_INCLUDE){
		ids = m_controllers.keys().toSet() & filterParamsPtr->GetMessageFilterIds();
	}
	else{
		ids = m_controllers.keys().toSet() - filterParamsPtr->GetMessageFilterIds();
	}

	//istd::TOptDelPtr<CEventHistoryResultContainer> containerPtr(new CEventHistoryResultContainer());
	if (ids.isEmpty()){
		containerPtr->Close();
		//return IEventProvider::EventContainerPtr(containerPtr.PopPtr());
	}

	QMutexLocker locker(&m_requestMutex);
	ReadRequest request;

	//request.containerPtr.SetPtr(containerPtr.PopPtr());
	for (int id : ids){
		request.readIds.insert(m_controllers[id]->AddJob(filterPtr, filterParamsPtr));
	}
	m_requests.append(request);

	return request.containerPtr;
}


// reimplemented (ilog::IMessageConsumer)

bool CEventHistoryControllerComp::IsMessageSupported(
			int /*messageCategory*/,
			int /*messageId*/,
			const istd::IInformationProvider* /*messagePt*/) const
{
	return true;
}


void CEventHistoryControllerComp::AddMessage(const MessagePtr& messagePtr)
{
	BaseClass::AddMessage(messagePtr);

	if (m_controllerState == CS_OK){
		istd::CChangeNotifier notifier(this);
		m_log.AddMessage(messagePtr);
		m_archiveTimeRange.Ensure(messagePtr->GetInformationTimeStamp());

		return;

		//istd::CChangeNotifier notifier(this);
		m_archiveTimeRange.Ensure(messagePtr->GetInformationTimeStamp());

		int id = messagePtr->GetInformationId();

		if (m_controllers.contains(id)){
			m_controllers[id]->AddMessage(messagePtr);
		}
		else{
			InitializeMessagesController(id);
			m_controllers[id]->AddMessage(messagePtr);
		}
	}
}


// reimplemented (ibase::TRuntimeStatusHanderCompWrap)

void CEventHistoryControllerComp::OnSystemShutdown()
{
	m_controllerState = CS_SHUTDOWN;

	for (MessagesControllerPtr& controllerPtr : m_controllers){
		controllerPtr->OnSystemShutdown();
	}
}


// reimplemented (icomp::CComponentBase)

void CEventHistoryControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_systemStartTime = QDateTime::currentDateTime();

	if (!m_logFolderCompPtr.IsValid() || (m_logFolderCompPtr->GetPathType() != ifile::IFileNameParam::PT_DIRECTORY)){
		SendWarningMessage(0, tr("Log folder not specified. Event history disabled"));
		m_controllerState = CS_FAILED;
		return;
	}

	if (!istd::CSystem::EnsurePathExists(m_logFolderCompPtr->GetPath())){
		SendErrorMessage(0, tr("Unable access log folder. Event history disabled"));
		m_controllerState = CS_FAILED;
		return;
	}

	if (!m_versionInfoCompPtr.IsValid()){
		SendWarningMessage(0, tr("Unavailable version info provider. Event history disabled"));
		m_controllerState = CS_FAILED;
		return;
	}

	m_compressorCompPtr.EnsureInitialized();

	InitializeHistoryController();

	m_controllerState = CS_OK;
}


// private methods

void CEventHistoryControllerComp::InitializeHistoryController()
{
	QDir dir(m_logFolderCompPtr->GetPath());
	QFileInfoList list = dir.entryInfoList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);

	for (int i = 0; i < list.count(); i++){
		QString dirName = list[i].fileName();
		bool ok;
		int id = dirName.toInt(&ok);
		if (ok){
			InitializeMessagesController(id);

			CTimeRange timeRange = m_controllers[id]->GetTimeRange();
			m_archiveTimeRange.Ensure(m_archiveTimeRange.GetBeginTime());
			m_archiveTimeRange.Ensure(m_archiveTimeRange.GetEndTime());
		}
	}
}


void CEventHistoryControllerComp::InitializeMessagesController(int id)
{
	int maxMessageCount = -1;
	int savingPeriod = -1;
	int cachingTime = -1;

	if (m_messagesControllerParamsProviderCompPtr.IsValid()){
		maxMessageCount = m_messagesControllerParamsProviderCompPtr->GetMaxContainerMessageCount(id);
		savingPeriod = m_messagesControllerParamsProviderCompPtr->GetContainerSavingPeriod(id);
		cachingTime = m_messagesControllerParamsProviderCompPtr->GetContainerCachingTime(id);
	}

	if (maxMessageCount < 0 || savingPeriod < 0 || cachingTime < 0){
		maxMessageCount = *m_defaultMaxContainerMessageCountAttrPtr;
		savingPeriod = *m_defaultContainerSavingPeriodAttrPtr;
		cachingTime = *m_defaultContainerCachingTimeAttrPtr;
	}

	MessagesControllerPtr controllerPtr(new CMessagesController(
				id,
				maxMessageCount,
				savingPeriod,
				cachingTime,
				m_logFolderCompPtr->GetPath() + "/" + QString("%1").arg(id),
				"xml", "arc",
				m_versionInfoCompPtr.GetPtr(),
				m_compressorCompPtr.GetPtr(),
				GetLogPtr()));

	m_controllers[id] = controllerPtr;

	connect(controllerPtr.GetPtr(), &CMessagesController::ReadFinished, this, &CEventHistoryControllerComp::OnReadFinished, Qt::QueuedConnection);
}


// private slots

void CEventHistoryControllerComp::OnReadFinished(QByteArray requestId)
{
	bool isFound = false;

	QMutexLocker locker(&m_requestMutex);
	for (int i = 0; i < m_requests.count() && !isFound; i++){
		if (m_requests[i].readIds.contains(requestId)){
			for (MessagesControllerPtr& controllerPtr : m_controllers){
				if (controllerPtr->IsValidResultId(requestId)){
					m_requests[i].readIds.remove(requestId);
					ilog::CMessageContainer container;
					if (controllerPtr->PopResult(requestId, container)){
						ilog::IMessageContainer::Messages messages = container.GetMessages();
						for (ilog::IMessageConsumer::MessagePtr& messagePtr : messages){
							m_requests[i].events.insert(messagePtr->GetInformationTimeStamp().toMSecsSinceEpoch(), messagePtr);
						}

						if (m_requests[i].readIds.isEmpty()){
							istd::CChangeGroup changeGroup(m_requests[i].containerPtr.GetPtr());

							CEventHistoryResultContainer* eventHistoryResultContainerPtr =
								dynamic_cast<CEventHistoryResultContainer*>(m_requests[i].containerPtr.GetPtr());

							QMultiMap<qint64, ilog::IMessageConsumer::MessagePtr>::const_iterator it = m_requests[i].events.cbegin();
							while (it != m_requests[i].events.cend()){
								eventHistoryResultContainerPtr->AddMessage(*it);
								eventHistoryResultContainerPtr->Close();
							}

							m_requests.removeAt(i);
						}
					}

					isFound = true;
					break;
				}
			}
		}
	}
}


} // namespace imtlog


