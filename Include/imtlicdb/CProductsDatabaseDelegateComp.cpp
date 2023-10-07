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

			if (key == "ProductId"){
				iprm::TParamsPtr<iprm::IIdParam> idParamPtr(&filterParams, key);
				if (!idParamPtr.IsValid()){
					return false;
				}

				QByteArray value = idParamPtr->GetId();

				filterQuery = QString(R"(("Document"::text ILIKE '%"ProductId": "%1"%'))").arg(qPrintable(value));
			}
			else if (key == "ProductName"){
				iprm::TParamsPtr<iprm::IIdParam> nameParamPtr(&filterParams, key);
				if (!nameParamPtr.IsValid()){
					return false;
				}

				QByteArray value = nameParamPtr->GetId();

				filterQuery = QString(R"(("Document"::text ILIKE '%"ProductName": "%1"%'))").arg(qPrintable(value));
			}
		}
	}

	return true;
}



} // namespace imtlicdb


