// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CCrossOrgGrantDocumentValidatorComp.h>


// ImtCore includes
#include <imtauth/ICrossOrgGrantData.h>
#include <imtauth/imtauth.h>


namespace imtauth
{


bool CCrossOrgGrantDocumentValidatorComp::ValidateDocumentData(
			const QByteArray& /*objectId*/,
			const istd::IChangeable& document,
			QString& errorMessage,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	const imtauth::ICrossOrgGrantData* grantData = dynamic_cast<const imtauth::ICrossOrgGrantData*>(&document);
	if (grantData == nullptr){
		errorMessage = QStringLiteral("Invalid document type for grant validation");
		return false;
	}

	imtauth::CrossOrgGrantInfo info = grantData->GetGrantInfo();

	if (info.targetTenantId.isEmpty()){
		errorMessage = QStringLiteral("Target organization must be specified");
		return false;
	}

	if (info.roleIds.isEmpty()){
		errorMessage = QStringLiteral("At least one role must be selected");
		return false;
	}

	if (m_connectionManagerCompPtr.IsValid() && !imtauth::AreTenantsConnected(*m_connectionManagerCompPtr, info.sourceTenantId, info.targetTenantId)){
		errorMessage = QStringLiteral("The organizations are not connected");
		return false;
	}

	return true;
}


} // namespace imtauth
