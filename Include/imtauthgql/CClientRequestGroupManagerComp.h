#pragma once


// ACF includes
#include <ibase/IApplicationInfo.h>

// ImtCore includes
#include <imtauth/IUserGroupManager.h>
#include <imtauth/IAccessTokenProvider.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Groups.h>


namespace imtauthgql
{


class CClientRequestGroupManagerComp:
			public imtclientgql::CClientRequestManagerCompBase,
			virtual public imtauth::IUserGroupManager
{
public:
	typedef imtclientgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CClientRequestGroupManagerComp)
		I_REGISTER_INTERFACE(imtauth::IUserGroupManager);
		I_ASSIGN(m_userGroupFactoryCompPtr, "UserGroupFactory", "User group factory", true, "UserGroupFactory");
		I_ASSIGN(m_accessTokenProviderCompPtr, "AccessTokenProvider", "Access token provider", false, "AccessTokenProvider");
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application info", true, "ApplicationInfo");
	I_END_COMPONENT;

	// reimplemented (imtauth::IUserGroupManager)
	virtual QByteArrayList GetGroupIds() const override;
	virtual QByteArray CreateGroup(const QString& groupName, const QString& description) override;
	virtual bool RemoveGroup(const QByteArray& groupId) override;
	virtual imtauth::IUserGroupInfoUniquePtr GetGroup(const QByteArray& groupId) const override;
	virtual bool AddUsersToGroup(const QByteArray& groupId, const QByteArrayList& userIds) override;
	virtual bool RemoveUsersFromGroup(const QByteArray& groupId, const QByteArrayList& userIds) override;
	virtual bool AddRolesToGroup(const QByteArray& groupId, const QByteArrayList& roleIds) override;
	virtual bool RemoveRolesFromGroup(const QByteArray& groupId, const QByteArrayList& roleIds) override;

private:
	bool GetGroupDataSdl(const QByteArray& groupId, sdl::imtauth::Groups::CGroupData::V1_0& groupData) const;
	bool SetGroupDataSdl(const QByteArray& groupId, const sdl::imtauth::Groups::CGroupData::V1_0& groupData) const;

	template<class Arguments, class Payload, class SdlRequest>
	bool SendModelRequestInternal(Arguments arguments, Payload& payload) const;

private:
	I_FACT(imtauth::IUserGroupInfo, m_userGroupFactoryCompPtr);
	I_REF(imtauth::IAccessTokenProvider, m_accessTokenProviderCompPtr);
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
};


} // namespace imtauthgql


