#include <imtauthdb/CUsersSettingsDatabaseDelegateComp.h>


// ACF includes
#include <iser/CCompactXmlMemWriteArchive.h>
#include <iser/CCompactXmlMemReadArchive.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtauth/CUserSettings.h>


namespace imtauthdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray CUsersSettingsDatabaseDelegateComp::GetSelectionQuery(
			const QByteArray& objectId,
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	if (!objectId.isEmpty()){
		return QString("SELECT * FROM \"%1\" WHERE \"%2\" = '%3'")
					.arg(qPrintable(*m_tableNameAttrPtr))
					.arg(qPrintable(*m_objectIdColumnAttrPtr))
					.arg(qPrintable(objectId))
					.toUtf8();
	}
	else{
		QString retVal = GetBaseSelectionQuery();

		return retVal.toUtf8();
	}

	return QByteArray();
}


istd::IChangeableUniquePtr CUsersSettingsDatabaseDelegateComp::CreateObjectFromRecord(const QSqlRecord& record) const
{
	if (!m_databaseEngineCompPtr.IsValid() || !m_userSettingsInfoFactCompPtr.IsValid()){
		return nullptr;
	}
	
	imtauth::IUserSettingsUniquePtr userSettingsPtr = m_userSettingsInfoFactCompPtr.CreateInstance();
	if (!userSettingsPtr.IsValid()){
		return nullptr;
	}

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

			iser::CCompactXmlMemReadArchive archive(data);

			if (!paramSetPtr->Serialize(archive)){
				return nullptr;
			}
		}
	}

	istd::IChangeableUniquePtr retVal;
	retVal.MoveCastedPtr(userSettingsPtr);

	return retVal;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CUsersSettingsDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& /*proposedObjectId*/,
			const QString& /*objectName*/,
			const QString& /*objectDescription*/,
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
		iser::CCompactXmlMemWriteArchive archive(m_versionInfoCompPtr.GetPtr());

		bool retVal = settingsPtr->Serialize(archive);
		if (!retVal){
			return NewObjectQuery();
		}

		data = archive.GetString();
	}

	NewObjectQuery retVal;

	retVal.query += QString("\nINSERT INTO \"UserSettings\" (\"UserId\", \"Settings\") VALUES ('%1', '%2');")
				.arg(qPrintable(userId))
				.arg(qPrintable(data)).toUtf8();

	return retVal;
}


QByteArray CUsersSettingsDatabaseDelegateComp::CreateDeleteObjectsQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArrayList& objectIds,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (objectIds.isEmpty()){
		return QByteArray();
	}
	
	QStringList quotedIds;
	for (const QByteArray& objectId : objectIds){
		quotedIds << QString("'%1'").arg(objectId);
	}
	
	QString query = QString(
						"DELETE FROM \"UserSettings\" WHERE \"UserId\" IN (%3);")
						.arg(
							quotedIds.join(", ")
							);
	
	return query.toUtf8();
}


QByteArray CUsersSettingsDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
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
		iser::CCompactXmlMemWriteArchive archive(m_versionInfoCompPtr.GetPtr());

		bool retVal = settingsPtr->Serialize(archive);
		if (!retVal){
			return QByteArray();
		}

		data = archive.GetString();
	}

	QByteArray retVal = QString("UPDATE \"UserSettings\" SET \"UserId\" ='%1', \"Settings\" = '%2' WHERE \"UserId\" ='%3';")
			.arg(qPrintable(userId))
			.arg(qPrintable(data))
			.arg(qPrintable(objectId))
			.toUtf8();

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


