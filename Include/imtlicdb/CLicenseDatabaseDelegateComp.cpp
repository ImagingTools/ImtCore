#include <imtlicdb/CLicenseDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>


namespace imtlicdb
{


// public methods

QString CLicenseDatabaseDelegateComp::GetBaseSelectionQuery() const
{
	return R"(
			SELECT
				"DocumentId",
				"Document",
				"LastModified",
				(SELECT "LastModified" FROM "Licenses" as t2 WHERE "RevisionNumber" = 1 AND t2."DocumentId" = t1."DocumentId" LIMIT 1) as "Added",
				(SELECT "Document"->>'ProductId' FROM "Products" as pr WHERE pr."DocumentId" = t1."Document"->>'ProductId' AND pr."IsActive" = true) as "ProductId"
			FROM "Licenses" as t1 WHERE "IsActive" = true
		)";
}


// reimplemented (imtdb::CSqlJsonDatabaseDelegateComp)

bool CLicenseDatabaseDelegateComp::CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
{
	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();
	if (!paramIds.isEmpty()){
#if QT_VERSION >= 0x051500
		QByteArrayList idList(paramIds.cbegin(), paramIds.cend());
#else
		QByteArrayList idList = paramIds.toList();
#endif
		for (int i = 0; i < idList.size(); i++){
			QByteArray key = idList[i];

			iprm::TParamsPtr<iprm::IIdParam> idParamPtr(&filterParams, key);
			if (!idParamPtr.IsValid()){
				return false;
			}

			if (!filterQuery.isEmpty()){
				filterQuery += " AND ";
			}

			QByteArray value = idParamPtr->GetId();

			filterQuery = QString(R"((lower("Document"->>'%1') = lower('%2')))").arg(qPrintable(key)).arg(qPrintable(value));
		}
	}

	return true;
}


} // namespace imtlicdb


