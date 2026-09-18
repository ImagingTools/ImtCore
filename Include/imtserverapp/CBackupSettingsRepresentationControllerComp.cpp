// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CBackupSettingsRepresentationControllerComp.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Settings.h>


// Qt includes
#include <QtCore/QDateTime>

// ImtCore includes
#include <imtapp/IBackupSettings.h>


namespace imtserverapp
{


// protected methods

// reimplemented (imtserverapp::TJsonRepresentationControllerWrap<sdl::V1_0::imtbase::CSchedulerParam>)

QByteArray CBackupSettingsRepresentationControllerComp::GetTypeId() const
{
	return sdl::V1_0::imtbase::CParamTypeIds::ParamTypeIdsFields::BackupSettings.toUtf8();
}


bool CBackupSettingsRepresentationControllerComp::IsModelSupported(const istd::IChangeable &dataModel) const
{
	const imtapp::IBackupSettings* backupSettingsPtr = dynamic_cast<const imtapp::IBackupSettings*>(&dataModel);

	return backupSettingsPtr != nullptr;
}


bool CBackupSettingsRepresentationControllerComp::GetSdlRepresentationFromDataModel(
			sdl::V1_0::imtbase::CBackupSettings& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const imtapp::IBackupSettings* backupSettingsPtr = dynamic_cast<const imtapp::IBackupSettings*>(&dataModel);
	Q_ASSERT(backupSettingsPtr != nullptr);
	if (backupSettingsPtr == nullptr){
		return false;
	}

	sdl::V1_0::imtbase::CSchedulerParam schedulerParam;
	schedulerParam.interval = backupSettingsPtr->GetInterval();

	QDateTime startTime = backupSettingsPtr->GetStartTime();

	const QString format = m_dateFormatAttrPtr.IsValid() ? *m_dateFormatAttrPtr : QStringLiteral("dd-MM-yyyy HH:mm");
	schedulerParam.startTime = startTime.toString(format);

	sdlRepresentation.schedulerParam = schedulerParam;

	QString backupFolderPath = backupSettingsPtr->GetPath();
	sdlRepresentation.folderPath = backupFolderPath;

	return true;
}


bool CBackupSettingsRepresentationControllerComp::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::V1_0::imtbase::CBackupSettings& sdlRepresentation) const
{
	imtapp::IBackupSettings* backupSettingsPtr = dynamic_cast<imtapp::IBackupSettings*>(&dataModel);
	Q_ASSERT(backupSettingsPtr != nullptr);
	if (backupSettingsPtr == nullptr){
		return false;
	}

	if (!sdlRepresentation.schedulerParam || !sdlRepresentation.folderPath){
		return false;
	}

	sdl::V1_0::imtbase::CSchedulerParam schedulerParam = *sdlRepresentation.schedulerParam;

	int interval = *schedulerParam.interval;
	backupSettingsPtr->SetInterval(interval);

	QString startTime = *schedulerParam.startTime;

	const QString format = m_dateFormatAttrPtr.IsValid() ? *m_dateFormatAttrPtr : QStringLiteral("dd-MM-yyyy HH:mm");
	QDateTime startDateTime = QDateTime::fromString(startTime, format);

	backupSettingsPtr->SetStartTime(startDateTime);

	QString folderPath = *sdlRepresentation.folderPath;
	backupSettingsPtr->SetPath(folderPath);

	return true;
}


} // namespace imtserverapp


