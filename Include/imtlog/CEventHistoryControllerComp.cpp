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
	qRegisterMetaType<MessagePtr>("MessagePtr");
	qRegisterMetaType<EventContainerPtr>("EventContainerPtr");
}


// reimplemented (imtlog::ITimeRangeProvider)

CTimeRange CEventHistoryControllerComp::GetTimeRange() const
{
	return m_archiveTimeRange;
}


// reimplemented (imtlog::IMessageFilter)

IEventProvider::EventContainerPtr CEventHistoryControllerComp::GetEvents(IEventProvider::EventFilterPtr filterPtr) const
{
	return IEventProvider::EventContainerPtr();
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
		if (m_groupsMap.contains(id)){
			m_groupsMap[id]->AddMessage(messagePtr);
		}
		else{
			m_generalGroup->AddMessage(messagePtr);
		}
	}
}


// reimplemented (ibase::TRuntimeStatusHanderCompWrap)

void CEventHistoryControllerComp::OnSystemShutdown()
{
	m_controllerState = CS_SHUTDOWN;

	for (EventHistoryGroupControllerPtr group : m_groups){
		group->OnSystemShutdown();
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

	cParams.containerDuration = 15;
	cParams.writeDelay = 5;
	cParams.removeDelay = 10;

	CTimeRange gTimeRange;

	pParams.repositoryDir = m_logFolderCompPtr->GetPath();
	pParams.groupDir = "UserActions";
	pParams.containerExtension = "xml";
	pParams.archiveExtension = "arc";
 	pParams.versionInfoPtr = m_versionInfoCompPtr.GetPtr();
	pParams.compressorPtr = m_compressorCompPtr.GetPtr();

	groupPtr.SetPtr(new CEventHistoryGroupController(cParams, pParams));
	groupPtr->SetLogPtr(GetLogPtr());
	gTimeRange = groupPtr->GetTimeRange();
	m_groups.append(groupPtr);
	m_groupsMap[100000000] = groupPtr;

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
	m_groups.append(groupPtr);
	m_groupsMap[19780000] = groupPtr;
	m_groupsMap[100000001] = groupPtr;

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
	m_groups.append(groupPtr);
	m_generalGroup = groupPtr;

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


} // namespace imtlog


