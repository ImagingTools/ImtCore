#include <imtauthdb/CRoleDatabaseDelegateComp.h>


// ACF includes
#include <iprm/IEnableableParam.h>
#include <iprm/CTextParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/CRole.h>


namespace imtauthdb
{


// public methods

// reimplemented (imtdb::CSqlJsonDatabaseDelegateComp)

bool CRoleDatabaseDelegateComp::CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
{
	iprm::TParamsPtr<iprm::IEnableableParam> isDefaultParamPtr(&filterParams, "IsDefault");
	if (isDefaultParamPtr.IsValid()){
		if (isDefaultParamPtr->IsEnabled()){
			filterQuery = QString("(\"Document\"->>'IsDefault')::boolean is TRUE");
		}
		else{
			filterQuery = QString("(\"Document\"->>'IsDefault')::boolean is FALSE");
		}
	}

	iprm::TParamsPtr<iprm::IEnableableParam> isGuestParamPtr(&filterParams, "IsGuest");
	if (isGuestParamPtr.IsValid()){
		if (isGuestParamPtr->IsEnabled()){
			filterQuery = QString("(\"Document\"->>'IsGuest')::boolean is TRUE");
		}
		else{
			filterQuery = QString("(\"Document\"->>'IsGuest')::boolean is FALSE");
		}
	}

	return BaseClass::CreateObjectFilterQuery(filterParams, filterQuery);
}


QByteArray CRoleDatabaseDelegateComp::CreateDeleteObjectsQuery(
			const imtbase::IObjectCollection& collection,
			const imtbase::ICollectionInfo::Ids& objectIds,
			const imtbase::IOperationContext* operationContextPtr) const
{
	for (const imtbase::ICollectionInfo::Id& objectId : objectIds){
		imtauth::CRole* roleInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (collection.GetObjectData(objectId, dataPtr)){
			roleInfoPtr = dynamic_cast<imtauth::CRole*>(dataPtr.GetPtr());
		}
		
		if (roleInfoPtr == nullptr){
			return QByteArray();
		}
		
		if (roleInfoPtr->IsDefault() || roleInfoPtr->IsGuest()){
			return QByteArray();
		}
	}

	return BaseClass::CreateDeleteObjectsQuery(collection, objectIds, operationContextPtr);
}


bool CRoleDatabaseDelegateComp::SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	BaseClass::SetCollectionItemMetaInfoFromRecord(record, metaInfo);

	if (record.contains("Document")){
		QByteArray json = record.value("Document").toByteArray();
		QJsonDocument jsonDocument = QJsonDocument::fromJson(json);

		if (!jsonDocument.isNull()){
			QString description = jsonDocument["RoleDescription"].toString();
			metaInfo.SetMetaInfo(imtbase::ICollectionInfo::EIT_DESCRIPTION, description);

			QString name = jsonDocument["RoleName"].toString();
			metaInfo.SetMetaInfo(imtbase::ICollectionInfo::EIT_NAME, name);
		}
	}

	return true;
}


} // namespace imtauthdb


