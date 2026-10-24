// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CRoleDocumentValidatorComp.h>


// Qt includes
#include <QtCore/QSet>

// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtauth/CTenantFilterParam.h>
#include <imtauth/IRole.h>
#include <imtbase/CComplexCollectionFilter.h>
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

	QByteArray tenantId;
	if (operationContextPtr != nullptr){
		tenantId = operationContextPtr->GetTenantId().trimmed();
	}

	if (!CheckRoleIdUnique(*m_roleCollectionCompPtr, objectId, roleId, productId, tenantId, errorMessage)){
		return false;
	}

	if (!CheckParentRolesInTenant(*m_roleCollectionCompPtr, roleInfoPtr->GetIncludedRoles(), tenantId, errorMessage)){
		return false;
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


bool CRoleDocumentValidatorComp::CheckRoleIdUnique(
			const imtbase::IObjectCollection& roleCollection,
			const QByteArray& objectId,
			const QByteArray& roleId,
			const QByteArray& productId,
			const QByteArray& tenantId,
			QString& errorMessage)
{
	imtauth::CTenantFilterParam tenantFilter;
	tenantFilter.SetTenantId(tenantId);

	imtbase::CComplexCollectionFilter::FilterExpression filterExpr;
	filterExpr.logicalOperation = imtbase::IComplexCollectionFilter::LO_AND;
	filterExpr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("RoleId", QString::fromUtf8(roleId));
	filterExpr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("ProductId", QString::fromUtf8(productId));
	if (!objectId.isEmpty()){
		filterExpr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter(
					"DocumentId", QString::fromUtf8(objectId), imtbase::IComplexCollectionFilter::FO_NOT_EQUAL);
	}

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(filterExpr);

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("TenantFilter", &tenantFilter);
	filterParams.SetEditableParameter("ComplexFilter", &complexFilter);

	if (!roleCollection.GetElementIds(0, 1, &filterParams).isEmpty()){
		errorMessage = QStringLiteral("Role with ID: '%1' already exists").arg(roleId);
		return false;
	}

	return true;
}


bool CRoleDocumentValidatorComp::CheckParentRolesInTenant(
			const imtbase::IObjectCollection& roleCollection,
			const QByteArrayList& parentRoleIds,
			const QByteArray& tenantId,
			QString& errorMessage)
{
	if (parentRoleIds.isEmpty()){
		return true;
	}

	imtauth::CTenantFilterParam tenantFilter;
	tenantFilter.SetTenantId(tenantId);

	imtbase::CComplexCollectionFilter::FilterExpression filterExpr;
	filterExpr.logicalOperation = imtbase::IComplexCollectionFilter::LO_OR;
	for (const QByteArray& parentRoleId : parentRoleIds){
		filterExpr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("DocumentId", QString::fromUtf8(parentRoleId));
	}

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(filterExpr);

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("TenantFilter", &tenantFilter);
	filterParams.SetEditableParameter("ComplexFilter", &complexFilter);

	const imtbase::ICollectionInfo::Ids tenantRoleIds = roleCollection.GetElementIds(0, -1, &filterParams);
	for (const QByteArray& parentRoleId : parentRoleIds){
		if (!tenantRoleIds.contains(parentRoleId)){
			errorMessage = QStringLiteral("The parent role '%1' belongs to another organization").arg(QString::fromUtf8(parentRoleId));
			return false;
		}
	}

	return true;
}


} // namespace imtauth
