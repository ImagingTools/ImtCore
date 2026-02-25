// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlicdb/CFeatureDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>


namespace imtlicdb
{


// public methods

// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)

bool CFeatureDatabaseDelegateComp::CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
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

			if (key == "FeatureId"){
				iprm::TParamsPtr<iprm::IIdParam> idParamPtr(&filterParams, key);
				if (!idParamPtr.IsValid()){
					return false;
				}

				QByteArray value = idParamPtr->GetId();

				filterQuery = QString(R"(("Document"::text ILIKE '%"FeatureId": "%1"%'))").arg(qPrintable(value));
			}
			else if (key == "FeatureName"){
				iprm::TParamsPtr<iprm::IIdParam> nameParamPtr(&filterParams, key);
				if (!nameParamPtr.IsValid()){
					return false;
				}

				QByteArray value = nameParamPtr->GetId();

				filterQuery = QString(R"(("Document"::text ILIKE '%"FeatureName": "%1"%'))").arg(qPrintable(value));
			}
		}
	}

	return true;
}


bool CFeatureDatabaseDelegateComp::SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	BaseClass::SetCollectionItemMetaInfoFromRecord(record, metaInfo);

	if (record.contains("Document")){
		QByteArray json = record.value("Document").toByteArray();
		QJsonDocument jsonDocument = QJsonDocument::fromJson(json);

		if (!jsonDocument.isNull()){
			QString description = jsonDocument["FeatureDescription"].toString();
			metaInfo.SetMetaInfo(imtbase::ICollectionInfo::EIT_DESCRIPTION, description);

			QString name = jsonDocument["FeatureName"].toString();
			metaInfo.SetMetaInfo(imtbase::ICollectionInfo::EIT_NAME, name);
		}
	}

	return true;
}


} // namespace imtlicdb


