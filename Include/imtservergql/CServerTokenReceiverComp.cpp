// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CServerTokenReceiverComp.h>


namespace imtservergql
{


// public methods

// reimplemented (IServerTokenReceiver)

QByteArray CServerTokenReceiverComp::GetServerProductId() const
{
	return m_permissionTreeCompPtr->GetProductId();
}


imtauth::IPersonalAccessTokenManager* CServerTokenReceiverComp::GetTokenManager() const
{
	return m_tokenManagerCompPtr.GetPtr();
}


bool CServerTokenReceiverComp::FindRequiredPermissions(const imtgql::CGqlRequest& gqlRequest, QByteArrayList& permissions) const
{
	permissions.clear();

	if (!m_policyCompPtr.IsValid()){
		return false;
	}

	// A request naming a collection is declared for that collection only
	const imtgql::CGqlParamObject* inputPtr = gqlRequest.GetParamObject("input");
	const QByteArray collectionId = inputPtr != nullptr ? inputPtr->GetParamArgumentValue("collectionId").toByteArray() : QByteArray();
	const QByteArray commandId = collectionId.isEmpty() ? gqlRequest.GetCommandId() : collectionId + '/' + gqlRequest.GetCommandId();
	if (!m_policyCompPtr->GetCommandIds().contains(commandId)){
		return false;
	}

	for (const QByteArray& permission : m_policyCompPtr->GetCommandPermissions(commandId)){
		if (!permission.isEmpty()){
			permissions.append(permission);
		}
	}

	return true;
}


} // namespace imtservergql
