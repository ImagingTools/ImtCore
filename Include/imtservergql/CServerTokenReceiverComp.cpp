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


bool CServerTokenReceiverComp::IsEnabled() const
{
	return !m_enabledParamCompPtr.IsValid() || m_enabledParamCompPtr->IsEnabled();
}


} // namespace imtservergql
