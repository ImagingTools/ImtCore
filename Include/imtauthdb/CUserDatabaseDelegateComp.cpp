#include <imtauthdb/CUserDatabaseDelegateComp.h>


namespace imtauthdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeable* CUserDatabaseDelegateComp::CreateObjectFromRecord(const QByteArray& /*typeId*/, const QSqlRecord& record) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_userInfoFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtauth::IUserInfo> userPtr = m_userInfoFactCompPtr.CreateInstance();
	if (!userPtr.IsValid()){
		return nullptr;
	}

	QByteArray userId;
	if (record.contains("Id")){
		userId = record.value("Id").toByteArray();
		userPtr->SetId(userId);
	}

	QString username;
	if (record.contains("Username")){
		username = record.value("Username").toString();
		userPtr->SetUsername(username);
	}

	QString name;
	if (record.contains("Name")){
		name = record.value("Name").toString();
		userPtr->SetName(name);
	}

	QByteArray passwordHash;
	if (record.contains("PasswordHash")){
		passwordHash = record.value("Id").toByteArray();
		userPtr->SetPasswordHash(passwordHash);
	}

	QString mail;
	if (record.contains("Mail")){
		mail = record.value("Mail").toString();
		userPtr->SetMail(mail);
	}

	return userPtr.PopPtr();
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CUserDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& /*proposedObjectId*/,
			const QString& objectName,
			const QString& objectDescription,
			const istd::IChangeable* valuePtr) const
{
	return NewObjectQuery();
}


QByteArray CUserDatabaseDelegateComp::CreateDeleteObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId) const
{
	return QByteArray();
}

QByteArray CUserDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const istd::IChangeable& object) const
{
	return QByteArray();
}


QByteArray CUserDatabaseDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const QString& newObjectName) const
{
	return QByteArray();
}


QByteArray CUserDatabaseDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const QString& description) const
{
	return QByteArray();
}


// protected methods
// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)

idoc::MetaInfoPtr CUserDatabaseDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	return idoc::MetaInfoPtr();
}


bool CUserDatabaseDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& /*record*/, idoc::IDocumentMetaInfo& /*metaInfo*/) const
{
	return true;
}


} // namespace imtauthdb


