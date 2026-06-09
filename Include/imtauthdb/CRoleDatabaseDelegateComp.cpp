// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CRoleDatabaseDelegateComp.h>


// ACF includes
#include <iprm/IIdParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/CRole.h>


namespace imtauthdb
{


// public methods

// reimplemented (imtdb::CSqlJsonDatabaseDelegateComp)

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


QString CRoleDatabaseDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	iprm::TParamsPtr<iprm::IIdParam> idParamPtr(&filterParams, "ParentListFilter");
	if (idParamPtr.IsValid()){
		QByteArray targetId = idParamPtr->GetId();
		return QString(R"(
			NOT EXISTS (
				WITH RECURSIVE descendants AS (
					SELECT g."DocumentId"
					FROM "Roles" g
					WHERE g."DocumentId" = '%1'
					AND g."State" = 'Active'
				
					UNION ALL
					SELECT child."DocumentId"
					FROM "Roles" child
					JOIN descendants d
						ON coalesce(child."Document"->'ParentsRoles', '[]'::jsonb)
							? (d."DocumentId")::text
					WHERE child."State" = 'Active'
				)
				SELECT 1
				FROM descendants d
				WHERE d."DocumentId" = root."DocumentId"
			)
					)").arg(QString::fromUtf8(targetId));
					}

	return QString();
}


} // namespace imtauthdb


