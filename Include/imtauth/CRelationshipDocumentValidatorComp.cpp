// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CRelationshipDocumentValidatorComp.h>


// ImtCore includes
#include <imtauth/CollectionSelection.h>
#include <imtauth/ITenantRelationshipInfo.h>
#include <imtdoc/imtdoc.h>


namespace imtauth
{


bool CRelationshipDocumentValidatorComp::ValidateDocumentData(
			const QUrl& url,
			const istd::IChangeable& document,
			QString& errorMessage,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	const QByteArray objectId = imtdoc::UrlToObjectId(url);

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

	// The end of an archived relationship is final: a later write would move it and join its period with a later one.
	// A stored relationship has an object id; one that cannot be read may be archived, so it is not written.
	if (m_relationshipCollectionCompPtr.IsValid() && !objectId.isEmpty()){
		imtbase::IObjectCollection::DataPtr storedDataPtr;
		const imtauth::ITenantRelationshipInfo* storedPtr = m_relationshipCollectionCompPtr->GetObjectData(objectId, storedDataPtr)
					? dynamic_cast<const imtauth::ITenantRelationshipInfo*>(storedDataPtr.GetPtr())
					: nullptr;
		if (storedPtr == nullptr){
			errorMessage = QStringLiteral("The relationship could not be read. Try again.");
			return false;
		}
		if (storedPtr->GetStatus() == imtauth::ITenantRelationshipInfo::TRS_ARCHIVED){
			errorMessage = QStringLiteral("An ended relationship cannot be changed");
			return false;
		}
	}

	// Check for duplicate relationship: same tenant pair with same role combination
	// (including symmetric/mirror matches) is not allowed.
	if (m_relationshipCollectionCompPtr.IsValid()){
		const QByteArray sourceTenantId = relInfo->GetSourceTenantId();
		const QByteArray targetTenantId = relInfo->GetTargetTenantId();
		const int srcRole = static_cast<int>(relInfo->GetSourceRole());
		const int tgtRole = static_cast<int>(relInfo->GetTargetRole());

		// Only the relationships of the same pair can be duplicates
		const CollectionSelection::FilterExpression pairExpression =
					CollectionSelection::PairInEitherOrder("SourceTenantId", "TargetTenantId", sourceTenantId, targetTenantId);
		for (const QByteArray& existingId : CollectionSelection::SelectIds(*m_relationshipCollectionCompPtr, pairExpression)){
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
