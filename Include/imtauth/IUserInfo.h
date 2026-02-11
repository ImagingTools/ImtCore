// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IUserGroupInfo.h>
#include <imtauth/IUserBaseInfo.h>


namespace imtauth
{


/**
	Interface for describing an user.
	\ingroup User
*/
class IUserInfo: virtual public IUserBaseInfo
{
public:
	enum MetaInfoTypes
	{
		MIT_EMAIL = MIT_ROLES + 1,
		MIT_GROUPS,
		MIT_SYSTEM_ID,
		MIT_SYSTEM_NAME,
		MIT_PASSWORD_HASH,
	};

	struct SystemInfo
	{
		QByteArray systemId;
		QString systemName;
		bool enabled = true;

		bool operator == (const SystemInfo& other) const
		{
			return (systemId == other.systemId) && (systemName == other.systemName) && (enabled == other.enabled);
		}

		bool operator != (const SystemInfo& other) const
		{
			return !(*this == other);
		}
	};

	typedef QList<SystemInfo> SystemInfoList;

	/**
		Get password hash of the user.
	*/
	virtual QByteArray GetPasswordHash() const = 0;

	/**
		Set password hash of the user.
	*/
	virtual void SetPasswordHash(const QByteArray& passwordHash) = 0;

	/**
		Get e-mail of the user.
	*/
	virtual QString GetMail() const = 0;

	/**
		Set e-mail of the user.
	*/
	virtual void SetMail(const QString& mail) = 0;

	/**
		Check admin user.
	*/
	virtual bool IsAdmin() const = 0;

	/**
		Get groups.
	*/
	virtual IUserGroupInfo::GroupIds GetGroups() const = 0;

	/**
		Add this user to group.
	*/
	virtual void AddToGroup(const QByteArray& groupId) = 0;

	/**
		Remove this user from group.
	*/
	virtual bool RemoveFromGroup(const QByteArray& groupId) = 0;

	/**
		Get information about the system in which this user is located.
	*/
	virtual SystemInfoList GetSystemInfos() const = 0;

	/**
		Add user to the system.
	*/
	virtual bool AddToSystem(SystemInfo systemInfo) = 0;

	/**
		Remove an user from the system.
	*/
	virtual bool RemoveFromSystem(const QByteArray& systemId) = 0;
};


typedef istd::TUniqueInterfacePtr<IUserInfo> IUserInfoUniquePtr;
typedef istd::TSharedInterfacePtr<IUserInfo> IUserInfoSharedPtr;


} // namespace imtauth


