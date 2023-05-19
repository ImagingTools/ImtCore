#include <imtauthdb/CUsersSessionsDatabaseDelegateComp.h>


// ImtCore includes
#include <imtlic/CFeatureInfo.h>
#include <imtauth/CSessionInfo.h>
#include <imtauth/CUserInfo.h>


namespace imtauthdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeable* CUsersSessionsDatabaseDelegateComp::CreateObjectFromRecord(const QSqlRecord& record) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtauth::CSessionInfo> sessionInfoPtr = new imtauth::CSessionInfo();

	QByteArray token;
	if (record.contains("AccessToken")){
		token = record.value("AccessToken").toByteArray();
		sessionInfoPtr->SetToken(token);
	}

	QByteArray userId;
	if (record.contains("UserId")){
		userId = record.value("UserId").toByteArray();
	}

	sessionInfoPtr->SetUserId(userId);

	return sessionInfoPtr.PopPtr();
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CUsersSessionsDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& /*typeId*/,
			const QByteArray& /*proposedObjectId*/,
			const QString& /*objectName*/,
			const QString& /*objectDescription*/,
			const istd::IChangeable* valuePtr) const
{
	const imtauth::ISession* sessionPtr = dynamic_cast<const imtauth::ISession*>(valuePtr);
	if (sessionPtr == nullptr){
		return NewObjectQuery();
	}

	QByteArray token = sessionPtr->GetToken();

	QByteArray userId = sessionPtr->GetUserId();

	NewObjectQuery retVal;

    retVal.query += QString("\nINSERT INTO \"UserSessions\" (\"AccessToken\", \"UserId\", \"LastActivity\") VALUES ('%1', '%2', '%3');")
			.arg(qPrintable(token))
			.arg(qPrintable(userId))
			.arg(QDateTime::currentDateTime().toString(Qt::ISODate)).toLocal8Bit();

	return retVal;
}


QByteArray CUsersSessionsDatabaseDelegateComp::CreateDeleteObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/) const
{
	return QByteArray();
}


QByteArray CUsersSessionsDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const istd::IChangeable& /*object*/,
			const ContextDescription& /*context*/,
			bool /*useExternDelegate*/) const
{
	return QByteArray();
}


QByteArray CUsersSessionsDatabaseDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*newObjectName*/) const
{
	return QByteArray();
}


QByteArray CUsersSessionsDatabaseDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const QString& /*description*/) const
{
	return QByteArray();
}


} // namespace imtauthdb


