// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CRoleDocumentValidatorComp.h>


// ImtCore includes
#include <imtauth/IRole.h>


namespace imtauth
{


bool CRoleDocumentValidatorComp::ValidateDocumentData(
			const QByteArray& objectId,
			const istd::IChangeable& document,
			QString& errorMessage,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
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

	imtbase::ICollectionInfo::Ids collectionIds = m_roleCollectionCompPtr->GetElementIds();
	for (const imtbase::ICollectionInfo::Id& collectionId : collectionIds){
		if (collectionId == objectId){
			continue;
		}

		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_roleCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
			const imtauth::IRole* currentRoleInfoPtr = dynamic_cast<const imtauth::IRole*>(dataPtr.GetPtr());
			if (currentRoleInfoPtr != nullptr){
				if (currentRoleInfoPtr->GetRoleId() == roleId && currentRoleInfoPtr->GetProductId() == productId){
					errorMessage = QString("Role with ID: '%1' already exists").arg(qPrintable(roleId));
					return false;
				}
			}
		}
	}

	return true;
}


} // namespace imtauth
