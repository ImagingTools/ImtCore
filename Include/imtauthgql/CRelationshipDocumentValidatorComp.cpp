// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "imtauthgql/CRelationshipDocumentValidatorComp.h"


// ImtCore includes
#include <imtauth/ITenantRelationshipInfo.h>


namespace imtauthgql
{


bool CRelationshipDocumentValidatorComp::ValidateDocumentData(
			const QByteArray& /*objectId*/,
			const istd::IChangeable& document,
			QString& errorMessage) const
{
	const imtauth::ITenantRelationshipInfo* relInfo = dynamic_cast<const imtauth::ITenantRelationshipInfo*>(&document);
	if (relInfo == nullptr){
		errorMessage = QStringLiteral("Invalid document type for relationship validation");
		return false;
	}

	if (relInfo->GetSourceTenantId().isEmpty()){
		errorMessage = QStringLiteral("Source tenant must be specified");
		return false;
	}

	if (relInfo->GetTargetTenantId().isEmpty()){
		errorMessage = QStringLiteral("Target tenant (partner organization) must be specified");
		return false;
	}

	if (relInfo->GetSourceTenantId() == relInfo->GetTargetTenantId()){
		errorMessage = QStringLiteral("Source and target tenants must be different organizations");
		return false;
	}

	return true;
}


} // namespace imtauthgql
