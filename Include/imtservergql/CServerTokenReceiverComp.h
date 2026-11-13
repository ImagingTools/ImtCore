// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iprm/IEnableableParam.h>

// ImtCore includes
#include <imtlic/IProductInfo.h>
#include <imtservergql/IServerTokenReceiver.h>


namespace imtservergql
{


/**
	The receiver of the servers of products: the product of this server and the tokens it issued for them.
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
		I_ASSIGN(m_enabledParamCompPtr, "EnabledParam", "Whether the servers of other products need their tokens; without it they always do", false, "EnabledParam");
	I_END_COMPONENT;

	// reimplemented (IServerTokenReceiver)
	virtual QByteArray GetServerProductId() const override;
	virtual imtauth::IPersonalAccessTokenManager* GetTokenManager() const override;
	virtual bool IsEnabled() const override;

private:
	I_REF(imtlic::IProductInfo, m_permissionTreeCompPtr);
	I_REF(imtauth::IPersonalAccessTokenManager, m_tokenManagerCompPtr);
	I_REF(iprm::IEnableableParam, m_enabledParamCompPtr);
};


} // namespace imtservergql
