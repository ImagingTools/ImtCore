#include <imtauthdb/CUserActionDatabaseDelegateComp.h>


// ImtCore includes
#include <imtauth/IUserRecentAction.h>


namespace imtauthdb
{


// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)

istd::IChangeableUniquePtr CUserActionDatabaseDelegateComp::CreateObjectFromRecord(
			const QSqlRecord& record,
			const iprm::IParamsSet* paramsPtr) const
{
	istd::IChangeableUniquePtr documentPtr = CreateObject("UserAction");
	if (!documentPtr.IsValid()){
		return nullptr;
	}

	imtauth::IUserRecentAction* userRecentActionPtr = dynamic_cast<imtauth::IUserRecentAction*>(documentPtr.GetPtr());
	if (userRecentActionPtr == nullptr){
		return nullptr;
	}

	return documentPtr;
}


QByteArray CUserActionDatabaseDelegateComp::GetSelectionQuery(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* paramsPtr) const
{
	int tableCount = m_databaseDelegatesCompPtr.GetCount();
	if (tableCount == 0){
		return QByteArray();
	}

	QByteArray retVal = QString(R"(SELECT "Id","DocumentId" as "TargetId","TypeId" as "TargetTypeId","RevisionInfo"->>'OwnerId' as "UserId","TimeStamp" FROM ()").toUtf8();

	for (int i = 0; i < tableCount; ++i){
		const imtdb::ISqlDatabaseObjectDelegate* databaseDelegatePtr = m_databaseDelegatesCompPtr[i];
		if (databaseDelegatePtr != nullptr){
			QString tableName = databaseDelegatePtr->GetTableName();
			retVal += QString(R"(SELECT * FROM "%1")").arg(tableName).toUtf8();

			if (i < tableCount - 1){
				retVal += QString(R"( UNION ALL )").toUtf8();
			}
		}
	}

	retVal += QString(R"() AS "UserActions" ORDER BY "TimeStamp" DESC)").toUtf8();

	return retVal;
}


} // namespace imtauthdb


