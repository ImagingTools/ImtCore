// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "imtauthgql/CCrossOrgGrantDocumentValidatorComp.h"


// ImtCore includes
#include <imtauth/ICrossOrgGrantData.h>


namespace imtauthgql
{


bool CCrossOrgGrantDocumentValidatorComp::ValidateDocumentData(
			const QByteArray& /*objectId*/,
			const istd::IChangeable& document,
			QString& errorMessage) const
{
	const imtauth::ICrossOrgGrantData* grantData = dynamic_cast<const imtauth::ICrossOrgGrantData*>(&document);
	if (grantData == nullptr){
		errorMessage = QStringLiteral("Invalid document type for grant validation");
		return false;
	}

	imtauth::CrossOrgGrantInfo info = grantData->GetGrantInfo();

	if (info.targetTenantId.isEmpty()){
		errorMessage = QStringLiteral("Target tenant must be specified");
		return false;
	}

	if (info.roleIds.isEmpty()){
		errorMessage = QStringLiteral("At least one role must be selected");
		return false;
	}

	return true;
}


} // namespace imtauthgql
