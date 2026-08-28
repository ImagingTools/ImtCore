// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CRoleDocumentValidatorComp.h>


// Qt includes
#include <QtCore/QSet>


// ImtCore includes
#include <imtauth/IRole.h>
#include <imtbase/IOperationContext.h>


namespace imtauth
{


bool CRoleDocumentValidatorComp::ValidateDocumentData(
			const QByteArray& objectId,
			const istd::IChangeable& document,
			QString& errorMessage,
			const imtbase::IOperationContext* operationContextPtr) const
{
	const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(&document);
	if (roleInfoPtr == nullptr){
		errorMessage = QStringLiteral("Invalid document type for role validation");
		return false;
	}

	const QByteArray roleId = roleInfoPtr->GetRoleId();
	if (roleId.isEmpty()){
		errorMessage = QStringLiteral("Role-ID can't be empty!");
		return false;
	}

	const QByteArray productId = roleInfoPtr->GetProductId();

	if (!m_roleCollectionCompPtr.IsValid()){
		errorMessage = QStringLiteral("Role collection is not configured");
		return false;
	}

	imtbase::ICollectionInfo::Ids collectionIds = m_roleCollectionCompPtr->GetElementIds();
	for (const imtbase::ICollectionInfo::Id& collectionId : collectionIds){
		if (collectionId == objectId){
			continue;
		}

		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_roleCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
			const imtauth::IRole* currentRoleInfoPtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());
			if (currentRoleInfoPtr != nullptr){
				if (currentRoleInfoPtr->GetRoleId() == roleId && currentRoleInfoPtr->GetProductId() == productId){
					errorMessage = QString("Role with ID: '%1' already exists").arg(roleId);
					return false;
				}
			}
		}
	}

	QByteArray tenantId;
	if (operationContextPtr != nullptr){
		tenantId = operationContextPtr->GetTenantId().trimmed();
	}

	if (!tenantId.isEmpty()){
		if (!m_tenantManagerCompPtr.IsValid()){
			errorMessage = QStringLiteral("Tenant manager is not configured for tenant-scoped role permissions validation");
			return false;
		}

		QSet<QByteArray> tenantPermissionsSet;
		const QByteArrayList tenantPermissions = m_tenantManagerCompPtr->GetTenantPermissions(tenantId);
		for (const QByteArray& permissionId : tenantPermissions){
			const QByteArray normalizedPermissionId = permissionId.trimmed();
			if (!normalizedPermissionId.isEmpty()){
				tenantPermissionsSet.insert(normalizedPermissionId);
			}
		}

		const imtauth::IRole::FeatureIds rolePermissions = roleInfoPtr->GetLocalPermissions();
		for (const QByteArray& permissionId : rolePermissions){
			const QByteArray normalizedPermissionId = permissionId.trimmed();
			if (normalizedPermissionId.isEmpty()){
				continue;
			}

			if (!tenantPermissionsSet.contains(normalizedPermissionId)){
				errorMessage = QStringLiteral("Permission '%1' is not allowed for tenant '%2'")
						.arg(QString::fromUtf8(normalizedPermissionId), QString::fromUtf8(tenantId));
				return false;
			}
		}
	}

	return true;
}


} // namespace imtauth
