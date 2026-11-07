// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CProductServerRequestPolicyComp.h>


namespace imtservergql
{


// public methods

// reimplemented (IProductServerRequestPolicy)

bool CProductServerRequestPolicyComp::FindRequiredPermission(const imtgql::CGqlRequest& gqlRequest, QByteArray& permission) const
{
	permission.clear();

	const QByteArray commandId = gqlRequest.GetCommandId();
	if (m_openCommandIdsAttrPtr.FindValue(commandId) >= 0){
		return true;
	}

	const int commandIndex = m_commandIdsAttrPtr.FindValue(commandId);
	if (commandIndex >= 0){
		if (commandIndex >= m_commandPermissionsAttrPtr.GetCount()){
			return false;
		}

		permission = m_commandPermissionsAttrPtr[commandIndex];

		return true;
	}

	const imtgql::CGqlParamObject* inputPtr = gqlRequest.GetParamObject("input");
	const QByteArray collectionId = inputPtr != nullptr ? inputPtr->GetParamArgumentValue("collectionId").toByteArray() : QByteArray();
	const int collectionIndex = collectionId.isEmpty() ? -1 : m_collectionIdsAttrPtr.FindValue(collectionId);
	if (collectionIndex < 0){
		return false;
	}

	// Reading one object has its own permission where the collection names one, the view permission otherwise
	const bool isObjectReading = m_objectReadCommandIdsAttrPtr.FindValue(commandId) >= 0;
	if (isObjectReading && collectionIndex < m_collectionObjectReadPermissionsAttrPtr.GetCount()){
		permission = m_collectionObjectReadPermissionsAttrPtr[collectionIndex];
		if (!permission.isEmpty()){
			return true;
		}
	}

	const bool isReading = isObjectReading
				|| gqlRequest.GetRequestType() != imtgql::IGqlRequest::RT_MUTATION
				|| m_viewCommandIdsAttrPtr.FindValue(commandId) >= 0;
	const auto& permissions = isReading ? m_collectionViewPermissionsAttrPtr : m_collectionEditPermissionsAttrPtr;
	if (collectionIndex >= permissions.GetCount()){
		return false;
	}

	permission = permissions[collectionIndex];

	return !permission.isEmpty();
}


} // namespace imtservergql
