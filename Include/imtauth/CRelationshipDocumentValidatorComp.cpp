// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CRelationshipDocumentValidatorComp.h>


// ImtCore includes
#include <imtauth/ITenantRelationshipInfo.h>


namespace imtauth
{


bool CRelationshipDocumentValidatorComp::ValidateDocumentData(
			const QByteArray& objectId,
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

	// Check for duplicate relationship: same tenant pair with same role combination
	// (including symmetric/mirror matches) is not allowed.
	if (m_relationshipCollectionCompPtr.IsValid()){
		const QByteArray sourceTenantId = relInfo->GetSourceTenantId();
		const QByteArray targetTenantId = relInfo->GetTargetTenantId();
		const int srcRole = static_cast<int>(relInfo->GetSourceRole());
		const int tgtRole = static_cast<int>(relInfo->GetTargetRole());

		for (const QByteArray& existingId : m_relationshipCollectionCompPtr->GetElementIds()){
			// Skip the document being edited (update case)
			if (existingId == objectId){
				continue;
			}

			imtbase::IObjectCollection::DataPtr existingDataPtr;
			if (!m_relationshipCollectionCompPtr->GetObjectData(existingId, existingDataPtr)){
				continue;
			}

			const imtauth::ITenantRelationshipInfo* existingPtr =
					dynamic_cast<const imtauth::ITenantRelationshipInfo*>(existingDataPtr.GetPtr());
			if (existingPtr == nullptr){
				continue;
			}

			const int existingSrcRole = static_cast<int>(existingPtr->GetSourceRole());
			const int existingTgtRole = static_cast<int>(existingPtr->GetTargetRole());

			// Direct match: same source→target with same roles
			const bool isDirectMatch =
					existingPtr->GetSourceTenantId() == sourceTenantId
					&& existingPtr->GetTargetTenantId() == targetTenantId
					&& existingSrcRole == srcRole
					&& existingTgtRole == tgtRole;

			// Mirror match: swapped tenants with swapped roles
			const bool isMirrorMatch =
					existingPtr->GetSourceTenantId() == targetTenantId
					&& existingPtr->GetTargetTenantId() == sourceTenantId
					&& existingSrcRole == tgtRole
					&& existingTgtRole == srcRole;

			if (isDirectMatch || isMirrorMatch){
				errorMessage = QStringLiteral("A relationship with these roles already exists between the specified tenants");
				return false;
			}
		}
	}

	return true;
}


} // namespace imtauth
