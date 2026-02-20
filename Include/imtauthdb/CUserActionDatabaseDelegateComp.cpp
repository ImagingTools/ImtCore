// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CUserActionDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/IUserGroupFilter.h>


namespace imtauthdb
{


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

					array += "'" + groupIds[i] + "'";
				}

				array += "]";

				filterQuery += QString(R"((users."Document"->'Groups' ?| %1))").arg(array);
			}
			else{
				filterQuery += QString(R"(users."Document"->>'Id' = '%1')").arg(qPrintable(userId));
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


// reimplemented (icomp::CComponentBase)

void CUserActionDatabaseDelegateComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_databaseEngineCompPtr.IsValid()){
		QString tableName = GetTableName();

		if (!TableExists(tableName)){
			QFile scriptFile(":/SQL/CreateCollectionTable.sql");
			if (!scriptFile.open(QFile::ReadOnly)){
				SendErrorMessage(0, QT_TR_NOOP(QString("Collection table creation script '%1'could not be loaded").arg(scriptFile.fileName())));
				return;
			}

			QByteArray createTableQuery = scriptFile.readAll();
			scriptFile.close();

			createTableQuery.replace("${TableName}", tableName.toUtf8());
			createTableQuery.replace("${TableScheme}", "public");

			QSqlError sqlError;
			m_databaseEngineCompPtr->ExecSqlQuery(createTableQuery, &sqlError);
	
			if (sqlError.type() != QSqlError::NoError){
				qCritical() << __FILE__ << __LINE__
							<< "\n\t| Table could not be created"
							<< "\n\t| Error: " << sqlError
							<< "\n\t| Query: " << createTableQuery;
	
				SendErrorMessage(0, QT_TR_NOOP(QString("\n\t| Table could not be created"
														"\n\t| Error: %1"
														 "\n\t| Query: %2")
													.arg(sqlError.text(), qPrintable(createTableQuery))));
			}
		}
	}
}


} // namespace imtauthdb


