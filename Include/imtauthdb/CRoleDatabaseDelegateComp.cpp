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
	const QSet<QByteArray> idsToDelete(objectIds.begin(), objectIds.end());

	for (const imtbase::ICollectionInfo::Id& objectId : objectIds){
		imtauth::CRole* roleInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (collection.GetObjectData(objectId, dataPtr)){
			roleInfoPtr = dynamic_cast<imtauth::CRole*>(dataPtr.GetPtr());
		}
		
		if (roleInfoPtr == nullptr){
			return QByteArray();
		}
		
		// A default/guest role can only be removed when it is a duplicate, i.e. when another
		// role with the same product and the same flag survives the deletion. This still
		// guarantees that at least one default role and one guest role per product is kept,
		// while allowing clean-up of duplicated entries (e.g. created on first LDAP login).
		if (roleInfoPtr->IsDefault() || roleInfoPtr->IsGuest()){
			if (!HasOtherSpecialRole(collection, *roleInfoPtr, idsToDelete)){
				return QByteArray();
			}
		}
	}

	return BaseClass::CreateDeleteObjectsQuery(collection, objectIds, operationContextPtr);
}


bool CRoleDatabaseDelegateComp::HasOtherSpecialRole(
			const imtbase::IObjectCollection& collection,
			const imtauth::IRole& roleToDelete,
			const QSet<QByteArray>& idsToDelete) const
{
	const QByteArray productId = roleToDelete.GetProductId();
	const bool matchDefault = roleToDelete.IsDefault();
	const bool matchGuest = roleToDelete.IsGuest();

	const imtbase::ICollectionInfo::Ids allIds = collection.GetElementIds();
	for (const imtbase::ICollectionInfo::Id& candidateId : allIds){
		if (idsToDelete.contains(candidateId)){
			continue;
		}

		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!collection.GetObjectData(candidateId, dataPtr)){
			continue;
		}

		const imtauth::IRole* candidatePtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());
		if (candidatePtr == nullptr){
			continue;
		}

		if (candidatePtr->GetProductId() != productId){
			continue;
		}

		if ((matchDefault && candidatePtr->IsDefault()) || (matchGuest && candidatePtr->IsGuest())){
			return true;
		}
	}

	return false;
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


