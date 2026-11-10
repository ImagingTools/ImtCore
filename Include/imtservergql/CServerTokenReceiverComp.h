// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlic/IProductInfo.h>
#include <imtserverapp/ICommandPermissionsProvider.h>
#include <imtservergql/IServerTokenReceiver.h>


namespace imtservergql
{


/**
	The receiver of the servers of products. What they may send is the command permissions of the policy:
	a command of a collection is declared as "Collection/Command", any other command by its name.
	A declared command without permissions is open to any connected server.
*/
class CServerTokenReceiverComp:
			public icomp::CComponentBase,
			virtual public IServerTokenReceiver
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CServerTokenReceiverComp);
		I_REGISTER_INTERFACE(IServerTokenReceiver);
		I_ASSIGN(m_permissionTreeCompPtr, "PermissionTree", "Product of this server", true, "PermissionTree");
		I_ASSIGN(m_tokenManagerCompPtr, "TokenManager", "Where the tokens issued for other servers are kept", true, "TokenManager");
		I_ASSIGN(m_policyCompPtr, "Policy", "Commands the servers of products may send, with the permissions of a token each needs", false, "Policy");
	I_END_COMPONENT;

	// reimplemented (IServerTokenReceiver)
	virtual QByteArray GetServerProductId() const override;
	virtual imtauth::IPersonalAccessTokenManager* GetTokenManager() const override;
	virtual bool FindRequiredPermissions(const imtgql::CGqlRequest& gqlRequest, QByteArrayList& permissions) const override;

private:
	I_REF(imtlic::IProductInfo, m_permissionTreeCompPtr);
	I_REF(imtauth::IPersonalAccessTokenManager, m_tokenManagerCompPtr);
	I_REF(imtserverapp::ICommandPermissionsProvider, m_policyCompPtr);
};


} // namespace imtservergql
