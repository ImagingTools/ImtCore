// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CDatabaseAccessSettingsRepresentationControllerComp.h>


// ImtCore includes
#include <imtdb/IDatabaseLoginSettings.h>


namespace imtserverapp
{


// public methods

// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CSchedulerParam::V1_0>)

QByteArray CDatabaseAccessSettingsRepresentationControllerComp::GetTypeId() const
{
	return sdl::imtbase::ImtBaseTypes::CParamTypeIds::V1_0::ParamTypeIdsFields::DatabaseAccessSettings.toUtf8();
}


bool CDatabaseAccessSettingsRepresentationControllerComp::IsModelSupported(const istd::IChangeable &dataModel) const
{
	const imtdb::IDatabaseLoginSettings* databaseLoginSettingsPtr = dynamic_cast<const imtdb::IDatabaseLoginSettings*>(&dataModel);

	return databaseLoginSettingsPtr != nullptr;
}


bool CDatabaseAccessSettingsRepresentationControllerComp::GetSdlRepresentationFromDataModel(
			sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettings::V1_0& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const imtdb::IDatabaseLoginSettings* databaseLoginSettingsPtr = dynamic_cast<const imtdb::IDatabaseLoginSettings*>(&dataModel);
	Q_ASSERT(databaseLoginSettingsPtr != nullptr);
	if (databaseLoginSettingsPtr == nullptr){
		return false;
	}

	QString dbName = databaseLoginSettingsPtr->GetDatabaseName();
	sdlRepresentation.dbName = dbName;

	QString dbPath = databaseLoginSettingsPtr->GetDatabasePath();
	sdlRepresentation.dbPath = dbPath;

	QString host = databaseLoginSettingsPtr->GetHost();
	sdlRepresentation.host = host;

	QString password = databaseLoginSettingsPtr->GetPassword();
	sdlRepresentation.password = password;

	QString userName = databaseLoginSettingsPtr->GetUserName();
	sdlRepresentation.username = userName;

	int port = databaseLoginSettingsPtr->GetPort();
	sdlRepresentation.port = port;

	sdlRepresentation.canBackupRestore = m_canBackupRestoreAttrPtr.IsValid() ? *m_canBackupRestoreAttrPtr : true;

	return true;
}


bool CDatabaseAccessSettingsRepresentationControllerComp::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::imtbase::ImtBaseTypes::CDatabaseAccessSettings::V1_0& sdlRepresentation) const
{
	imtdb::IDatabaseLoginSettings* databaseLoginSettingsPtr = dynamic_cast<imtdb::IDatabaseLoginSettings*>(&dataModel);
	Q_ASSERT(databaseLoginSettingsPtr != nullptr);
	if (databaseLoginSettingsPtr == nullptr){
		return false;
	}

	if (sdlRepresentation.dbName){
		databaseLoginSettingsPtr->SetDatabaseName(*sdlRepresentation.dbName);
	}

	if (sdlRepresentation.dbPath){
		databaseLoginSettingsPtr->SetDatabasePath(*sdlRepresentation.dbPath);
	}

	if (sdlRepresentation.host){
		databaseLoginSettingsPtr->SetHost(*sdlRepresentation.host);
	}

	if (sdlRepresentation.password){
		databaseLoginSettingsPtr->SetPassword(*sdlRepresentation.password);
	}

	if (sdlRepresentation.username){
		databaseLoginSettingsPtr->SetUserName(*sdlRepresentation.username);
	}

	if (sdlRepresentation.port){
		databaseLoginSettingsPtr->SetPort(*sdlRepresentation.port);
	}

	return true;
}


} // namespace imtserverapp


