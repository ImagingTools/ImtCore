#include <imtlicdb/CProductsDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>


namespace imtlicdb
{


// public methods

// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)

bool CProductsDatabaseDelegateComp::CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
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

			filterQuery += QString(R"(("Document"->>'%1' = '%2'))").arg(qPrintable(key)).arg(qPrintable(value));
		}
	}

	return true;
}



} // namespace imtlicdb


