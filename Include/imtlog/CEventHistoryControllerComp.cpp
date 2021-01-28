#include <imtlog/CEventHistoryControllerComp.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QUuid>

// ACF includes
#include <ifile/CCompactXmlFileWriteArchive.h>
#include <ifile/CCompactXmlFileReadArchive.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <istd/CSystem.h>


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

	MessageContainer* containerPtr = new MessageContainer();

	ilog::IMessageContainer::Messages messages = m_log.GetMessages();
	for (int i = messages.count() - 1; i >= 0; i--){
		if (filterPtr->IsMessageAccepted(*messages[i], filterParamsPtr)){
			containerPtr->AddMessage(messages[i]);
		}
	}

	return IEventProvider::EventContainerPtr(containerPtr);

	//QSet<int> messageIds = filterParamsPtr->GetMessageFilterIds();


	//EventHistoryGroupControllerPtr groupPtr;

	//QSet<int> messageIds;
	//
	//if (filterParamsPtr != nullptr){
	//	messageIds = filterParamsPtr->GetMessageFilterIds();
	//}

	//for (int i = 0; i < messageIds.count(); i++){
	//	groupPtr = GetGroupForMessageId(messageIds[i]);
	//	if (groupPtr.IsValid()){
	//		break;
	//	}
	//}

	//if (!groupPtr.IsValid()){
	//	groupPtr = m_groups[0].groupPtr;
	//}

	//QMutexLocker locker(&m_requestMutex);

	//QByteArray uuid = groupPtr->RequestEvents(filterPtr);

	//RequestMapItem groupItem;
	//groupItem.resultEventsPtr.SetPtr(new ilog::CMessageContainer());
	//groupItem.groupPtr = groupPtr;

	//m_requests[uuid] = groupItem;

	//return groupItem.resultEventsPtr;

	return EventContainerPtr();
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
		
		// !!!

		int id = messagePtr->GetInformationId();

		EventHistoryGroupControllerPtr groupPtr = GetGroupForMessageId(id);
		if (!groupPtr.IsValid()){
			groupPtr = m_groups[0].groupPtr;
		}

		groupPtr->AddMessage(messagePtr);
	}
}


// reimplemented (ibase::TRuntimeStatusHanderCompWrap)

void CEventHistoryControllerComp::OnSystemShutdown()
{
	m_controllerState = CS_SHUTDOWN;

	for (GroupListItem& item : m_groups){
		item.groupPtr->OnSystemShutdown();
	}

	if (m_generalGroupPtr.IsValid()){
		m_generalGroupPtr->OnSystemShutdown();
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

	if (m_groupProviderCompPtr.IsValid()){
		imtbase::ICollectionInfo::Ids groupIds = m_groupProviderCompPtr->GetElementIds();
		for (int i = 0; i < groupIds.count(); i++){
			QSet<int> messageIds;

			Q_ASSERT(m_groupMessageIdProviderCompPtr->GetGroupMessageIds(groupIds[i], messageIds));

			EventHistoryGroupControllerParams cParams;
			cParams.containerDuration = 10;
			cParams.writeDelay = 5;
			cParams.removeDelay = 10;

			EventHistoryGroupPersistenceParams pParams;
			pParams.repositoryDir = m_logFolderCompPtr->GetPath();
			pParams.groupDir = m_groupProviderCompPtr->GetElementInfo(groupIds[i], imtbase::ICollectionInfo::EIT_NAME).toString();
			pParams.containerExtension = "xml";
			pParams.archiveExtension = "arc";
			pParams.versionInfoPtr = m_versionInfoCompPtr.GetPtr();
			pParams.compressorPtr = m_compressorCompPtr.GetPtr();

			EventHistoryGroupControllerPtr groupPtr(new CEventHistoryGroupController(cParams, pParams));
			groupPtr->SetLogPtr(GetLogPtr());

			CTimeRange gTimeRange = groupPtr->GetTimeRange();
			qDebug() << gTimeRange.GetBeginTime();
			qDebug() << gTimeRange.GetEndTime();

			if (!m_archiveTimeRange.IsClosed()){
				m_archiveTimeRange = gTimeRange;
			}
			else{
				if (gTimeRange.GetBeginTime() < m_archiveTimeRange.GetBeginTime()){
					m_archiveTimeRange.SetBeginTime(gTimeRange.GetBeginTime());
				}

				if (gTimeRange.GetEndTime() > m_archiveTimeRange.GetEndTime()){
					m_archiveTimeRange.SetEndTime(gTimeRange.GetEndTime());
				}
			}

			if (messageIds.isEmpty()){
				Q_ASSERT(!m_generalGroupPtr.IsValid());
				m_generalGroupPtr = groupPtr;
			}
			else{
				GroupListItem groupItem;

				groupItem.messageIds = messageIds;
				groupItem.groupPtr = groupPtr;
				m_groups.append(groupItem);
			}
		}
	}

	m_controllerState = CS_OK;
}


// public slots

void CEventHistoryControllerComp::OnRequestFinished(QByteArray requestId)
{
	QMutexLocker locker(&m_requestMutex);

	if (m_requests.contains(requestId)){
		m_requests[requestId].groupPtr->PopResult(requestId, *(dynamic_cast<ilog::CMessageContainer*>(m_requests[requestId].resultEventsPtr.GetPtr())));
		m_requests.remove(requestId);
	}
}


// private methods

CEventHistoryControllerComp::EventHistoryGroupControllerPtr CEventHistoryControllerComp::GetGroupForMessageId(int messageId) const
{
	for (const GroupListItem& groupItem : m_groups){
		if (groupItem.messageIds.contains(messageId)){
			return groupItem.groupPtr;
		}
	}
	
	Q_ASSERT(m_generalGroupPtr.IsValid());
	return m_generalGroupPtr;
}


} // namespace imtlog


