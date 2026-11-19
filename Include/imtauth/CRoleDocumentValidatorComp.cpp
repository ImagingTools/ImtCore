// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CRoleDocumentValidatorComp.h>


// Qt includes
#include <QtCore/QSet>

// ACF includes
#include <iprm/CParamsSet.h>
#include <iqt/iqt.h>

// ImtCore includes
#include <imtauth/CTenantFilterParam.h>
#include <imtauth/IRole.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtbase/IOperationContext.h>
#include <imtdoc/imtdoc.h>


namespace imtauth
{


static QString Translate(const iqt::ITranslationManager* translationManagerPtr, const char* text, const QByteArray& languageId)
{
	// Without a language of the caller the text stays English, not the language of the server
	if (languageId.isEmpty()){
		return QString::fromUtf8(text);
	}

	return QString::fromUtf8(iqt::GetTranslation(translationManagerPtr, text, languageId, "CRoleDocumentValidatorComp"));
}


bool CRoleDocumentValidatorComp::ValidateDocumentData(
			const istd::IChangeable& document,
			QString& errorMessage,
			const iprm::IParamsSet* validationParams,
			const imtbase::IOperationContext* operationContextPtr) const
{
	const QByteArray objectId = imtdoc::GetObjectIdFromParams(validationParams);
	const QByteArray languageId = imtdoc::GetLanguageIdFromParams(validationParams);
	const iqt::ITranslationManager* translationManagerPtr = m_translationManagerCompPtr.GetPtr();

	const imtauth::IRole* roleInfoPtr = dynamic_cast<const imtauth::IRole*>(&document);
	if (roleInfoPtr == nullptr){
		errorMessage = QStringLiteral("Invalid document type for role validation");
		return false;
	}

	const QByteArray roleId = roleInfoPtr->GetRoleId();
	if (roleId.isEmpty()){
		errorMessage = Translate(translationManagerPtr, QT_TRANSLATE_NOOP("CRoleDocumentValidatorComp", "Role-ID can't be empty!"), languageId);
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

	if (!CheckRoleIdUnique(*m_roleCollectionCompPtr, objectId, roleId, productId, tenantId, errorMessage, translationManagerPtr, languageId)){
		return false;
	}

	if (!CheckParentRolesInTenant(*m_roleCollectionCompPtr, roleInfoPtr->GetIncludedRoles(), tenantId, errorMessage, translationManagerPtr, languageId)){
		return false;
	}

	if (!CheckParentRolesInProduct(*m_roleCollectionCompPtr, roleInfoPtr->GetIncludedRoles(), productId, errorMessage, translationManagerPtr, languageId)){
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
				errorMessage = Translate(translationManagerPtr, QT_TRANSLATE_NOOP("CRoleDocumentValidatorComp", "Permission '%1' is not allowed for tenant '%2'"), languageId)
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
			QString& errorMessage,
			const iqt::ITranslationManager* translationManagerPtr,
			const QByteArray& languageId)
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
		errorMessage = Translate(translationManagerPtr, QT_TRANSLATE_NOOP("CRoleDocumentValidatorComp", "Role with ID: '%1' already exists"), languageId).arg(QString::fromUtf8(roleId));
		return false;
	}

	return true;
}


bool CRoleDocumentValidatorComp::CheckParentRolesInTenant(
			const imtbase::IObjectCollection& roleCollection,
			const QByteArrayList& parentRoleIds,
			const QByteArray& tenantId,
			QString& errorMessage,
			const iqt::ITranslationManager* translationManagerPtr,
			const QByteArray& languageId)
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
			errorMessage = Translate(translationManagerPtr, QT_TRANSLATE_NOOP("CRoleDocumentValidatorComp", "The parent role '%1' belongs to another organization"), languageId).arg(QString::fromUtf8(parentRoleId));
			return false;
		}
	}

	return true;
}


bool CRoleDocumentValidatorComp::CheckParentRolesInProduct(
			const imtbase::IObjectCollection& roleCollection,
			const QByteArrayList& parentRoleIds,
			const QByteArray& productId,
			QString& errorMessage,
			const iqt::ITranslationManager* translationManagerPtr,
			const QByteArray& languageId)
{
	for (const QByteArray& parentRoleId : parentRoleIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		const imtauth::IRole* parentRolePtr = roleCollection.GetObjectData(parentRoleId, dataPtr) ? dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr()) : nullptr;
		if (parentRolePtr == nullptr || parentRolePtr->GetProductId() != productId){
			errorMessage = Translate(translationManagerPtr, QT_TRANSLATE_NOOP("CRoleDocumentValidatorComp", "The parent role '%1' belongs to another product"), languageId).arg(QString::fromUtf8(parentRoleId));
			return false;
		}
	}

	return true;
}


} // namespace imtauth
