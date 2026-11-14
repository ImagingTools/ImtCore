// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QRecursiveMutex>

// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/IUserGroupInfo.h>
#include <imtauth/CPrincipalBase.h>


namespace imtauth
{


class CUserGroupInfo: virtual public IUserGroupInfo, public CPrincipalBase
{
	typedef CPrincipalBase BaseClass;

public:
	CUserGroupInfo();

	/**
		Get the users assigned to this group directly, without the users of the parent groups.
	*/
	UserIds GetOwnUsers() const;

	/**
		Lock serializing the read-modify-write updates of stored user groups,
		e.g. between the LDAP group synchronization and the GraphQL group mutations.
	*/
	static QRecursiveMutex& GetUpdateMutex();

	// reimplemented (IUserGroupInfo)
	virtual UserIds GetUsers() const override;
	virtual void SetUsers(const UserIds& users) override;
	virtual void AddUser(const QByteArray& userId) override;
	virtual bool RemoveUser(const QByteArray& userId) override;
	virtual GroupIds GetParentGroups() const override;
	virtual bool AddParentGroup(const QByteArray& parentGroupId) override;
	virtual bool RemoveParentGroup(const QByteArray& parentGroupId) override;
	virtual const imtauth::IUserInfoProvider* GetUserProvider() const override;
	virtual SystemInfoList GetSystemInfos() const override;
	virtual bool AddToSystem(SystemInfo systemInfo) override;
	virtual bool RemoveFromSystem(const QByteArray& systemId) override;

	// reimplemented (IPrincipal)
	virtual FeatureIds GetPermissions(const QByteArray& productId = QByteArray()) const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable &object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	// The given groups and all their ancestors, each once.
	QByteArrayList GetAncestorGroups(const QByteArrayList& groupIds) const;

protected:
	IUserGroupInfo::UserIds m_userIds;
	IUserGroupInfo::GroupIds m_parentGroupIds;
	IUserGroupInfo::SystemInfoList m_systemInfos;
	imtauth::IUserInfoProvider* m_userInfoProviderPtr;
};


typedef imtbase::TIdentifiableWrap<CUserGroupInfo> CIdentifiableUserGroupInfo;


} // namespace imtauth



