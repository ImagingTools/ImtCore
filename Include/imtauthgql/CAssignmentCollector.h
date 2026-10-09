// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArrayList>
#include <QtCore/QList>
#include <QtCore/QString>

// ACF includes
#include <istd/TNullableValue.h>

// ImtCore includes
#include <imtauth/IRole.h>
#include <imtauth/IRoleInfoProvider.h>
#include <imtauth/IUserGroupInfo.h>
#include <imtauth/IUserGroupInfoProvider.h>
#include <imtauth/IUserInfo.h>
#include <imtsdl/TElementList.h>

// Generated includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Assignments.h>


namespace imtauthgql
{


/**
	Roles, groups, users and permissions linked to a user, group or role: the direct assignments
	and the inherited ones together with every path they are inherited through.
*/
class CAssignmentCollector
{
public:
	typedef imtsdl::TElementList<sdl::V1_0::imtauth::CAssignment> Assignments;

	enum StepKind
	{
		SK_GROUP,
		SK_PARENT_GROUP,
		SK_ROLE,
		SK_PARENT_ROLE,
		SK_DELEGATED_ROLE
	};

	struct Step
	{
		StepKind kind;
		QByteArray id;
		QString name;

		bool operator==(const Step& other) const
		{
			return (kind == other.kind) && (id == other.id);
		}
	};

	typedef QList<Step> Path;

	struct Item
	{
		QByteArray id;
		QString name;
		bool direct = false;
		QList<Path> sources;
	};

	struct Warning
	{
		QByteArray code;
		QByteArrayList ids;
		QString message;
	};

	/**
		Collect a user: the given roles (those not in \c directRoleIds are delegated) and the user's groups.
	*/
	void CollectUser(
				const imtauth::IUserInfo& userInfo,
				const imtauth::IUserBaseInfo::RoleIds& roleIds,
				const imtauth::IUserBaseInfo::RoleIds& directRoleIds,
				const QByteArray& productId);

	/**
		Collect a group: its members (only if \c collectUsers is set), its roles and the roles of its parent groups.
	*/
	void CollectGroup(const imtauth::IUserGroupInfo& group, const QByteArray& groupId, const QByteArray& productId, bool collectUsers = true);

	/**
		Collect a role: its own permissions and those of its parent roles.
	*/
	void CollectRole(const imtauth::IRole& role, const QByteArray& roleId, const imtauth::IRoleInfoProvider* roleProviderPtr);

	/**
		Fill roles, groups, permissions and accessWarnings of a user representation.
	*/
	template<class Representation>
	void FillUser(Representation& representation) const;

	/**
		Fill roles, users, parentGroups, permissions and accessWarnings of a group representation.
	*/
	template<class Representation>
	void FillGroup(Representation& representation) const;

	/**
		Fill parentRoles, permissions and accessWarnings of a role representation.
	*/
	template<class Representation>
	void FillRole(Representation& representation) const;

	/**
		Ids of the direct assignments, the only ones stored on update.
	*/
	static QByteArrayList GetDirectIds(const istd::TNullableValue<Assignments>& assignments);

	/**
		Direct assignments for the given ids, e.g. to send an update.
	*/
	static Assignments CreateDirectAssignments(const QByteArrayList& ids);

private:
	static Assignments CreateAssignmentList(const QList<Item>& items);
	static sdl::V1_0::imtauth::AssignmentSourceKind GetSdlKind(StepKind kind);
	imtsdl::TElementList<sdl::V1_0::imtauth::CAccessWarning> CreateAccessWarningList() const;

	static void AddItem(QList<Item>& items, const QByteArray& id, const QString& name, const Path& path);
	void AddWarning(const QByteArray& code, const QByteArrayList& ids, const QString& message);
	void AddPermissions(const QByteArrayList& permissionIds, const Path& path);
	void AddRole(
				const imtauth::IRoleInfoProvider* roleProviderPtr,
				const QByteArray& roleId,
				StepKind kind,
				const Path& path,
				QByteArrayList chain);
	void AddGroup(
				const imtauth::IUserGroupInfoProvider* groupProviderPtr,
				const QByteArray& groupId,
				StepKind kind,
				const QByteArray& productId,
				const Path& path,
				QByteArrayList chain);
	void AddGroupContents(
				const imtauth::IUserGroupInfo& group,
				const QByteArray& productId,
				const Path& groupPath,
				const QByteArrayList& chain,
				const imtauth::IUserGroupInfoProvider* fallbackGroupProviderPtr);

private:
	QList<Item> m_roles;
	QList<Item> m_groups;
	QList<Item> m_users;
	QList<Item> m_permissions;
	QList<Warning> m_warnings;
};


// public template methods

template<class Representation>
void CAssignmentCollector::FillUser(Representation& representation) const
{
	representation.roles = CreateAssignmentList(m_roles);
	representation.groups = CreateAssignmentList(m_groups);
	representation.permissions = CreateAssignmentList(m_permissions);
	representation.accessWarnings = CreateAccessWarningList();
}


template<class Representation>
void CAssignmentCollector::FillGroup(Representation& representation) const
{
	representation.roles = CreateAssignmentList(m_roles);
	representation.users = CreateAssignmentList(m_users);
	representation.parentGroups = CreateAssignmentList(m_groups);
	representation.permissions = CreateAssignmentList(m_permissions);
	representation.accessWarnings = CreateAccessWarningList();
}


template<class Representation>
void CAssignmentCollector::FillRole(Representation& representation) const
{
	representation.parentRoles = CreateAssignmentList(m_roles);
	representation.permissions = CreateAssignmentList(m_permissions);
	representation.accessWarnings = CreateAccessWarningList();
}


} // namespace imtauthgql


