#pragma once


// ImtCore includes
#include <imtauth/IRoleInfoProvider.h>
#include <imtgql/TClientRequestManagerCompWrap.h>


namespace imtauthgql
{


class CRemoteRoleInfoProviderComp:
			public imtgql::CClientRequestManagerCompBase,
			virtual public imtauth::IRoleInfoProvider
{
public:
	typedef imtgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CRemoteRoleInfoProviderComp)
		I_REGISTER_INTERFACE(imtauth::IRoleInfoProvider);
		I_ASSIGN(m_roleInfoFactCompPtr, "RoleFactory", "Factory used for creation of the new role", true, "RoleFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (IRoleInfoProvider)
	virtual const imtbase::ICollectionInfo& GetRoleList() const override;
	virtual const imtauth::IRole* GetRole(const QByteArray& roleId, const QByteArray& productId) const override;
	virtual const imtauth::IRole* GetRole(const QByteArray& objectId) const override;

protected:
	I_FACT(imtauth::IRole, m_roleInfoFactCompPtr);
};


} // namespace imtauthgql


