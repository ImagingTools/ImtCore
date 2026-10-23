// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantDocumentValidatorComp.h>


// Qt includes
#include <QtCore/QSet>

// ACF includes
#include <iprm/CIdParam.h>
#include <iprm/CParamsSet.h>
#include <iprm/CTextParam.h>

// ImtCore includes
#include <imtauth/ITenantInfo.h>
#include <imtauth/imtauth.h>
#include <imtbase/IOperationContext.h>


namespace imtauth
{

bool IsTenantPermissionScopeChangeAllowed(
			const QByteArray& creatorId,
			const QByteArrayList& currentPermissions,
			const QByteArrayList& newPermissions,
			const QByteArray& userId)
{
	if (QSet<QByteArray>(currentPermissions.begin(), currentPermissions.end()) == QSet<QByteArray>(newPermissions.begin(), newPermissions.end())){
		return true;
	}

	return !creatorId.isEmpty() && creatorId == userId;
}


bool CTenantDocumentValidatorComp::ValidateDocumentData(
			const QByteArray& objectId,
			const istd::IChangeable& document,
			QString& errorMessage,
			const imtbase::IOperationContext* operationContextPtr) const
{
	const imtauth::ITenantInfo* tenantInfo = dynamic_cast<const imtauth::ITenantInfo*>(&document);
	if (tenantInfo == nullptr){
		errorMessage = QStringLiteral("Invalid document type for tenant validation");
		return false;
	}

	const QString tenantName = tenantInfo->GetTenantName().trimmed();
	if (tenantName.isEmpty()){
		errorMessage = QStringLiteral("Organization name must be specified");
		return false;
	}

	if (!m_tenantCollectionCompPtr.IsValid()){
		errorMessage = QStringLiteral("Tenant collection is not configured");
		return false;
	}

	const imtauth::ITenantInfo* storedTenantInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr storedDataPtr;
	if (!objectId.isEmpty()){
		if (!m_tenantCollectionCompPtr->GetObjectData(objectId, storedDataPtr)){
			errorMessage = QStringLiteral("The stored organization could not be loaded");
			return false;
		}

		storedTenantInfoPtr = dynamic_cast<const imtauth::ITenantInfo*>(storedDataPtr.GetPtr());
		if (storedTenantInfoPtr == nullptr){
			errorMessage = QStringLiteral("Invalid stored organization for tenant validation");
			return false;
		}

		const QByteArrayList storedPermissionList = storedTenantInfoPtr->GetTenantPermissions();
		const QByteArrayList documentPermissions = tenantInfo->GetTenantPermissions();
		const QByteArray userId = operationContextPtr != nullptr ? operationContextPtr->GetOperationOwnerId().id : QByteArray();
		if (!IsTenantPermissionScopeChangeAllowed(storedTenantInfoPtr->GetCreatorId(), storedPermissionList, documentPermissions, userId)){
			errorMessage = QStringLiteral("Only the creator of the organization can change its permissions");
			return false;
		}
	}

	iprm::CParamsSet filterParams;

	iprm::CTextParam tenantNameParam;
	tenantNameParam.SetText(tenantName);
	filterParams.SetEditableParameter("TenantName", &tenantNameParam);

	if (!objectId.isEmpty()){
		iprm::CIdParam excludedTenantIdParam;
		excludedTenantIdParam.SetId(objectId);
		filterParams.SetEditableParameter("ExcludedTenantId", &excludedTenantIdParam);
	}

	const imtbase::ICollectionInfo::Ids foundIds = m_tenantCollectionCompPtr->GetElementIds(0, 1, &filterParams);
	if (!foundIds.isEmpty()){
		errorMessage = QStringLiteral("An organization with the same name already exists");
		return false;
	}

	// The parent may have been narrowed since the scope was edited
	const QByteArray parentTenantId = storedTenantInfoPtr != nullptr ? storedTenantInfoPtr->GetParentTenantId() : tenantInfo->GetParentTenantId();
	if (!parentTenantId.isEmpty()){
		imtbase::IObjectCollection::DataPtr parentDataPtr;
		const imtauth::ITenantInfo* parentTenantPtr = nullptr;
		if (m_tenantCollectionCompPtr->GetObjectData(parentTenantId, parentDataPtr)){
			parentTenantPtr = dynamic_cast<const imtauth::ITenantInfo*>(parentDataPtr.GetPtr());
		}

		if (parentTenantPtr == nullptr){
			errorMessage = QStringLiteral("The parent organization does not exist");
			return false;
		}

		const QByteArrayList parentPermissions = parentTenantPtr->GetTenantPermissions();
		for (const QByteArray& permission : tenantInfo->GetTenantPermissions()){
			if (!HasPermission(parentPermissions, permission, false)){
				errorMessage = QStringLiteral("Permission '%1' is not available to the parent organization").arg(QString::fromUtf8(permission));
				return false;
			}
		}
	}

	return true;
}


} // namespace imtauth
