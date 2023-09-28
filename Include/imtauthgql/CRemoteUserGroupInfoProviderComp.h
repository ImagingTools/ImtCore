#pragma once


// ImtCore includes
#include <imtauth/IUserGroupInfoProvider.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>


namespace imtauthgql
{


class CRemoteUserGroupInfoProviderComp:
			public imtclientgql::CClientRequestManagerCompBase,
			virtual public imtauth::IUserGroupInfoProvider
{
public:
	typedef imtclientgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CRemoteUserGroupInfoProviderComp)
		I_REGISTER_INTERFACE(imtauth::IUserGroupInfoProvider);
		I_ASSIGN(m_userGroupInfoFactCompPtr, "GroupFactory", "Factory used for creation of the new group", true, "GroupFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (IUserGroupInfoProvider)
	virtual const imtbase::ICollectionInfo& GetUserGroupList() const override;
	virtual const imtauth::IUserGroupInfo* GetUserGroup(const QByteArray& groupId) const override;

protected:
	I_FACT(imtauth::IUserGroupInfo, m_userGroupInfoFactCompPtr);
};


} // namespace imtauthgql


