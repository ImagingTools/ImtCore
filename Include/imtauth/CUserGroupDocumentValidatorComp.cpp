// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CUserGroupDocumentValidatorComp.h>


// ImtCore includes
#include <imtauth/IUserGroupInfo.h>


namespace imtauth
{


bool CUserGroupDocumentValidatorComp::ValidateDocumentData(
			const QByteArray& objectId,
			const istd::IChangeable& document,
			QString& errorMessage,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
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

	imtbase::ICollectionInfo::Ids collectionIds = m_userGroupCollectionCompPtr->GetElementIds();
	for (const imtbase::ICollectionInfo::Id& collectionId : collectionIds){
		if (collectionId == objectId){
			continue;
		}

		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_userGroupCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
			const imtauth::IUserGroupInfo* currentGroupInfoPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
			if (currentGroupInfoPtr != nullptr){
				if (currentGroupInfoPtr->GetName() == name){
					errorMessage = QStringLiteral("Group Name '%1' already exists").arg(name);
					return false;
				}
			}
		}
	}

	return true;
}


} // namespace imtauth
