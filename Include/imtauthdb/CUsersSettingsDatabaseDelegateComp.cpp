#include <imtauthdb/CUsersSettingsDatabaseDelegateComp.h>


// ACF includes
#include <iser/CMemoryWriteArchive.h>
#include <iser/CMemoryReadArchive.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtlic/CFeatureInfo.h>
#include <imtauth/CUserSettings.h>
#include <imtauth/CUserInfo.h>


namespace imtauthdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeable* CUsersSettingsDatabaseDelegateComp::CreateObjectFromRecord(const QSqlRecord& record) const
{
	if (!m_databaseEngineCompPtr.IsValid() || !m_userSettingsInfoFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtauth::IUserSettings> userSettingsPtr = m_userSettingsInfoFactCompPtr.CreateInstance();

	QByteArray userId;
	if (record.contains("UserId")){
		userId = record.value("UserId").toByteArray();
	}

	userSettingsPtr->SetUserId(userId);

	QByteArray data;

	iprm::IParamsSet* paramSetPtr = userSettingsPtr->GetSettings();
	if (paramSetPtr != nullptr){
		if (record.contains("Settings")){
			data = record.value("Settings").toByteArray();

			data = QByteArray::fromBase64(record.value("Settings").toByteArray());

			iser::CMemoryReadArchive archive(data.constData(), data.size());

			if (!paramSetPtr->Serialize(archive)){
				return nullptr;
			}

//			userSettingsPtr->SetSettings(paramSetPtr);
		}
	}

	return userSettingsPtr.PopPtr();
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CUsersSettingsDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& /*proposedObjectId*/,
			const QString& objectName,
			const QString& objectDescription,
			const istd::IChangeable* valuePtr,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	const imtauth::IUserSettings* userSettingsPtr = dynamic_cast<const imtauth::IUserSettings*>(valuePtr);
	if (userSettingsPtr == nullptr){
		return NewObjectQuery();
	}

	iprm::IParamsSet* settingsPtr = userSettingsPtr->GetSettings();

	if (settingsPtr == nullptr){
		return NewObjectQuery();
	}

	QByteArray userId = userSettingsPtr->GetUserId();

	QByteArray data;
	{
		iser::CMemoryWriteArchive archive(m_versionInfoCompPtr.GetPtr());

		bool retVal = settingsPtr->Serialize(archive);
		if (!retVal){
			return NewObjectQuery();
		}

		data = QByteArray((const char*) archive.GetBuffer(), archive.GetBufferSize()).toBase64();
	}

	NewObjectQuery retVal;

    retVal.query += QString("\nINSERT INTO \"UserSettings\" (\"UserId\", \"Settings\") VALUES ('%1', '%2');")
			.arg(qPrintable(userId))
			.arg(qPrintable(data)).toLocal8Bit();

	return retVal;
}


QByteArray CUsersSettingsDatabaseDelegateComp::CreateDeleteObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QString("\nDELETE FROM \"UserSettings\" WHERE \"UserId\" = '%1';")
			.arg(qPrintable(objectId)).toLocal8Bit();
}


QByteArray CUsersSettingsDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* /*operationContextPtr*/,
			bool /*useExternDelegate*/) const
{
	const imtauth::IUserSettings* userSettingsPtr = dynamic_cast<const imtauth::IUserSettings*>(&object);
	if (userSettingsPtr == nullptr){
		return QByteArray();
	}

	QByteArray userId = userSettingsPtr->GetUserId();

	iprm::IParamsSet* settingsPtr = userSettingsPtr->GetSettings();
	if (settingsPtr == nullptr){
		return QByteArray();
	}

	QByteArray data;
	{
		iser::CMemoryWriteArchive archive;

		bool retVal = settingsPtr->Serialize(archive);
		if (!retVal){
			return QByteArray();
		}

		data = QByteArray((const char*) archive.GetBuffer(), archive.GetBufferSize()).toBase64();
	}

    QByteArray retVal = QString("UPDATE \"UserSettings\" SET \"UserId\" ='%1', \"Settings\" = '%2' WHERE \"UserId\" ='%3';")
			.arg(qPrintable(userId))
			.arg(qPrintable(data))
			.arg(qPrintable(objectId))
			.toLocal8Bit();

	return retVal;
}


QByteArray CUsersSettingsDatabaseDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*newObjectName*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


QByteArray CUsersSettingsDatabaseDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*description*/,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	return QByteArray();
}


} // namespace imtauthdb


