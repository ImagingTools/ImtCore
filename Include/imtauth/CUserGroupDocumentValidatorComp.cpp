// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserGroupDocumentValidatorComp.h>


// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtauth/CTenantFilterParam.h>
#include <imtauth/IUserGroupInfo.h>
#include <imtbase/CComplexCollectionFilter.h>
#include <imtbase/IOperationContext.h>


namespace imtauth
{


bool CUserGroupDocumentValidatorComp::ValidateDocumentData(
			const QByteArray& objectId,
			const istd::IChangeable& document,
			QString& errorMessage,
			const imtbase::IOperationContext* operationContextPtr) const
{
	const imtauth::IUserGroupInfo* userGroupInfoPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(&document);
	if (userGroupInfoPtr == nullptr){
		errorMessage = QStringLiteral("Invalid document type for user group validation");
		return false;
	}

	const QString name = userGroupInfoPtr->GetName();
	if (name.isEmpty()){
		errorMessage = QStringLiteral("Group name cannot be empty");
		return false;
	}

	if (!m_userGroupCollectionCompPtr.IsValid()){
		errorMessage = QStringLiteral("User group collection is not configured");
		return false;
	}

	QByteArray tenantId;
	if (operationContextPtr != nullptr){
		tenantId = operationContextPtr->GetTenantId().trimmed();
	}

	if (!CheckGroupNameUnique(*m_userGroupCollectionCompPtr, objectId, name, tenantId, errorMessage)){
		return false;
	}

	return CheckParentGroupsInTenant(*m_userGroupCollectionCompPtr, userGroupInfoPtr->GetParentGroups(), tenantId, errorMessage);
}


bool CUserGroupDocumentValidatorComp::CheckGroupNameUnique(
			const imtbase::IObjectCollection& userGroupCollection,
			const QByteArray& objectId,
			const QString& name,
			const QByteArray& tenantId,
			QString& errorMessage)
{
	imtauth::CTenantFilterParam tenantFilter;
	tenantFilter.SetTenantId(tenantId);

	imtbase::CComplexCollectionFilter::FilterExpression filterExpr;
	filterExpr.logicalOperation = imtbase::IComplexCollectionFilter::LO_AND;
	filterExpr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("Name", name);
	if (!objectId.isEmpty()){
		filterExpr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter(
					"DocumentId", QString::fromUtf8(objectId), imtbase::IComplexCollectionFilter::FO_NOT_EQUAL);
	}

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(filterExpr);

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("TenantFilter", &tenantFilter);
	filterParams.SetEditableParameter("ComplexFilter", &complexFilter);

	if (!userGroupCollection.GetElementIds(0, 1, &filterParams).isEmpty()){
		errorMessage = QStringLiteral("Group Name '%1' already exists").arg(name);
		return false;
	}

	return true;
}


bool CUserGroupDocumentValidatorComp::CheckParentGroupsInTenant(
			const imtbase::IObjectCollection& userGroupCollection,
			const QByteArrayList& parentGroupIds,
			const QByteArray& tenantId,
			QString& errorMessage)
{
	if (parentGroupIds.isEmpty()){
		return true;
	}

	imtauth::CTenantFilterParam tenantFilter;
	tenantFilter.SetTenantId(tenantId);

	imtbase::CComplexCollectionFilter::FilterExpression filterExpr;
	filterExpr.logicalOperation = imtbase::IComplexCollectionFilter::LO_OR;
	for (const QByteArray& parentGroupId : parentGroupIds){
		filterExpr.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter("DocumentId", QString::fromUtf8(parentGroupId));
	}

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(filterExpr);

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("TenantFilter", &tenantFilter);
	filterParams.SetEditableParameter("ComplexFilter", &complexFilter);

	const imtbase::ICollectionInfo::Ids tenantGroupIds = userGroupCollection.GetElementIds(0, -1, &filterParams);
	for (const QByteArray& parentGroupId : parentGroupIds){
		if (!tenantGroupIds.contains(parentGroupId)){
			errorMessage = QStringLiteral("The parent group '%1' belongs to another organization").arg(QString::fromUtf8(parentGroupId));
			return false;
		}
	}

	return true;
}


} // namespace imtauth
