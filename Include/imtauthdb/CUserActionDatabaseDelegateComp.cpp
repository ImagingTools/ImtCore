// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CUserActionDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/IUserGroupFilter.h>


namespace imtauthdb
{

namespace
{

QString GetSqlResourcePath(const imtdb::IDatabaseEngine& databaseEngine, const QString& fileName)
{
	const QByteArray databaseDriverId = databaseEngine.GetDatabaseDriverId();
	const bool isSqlite = databaseDriverId.compare(QByteArrayLiteral("QSQLITE"), Qt::CaseInsensitive) == 0;
	const QString prefix = isSqlite ? QStringLiteral(":/SQL/SQLite/") : QStringLiteral(":/SQL/Postgres/");
	return prefix + fileName;
}

} // namespace


// protected methods

// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)

QString CUserActionDatabaseDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	if (!TableExists(QByteArrayLiteral("Users"))){
		return QString();
	}

	QString filterQuery;

	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();
	if (paramIds.contains("GroupFilter")){
		iprm::TParamsPtr<imtauth::IUserGroupFilter> filterParamPtr(&filterParams, "GroupFilter");
		if (filterParamPtr.IsValid()){
			QByteArray userId = filterParamPtr->GetUserId();
			QByteArrayList groupIds = filterParamPtr->GetGroupIds();

			if (!groupIds.isEmpty()){
				QString array = "array[";

				for (int i = 0; i < groupIds.size(); i++){
					if (i > 0){
						array += ",";
					}

					array += "'" + SqlEncode(QString::fromUtf8(groupIds[i])) + "'";
				}

				array += "]";

				filterQuery += QStringLiteral(R"((users."Document"->'Groups' ?| %1))").arg(array);
			}
			else{
				filterQuery += QStringLiteral(R"(users."Document"->>'Id' = '%1')").arg(SqlEncode(QString::fromUtf8(userId)));
			}
		}
	}

	return filterQuery;
}


QByteArray CUserActionDatabaseDelegateComp::CreateJoinTablesQuery() const
{
	if (!TableExists(QByteArrayLiteral("Users"))){
		return QByteArray();
	}

	return QByteArray(R"(
			LEFT JOIN "Users" AS users
				ON (
					users."DocumentId"::text = root."DataMetaInfo"->>'userId'
					AND users."State" = 'Active'
				)
	)");
}


} // namespace imtauthdb

