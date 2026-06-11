// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantDocumentValidatorComp.h>


// ACF includes
#include <iprm/CIdParam.h>
#include <iprm/CParamsSet.h>
#include <iprm/CTextParam.h>

// ImtCore includes
#include <imtauth/ITenantInfo.h>


namespace imtauth
{


bool CTenantDocumentValidatorComp::ValidateDocumentData(
			const QByteArray& objectId,
			const istd::IChangeable& document,
			QString& errorMessage,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	const imtauth::ITenantInfo* tenantInfo = dynamic_cast<const imtauth::ITenantInfo*>(&document);
	if (tenantInfo == nullptr){
		errorMessage = QStringLiteral("Invalid document type for tenant validation");
		return false;
	}

	const QString tenantName = tenantInfo->GetTenantName().trimmed();
	if (tenantName.isEmpty()){
		errorMessage = QStringLiteral("Tenant name must be specified");
		return false;
	}

	if (!m_tenantCollectionCompPtr.IsValid()){
		errorMessage = QStringLiteral("Tenant collection is not configured");
		return false;
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
		errorMessage = QStringLiteral("Tenant with the same name already exists");
		return false;
	}

	return true;
}


} // namespace imtauth
