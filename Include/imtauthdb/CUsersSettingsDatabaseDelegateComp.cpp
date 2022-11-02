#include <imtauthdb/CUsersSettingsDatabaseDelegateComp.h>


// ACF includes
#include <iser/CJsonStringWriteArchive.h>
#include <iser/CJsonStringReadArchive.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtlic/CFeatureInfo.h>
#include <imtauth/CUserSettings.h>
#include <imtauth/CUserInfo.h>


namespace imtauthdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeable* CUsersSettingsDatabaseDelegateComp::CreateObjectFromRecord(const QByteArray& /*typeId*/, const QSqlRecord& record) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtauth::CUserSettings> userSettingsPtr = new imtauth::CUserSettings();

	QByteArray userId;
	if (record.contains("UserId")){
		userId = record.value("UserId").toByteArray();
	}

	userSettingsPtr->SetUserId(userId);

	QByteArray settingsData;
	if (record.contains("Settings")){
		settingsData = record.value("Settings").toByteArray();
	}

	userSettingsPtr->SetSettings(settingsData);

	return userSettingsPtr.PopPtr();
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CUsersSettingsDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& /*proposedObjectId*/,
			const QString& objectName,
			const QString& objectDescription,
			const istd::IChangeable* valuePtr) const
{
	const imtauth::IUserSettings* userSettingsPtr = dynamic_cast<const imtauth::IUserSettings*>(valuePtr);
	if (userSettingsPtr == nullptr){
		return NewObjectQuery();
	}

//	iprm::IParamsSet* settingsPtr = userSettingsPtr->GetSettings();
	QByteArray settingsJson = userSettingsPtr->GetSettings();
	QByteArray userId = userSettingsPtr->GetUserId();

//	QByteArray data;
//	iser::CJsonStringWriteArchive archive(data);
//	settingsPtr->Serialize(archive);

	NewObjectQuery retVal;

	retVal.query += QString("\nINSERT INTO \"UsersSettings\" (UserId, Settings) VALUES ('%1', '%2');")
			.arg(qPrintable(userId))
			.arg(qPrintable(settingsJson)).toLocal8Bit();

	return retVal;
}


QByteArray CUsersSettingsDatabaseDelegateComp::CreateDeleteObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId) const
{
	return QString("\nDELETE FROM \"UsersSettings\" WHERE UserId = '%1';")
			.arg(qPrintable(objectId)).toLocal8Bit();
}


QByteArray CUsersSettingsDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const istd::IChangeable& object) const
{
	const imtauth::IUserSettings* userSettingsPtr = dynamic_cast<const imtauth::IUserSettings*>(&object);
	if (userSettingsPtr == nullptr){
		return QByteArray();
	}

	QByteArray userId = userSettingsPtr->GetUserId();
	QByteArray settingsData = userSettingsPtr->GetSettings();

	QByteArray retVal = QString("UPDATE \"UsersSettings\" SET UserId ='%1', Settings = '%2' WHERE UserId ='%3';")
			.arg(qPrintable(userId))
			.arg(qPrintable(settingsData))
			.arg(qPrintable(objectId))
			.toLocal8Bit();

	return retVal;
}


QByteArray CUsersSettingsDatabaseDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const QString& newObjectName) const
{
	return QByteArray();
}


QByteArray CUsersSettingsDatabaseDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const QString& description) const
{
	return QByteArray();
}


} // namespace imtauthdb


