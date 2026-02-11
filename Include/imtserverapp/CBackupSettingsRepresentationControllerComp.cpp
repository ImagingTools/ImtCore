// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CBackupSettingsRepresentationControllerComp.h>


// Qt includes
#include <QtCore/QDateTime>

// ImtCore includes
#include <imtapp/IBackupSettings.h>


namespace imtserverapp
{


// protected methods

// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CSchedulerParam::V1_0>)

QByteArray CBackupSettingsRepresentationControllerComp::GetTypeId() const
{
	return sdl::imtbase::ImtBaseTypes::CParamTypeIds::V1_0::ParamTypeIdsFields::BackupSettings.toUtf8();
}


bool CBackupSettingsRepresentationControllerComp::IsModelSupported(const istd::IChangeable &dataModel) const
{
	const imtapp::IBackupSettings* backupSettingsPtr = dynamic_cast<const imtapp::IBackupSettings*>(&dataModel);

	return backupSettingsPtr != nullptr;
}


bool CBackupSettingsRepresentationControllerComp::GetSdlRepresentationFromDataModel(
			sdl::imtbase::ImtBaseTypes::CBackupSettings::V1_0& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const imtapp::IBackupSettings* backupSettingsPtr = dynamic_cast<const imtapp::IBackupSettings*>(&dataModel);
	Q_ASSERT(backupSettingsPtr != nullptr);
	if (backupSettingsPtr == nullptr){
		return false;
	}

	sdl::imtbase::ImtBaseTypes::CSchedulerParam::V1_0 schedulerParam;
	schedulerParam.interval = backupSettingsPtr->GetInterval();

	QDateTime startTime = backupSettingsPtr->GetStartTime();

	QString format = m_dateFormatAttrPtr.IsValid() ? *m_dateFormatAttrPtr : "dd-MM-yyyy HH:mm";
	schedulerParam.startTime = startTime.toString(format);

	sdlRepresentation.schedulerParam = schedulerParam;

	QString backupFolderPath = backupSettingsPtr->GetPath();
	sdlRepresentation.folderPath = backupFolderPath;

	return true;
}


bool CBackupSettingsRepresentationControllerComp::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::imtbase::ImtBaseTypes::CBackupSettings::V1_0& sdlRepresentation) const
{
	imtapp::IBackupSettings* backupSettingsPtr = dynamic_cast<imtapp::IBackupSettings*>(&dataModel);
	Q_ASSERT(backupSettingsPtr != nullptr);
	if (backupSettingsPtr == nullptr){
		return false;
	}

	if (!sdlRepresentation.schedulerParam || !sdlRepresentation.folderPath){
		return false;
	}

	sdl::imtbase::ImtBaseTypes::CSchedulerParam::V1_0 schedulerParam = *sdlRepresentation.schedulerParam;

	int interval = *schedulerParam.interval;
	backupSettingsPtr->SetInterval(interval);

	QString startTime = *schedulerParam.startTime;

	QString format = m_dateFormatAttrPtr.IsValid() ? *m_dateFormatAttrPtr : "dd-MM-yyyy HH:mm";
	QDateTime startDateTime = QDateTime::fromString(startTime, format);

	backupSettingsPtr->SetStartTime(startDateTime);

	QString folderPath = *sdlRepresentation.folderPath;
	backupSettingsPtr->SetPath(folderPath);

	return true;
}


} // namespace imtserverapp


