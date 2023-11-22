#include <imtlicdb/CProductsDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtlic/IProductInfo.h>


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


bool CProductsDatabaseDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	BaseClass::SetObjectMetaInfoFromRecord(record, metaInfo);

	if (record.contains("Document")){
		QByteArray json = record.value("Document").toByteArray();
		QJsonDocument jsonDocument = QJsonDocument::fromJson(json);
		if (!jsonDocument.isNull()){
			QString productId = jsonDocument["ProductId"].toString();
			metaInfo.SetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_ID, productId);

			QString productName = jsonDocument["ProductName"].toString();
			metaInfo.SetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_NAME, productName);

			QString description = jsonDocument["ProductDescription"].toString();
			metaInfo.SetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_DESCRIPTION, description);

			QString category = jsonDocument["CategoryId"].toString();
			metaInfo.SetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_CATEGORY, category);
		}
	}

	return true;
}


} // namespace imtlicdb


