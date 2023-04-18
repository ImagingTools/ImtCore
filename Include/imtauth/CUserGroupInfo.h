#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/IUserGroupInfo.h>
#include <imtauth/CUserBaseInfo.h>


namespace imtauth
{


class CUserGroupInfo: virtual public IUserGroupInfo, public CUserBaseInfo
{
	typedef CUserBaseInfo BaseClass;

public:
	CUserGroupInfo();

	// reimplemented (IUserGroupInfo)
	virtual UserIds GetUsers() const override;
	virtual void SetUsers(const UserIds& users) override;
	virtual void AddUser(const QByteArray& userId) override;
	virtual bool RemoveUser(const QByteArray& userId) override;
	virtual GroupIds GetParentGroups() const override;
	virtual bool AddParentGroup(const QByteArray& parentGroupId) override;
	virtual bool RemoveParentGroup(const QByteArray& parentGroupId) override;
	virtual const imtauth::IUserInfoProvider* GetUserProvider() const override;

	// reimplemented (IUserBaseInfo)
	virtual RoleIds GetRoles() const override;
	virtual FeatureIds GetPermissions() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable &object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeable *CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QByteArrayList GetParentGroups(const QByteArray& groupId) const;
	void GetParentGroupList(const IUserGroupInfo& userGroupInfo, QByteArrayList& groupList) const;

protected:
	IUserGroupInfo::UserIds m_userIds;
	IUserGroupInfo::GroupIds m_parentGroupIds;
	imtauth::IUserInfoProvider* m_userInfoProviderPtr;
};


typedef imtbase::TIdentifiableWrap<CUserGroupInfo> CIdentifiableUserGroupInfo;


} // namespace imtauth



