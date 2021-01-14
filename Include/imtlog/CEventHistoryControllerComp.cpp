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


// reimplemented (imtlog::IMessageFilter)

IEventProvider::EventContainerPtr CEventHistoryControllerComp::GetEvents(IEventProvider::EventFilterPtr filterPtr) const
{
	EventHistoryGroupControllerPtr groupPtr;

	QList<int> ids = filterPtr->GetGroupMessageIds();
	for (int i = 0; i < ids.count(); i++){
		groupPtr = GetGroupForMessageId(ids[i]);
		if (groupPtr.IsValid()){
			break;
		}
	}

	if (!groupPtr.IsValid()){
		groupPtr = m_groups[0].groupPtr;
	}

	QMutexLocker locker(&m_requestMutex);

	QByteArray uuid = groupPtr->RequestEvents(filterPtr);

	RequestMapItem item;
	item.resultEventsPtr.SetPtr(new ilog::CMessageContainer());
	item.groupPtr = groupPtr;

	m_requests[uuid] = item;

	return item.resultEventsPtr;
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

	EventHistoryGroupControllerParams cParams;
	EventHistoryGroupPersistenceParams pParams;
	EventHistoryGroupControllerPtr groupPtr;
	CTimeRange gTimeRange;
	GroupListItem item;

	cParams.containerDuration = 15;
	cParams.writeDelay = 5;
	cParams.removeDelay = 10;


	pParams.repositoryDir = m_logFolderCompPtr->GetPath();
	pParams.groupDir = "UserActions";
	pParams.containerExtension = "xml";
	pParams.archiveExtension = "arc";
 	pParams.versionInfoPtr = m_versionInfoCompPtr.GetPtr();
	pParams.compressorPtr = m_compressorCompPtr.GetPtr();

	groupPtr.SetPtr(new CEventHistoryGroupController(cParams, pParams));
	groupPtr->SetLogPtr(GetLogPtr());
	gTimeRange = groupPtr->GetTimeRange();

	item.messageIds.clear();
	item.messageIds.append(100000000);
	item.groupPtr = groupPtr;
	m_groups.append(item);

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
	
	pParams.groupDir = "Production";
	groupPtr.SetPtr(new CEventHistoryGroupController(cParams, pParams));
	groupPtr->SetLogPtr(GetLogPtr());
	gTimeRange = groupPtr->GetTimeRange();

	item.messageIds.clear();
	item.messageIds.append(1000000001);
	item.messageIds.append(19780000);
	item.groupPtr = groupPtr;
	m_groups.append(item);

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

	pParams.groupDir = "General";
	groupPtr.SetPtr(new CEventHistoryGroupController(cParams, pParams));
	groupPtr->SetLogPtr(GetLogPtr());
	gTimeRange = groupPtr->GetTimeRange();

	item.messageIds.clear();
	item.groupPtr = groupPtr;
	m_groups.prepend(item);

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

	m_controllerState = CS_OK;
}


// public slots

void CEventHistoryControllerComp::OnRequestFinished(QByteArray requestId)
{
	QMutexLocker locker(&m_requestMutex);

	if (m_requests.contains(requestId)){
		m_requests[requestId].groupPtr->PopResult(requestId, *m_requests[requestId].resultEventsPtr);
		m_requests.remove(requestId);
	}
}


// private methods

CEventHistoryControllerComp::EventHistoryGroupControllerPtr CEventHistoryControllerComp::GetGroupForMessageId(int messageId) const
{
	for (const GroupListItem& item : m_groups){
		if (item.messageIds.contains(messageId)){
			return item.groupPtr;
		}
	}
	
	return EventHistoryGroupControllerPtr();
}


} // namespace imtlog


