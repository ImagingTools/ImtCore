// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CRoleDatabaseDelegateComp.h>


// ACF includes
#include <iprm/IIdParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/CRole.h>
#include <imtauth/IUserBaseInfo.h>
#include <imtauthdb/imtauthdb.h>


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

	QByteArray retVal = BaseClass::CreateDeleteObjectsQuery(collection, objectIds, operationContextPtr);
	if (retVal.isEmpty()){
		return retVal;
	}

	// Users, groups and roles must not keep referencing the removed roles.
	const QByteArrayList roleIds(objectIds.cbegin(), objectIds.cend());

	iprm::CParamsSet roleFilterParams;
	SetFilterIds(roleFilterParams, s_roleFilterId, roleIds);

	auto removeRoles = [&roleIds](imtauth::IUserBaseInfo& userBaseInfo){
		RemovedReferences removedReferences;
		for (const QByteArray& productId : userBaseInfo.GetProducts()){
			for (const QByteArray& roleId : roleIds){
				if (userBaseInfo.RemoveRole(productId, roleId)){
					removedReferences << RemovedReference{"RemoveRole", "RoleId", QT_TRANSLATE_NOOP("Attribute", "Role"), roleId};
				}
			}
		}
		return removedReferences;
	};

	if (m_userCollectionCompPtr.IsValid() && m_userDatabaseDelegateCompPtr.IsValid()){
		retVal += CreateReferenceRemovalQuery<imtauth::IUserBaseInfo>(
					*m_userCollectionCompPtr, *m_userDatabaseDelegateCompPtr, roleFilterParams, imtbase::ICollectionInfo::Ids(), removeRoles, operationContextPtr);
	}

	if (m_userGroupCollectionCompPtr.IsValid() && m_userGroupDatabaseDelegateCompPtr.IsValid()){
		retVal += CreateReferenceRemovalQuery<imtauth::IUserBaseInfo>(
					*m_userGroupCollectionCompPtr, *m_userGroupDatabaseDelegateCompPtr, roleFilterParams, imtbase::ICollectionInfo::Ids(), removeRoles, operationContextPtr);
	}

	iprm::CParamsSet parentFilterParams;
	SetFilterIds(parentFilterParams, s_parentFilterId, roleIds);

	retVal += CreateReferenceRemovalQuery<imtauth::IRole>(
				collection,
				*this,
				parentFilterParams,
				objectIds,
				[&roleIds](imtauth::IRole& role){
					RemovedReferences removedReferences;
					const QByteArrayList parentRoleIds = role.GetIncludedRoles();
					for (const QByteArray& roleId : roleIds){
						if (parentRoleIds.contains(roleId)){
							role.ExcludeRole(roleId);
							removedReferences << RemovedReference{"RemoveParentRole", "ParentRoleId", QT_TRANSLATE_NOOP("Attribute", "Parent Role"), roleId};
						}
					}
					return removedReferences;
				},
				operationContextPtr);

	return retVal;
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
	QStringList conditions;

	const QByteArrayList parentRoleIds = GetFilterIds(filterParams, s_parentFilterId);
	if (!parentRoleIds.isEmpty()){
		conditions << CreateArrayContainsAnyQuery(IsSQLite(), QStringLiteral("ParentsRoles"), parentRoleIds);
	}

	iprm::TParamsPtr<iprm::IIdParam> idParamPtr(&filterParams, "ParentListFilter");
	if (idParamPtr.IsValid()){
		QByteArray targetId = idParamPtr->GetId();
		conditions << QStringLiteral(R"(
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
					)").arg(targetId);
	}

	return conditions.join(QStringLiteral(" AND "));
}


} // namespace imtauthdb


