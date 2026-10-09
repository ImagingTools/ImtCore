// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CAssignmentCollector.h>


// ImtCore includes
#include <imtauth/IUserInfoProvider.h>


namespace imtauthgql
{


// public methods

void CAssignmentCollector::CollectUser(
			const imtauth::IUserInfo& userInfo,
			const imtauth::IUserBaseInfo::RoleIds& roleIds,
			const imtauth::IUserBaseInfo::RoleIds& directRoleIds,
			const QByteArray& productId)
{
	for (const QByteArray& roleId : roleIds){
		const StepKind kind = directRoleIds.contains(roleId) ? SK_ROLE : SK_DELEGATED_ROLE;
		AddRole(userInfo.GetRoleProvider(), roleId, kind, Path(), QByteArrayList());
	}

	for (const QByteArray& groupId : userInfo.GetGroups()){
		AddGroup(userInfo.GetUserGroupProvider(), groupId, SK_GROUP, productId, Path(), QByteArrayList());
	}
}


void CAssignmentCollector::CollectGroup(const imtauth::IUserGroupInfo& group, const QByteArray& groupId, const QByteArray& productId, bool collectUsers)
{
	const imtauth::IUserInfoProvider* userProviderPtr = group.GetUserProvider();
	const QByteArrayList userIds = collectUsers ? group.GetUsers() : QByteArrayList();
	for (const QByteArray& userId : userIds){
		QString userName;
		if (userProviderPtr != nullptr){
			imtauth::IUserInfoUniquePtr userPtr = userProviderPtr->GetUser(userId);
			if (userPtr.IsValid()){
				userName = userPtr->GetName();
			}
		}

		AddItem(m_users, userId, userName.isEmpty() ? QString::fromUtf8(userId) : userName, Path());
	}

	AddGroupContents(group, productId, Path(), QByteArrayList() << groupId, nullptr);
}


void CAssignmentCollector::CollectRole(const imtauth::IRole& role, const QByteArray& roleId, const imtauth::IRoleInfoProvider* roleProviderPtr)
{
	AddPermissions(role.GetLocalPermissions(), Path());

	const imtauth::IRoleInfoProvider* parentProviderPtr = role.GetParentRolesProvider();
	if (parentProviderPtr == nullptr){
		parentProviderPtr = roleProviderPtr;
	}

	for (const QByteArray& parentRoleId : role.GetIncludedRoles()){
		AddRole(parentProviderPtr, parentRoleId, SK_PARENT_ROLE, Path(), QByteArrayList() << roleId);
	}
}


QByteArrayList CAssignmentCollector::GetDirectIds(const istd::TNullableValue<Assignments>& assignments)
{
	QByteArrayList retVal;
	if (!assignments.HasValue()){
		return retVal;
	}

	for (const istd::TNullableValue<sdl::V1_0::imtauth::CAssignment>& assignment : *assignments){
		if (assignment.HasValue() && assignment->id && assignment->direct && *assignment->direct){
			const QByteArray id = *assignment->id;
			if (!id.isEmpty() && !retVal.contains(id)){
				retVal << id;
			}
		}
	}

	return retVal;
}


CAssignmentCollector::Assignments CAssignmentCollector::CreateDirectAssignments(const QByteArrayList& ids)
{
	Assignments retVal;
	for (const QByteArray& id : ids){
		sdl::V1_0::imtauth::CAssignment assignment;
		assignment.id = id;
		assignment.direct = true;
		retVal << assignment;
	}

	return retVal;
}


// private methods

CAssignmentCollector::Assignments CAssignmentCollector::CreateAssignmentList(const QList<Item>& items)
{
	Assignments retVal;
	for (const Item& item : items){
		sdl::V1_0::imtauth::CAssignment assignment;
		assignment.id = item.id;
		assignment.name = item.name;
		assignment.direct = item.direct;

		imtsdl::TElementList<sdl::V1_0::imtauth::CAssignmentPath> sourceList;
		for (const Path& path : item.sources){
			imtsdl::TElementList<sdl::V1_0::imtauth::CAssignmentStep> stepList;
			for (const Step& step : path){
				sdl::V1_0::imtauth::CAssignmentStep sdlStep;
				sdlStep.kind = GetSdlKind(step.kind);
				sdlStep.id = step.id;
				sdlStep.name = step.name;
				stepList << sdlStep;
			}

			sdl::V1_0::imtauth::CAssignmentPath sdlPath;
			sdlPath.steps = std::move(stepList);
			sourceList << sdlPath;
		}

		assignment.sources = std::move(sourceList);
		retVal << assignment;
	}

	return retVal;
}


sdl::V1_0::imtauth::AssignmentSourceKind CAssignmentCollector::GetSdlKind(StepKind kind)
{
	switch (kind){
	case SK_GROUP:
		return sdl::V1_0::imtauth::AssignmentSourceKind::Group;
	case SK_PARENT_GROUP:
		return sdl::V1_0::imtauth::AssignmentSourceKind::ParentGroup;
	case SK_PARENT_ROLE:
		return sdl::V1_0::imtauth::AssignmentSourceKind::ParentRole;
	case SK_DELEGATED_ROLE:
		return sdl::V1_0::imtauth::AssignmentSourceKind::DelegatedRole;
	case SK_ROLE:
	default:
		return sdl::V1_0::imtauth::AssignmentSourceKind::Role;
	}
}


imtsdl::TElementList<sdl::V1_0::imtauth::CAccessWarning> CAssignmentCollector::CreateAccessWarningList() const
{
	imtsdl::TElementList<sdl::V1_0::imtauth::CAccessWarning> retVal;
	for (const Warning& warning : m_warnings){
		sdl::V1_0::imtauth::CAccessWarning sdlWarning;
		sdlWarning.code = QString::fromUtf8(warning.code);
		sdlWarning.ids.Emplace().FromList(warning.ids);
		sdlWarning.message = warning.message;
		retVal << sdlWarning;
	}

	return retVal;
}


void CAssignmentCollector::AddItem(QList<Item>& items, const QByteArray& id, const QString& name, const Path& path)
{
	Item* itemPtr = nullptr;
	for (Item& item : items){
		if (item.id == id){
			itemPtr = &item;
			break;
		}
	}

	if (itemPtr == nullptr){
		Item item;
		item.id = id;
		item.name = name;
		items << item;
		itemPtr = &items.last();
	}

	// An empty path is an assignment to the edited object itself.
	if (path.isEmpty()){
		itemPtr->direct = true;
	}
	else if (!itemPtr->sources.contains(path)){
		itemPtr->sources << path;
	}
}


void CAssignmentCollector::AddWarning(const QByteArray& code, const QByteArrayList& ids, const QString& message)
{
	for (const Warning& warning : std::as_const(m_warnings)){
		if ((warning.code == code) && (warning.ids == ids)){
			return;
		}
	}

	m_warnings << Warning{code, ids, message};
}


void CAssignmentCollector::AddPermissions(const QByteArrayList& permissionIds, const Path& path)
{
	for (const QByteArray& permissionId : permissionIds){
		if (!permissionId.isEmpty()){
			AddItem(m_permissions, permissionId, QString::fromUtf8(permissionId), path);
		}
	}
}


void CAssignmentCollector::AddRole(
			const imtauth::IRoleInfoProvider* roleProviderPtr,
			const QByteArray& roleId,
			StepKind kind,
			const Path& path,
			QByteArrayList chain)
{
	if (chain.contains(roleId)){
		chain << roleId;
		AddWarning(QByteArrayLiteral("CycleInParentRoles"), chain, QStringLiteral("Cycle in parent roles"));
		return;
	}

	imtauth::IRoleUniquePtr rolePtr = (roleProviderPtr != nullptr) ? roleProviderPtr->GetRole(roleId) : imtauth::IRoleUniquePtr();
	if (!rolePtr.IsValid()){
		AddWarning(QByteArrayLiteral("RoleNotFound"), QByteArrayList() << roleId, QStringLiteral("Role not found"));
		return;
	}

	QString roleName = rolePtr->GetRoleName();
	if (roleName.isEmpty()){
		roleName = QString::fromUtf8(roleId);
	}

	const Path rolePath = Path(path) << Step{kind, roleId, roleName};

	// A delegated role is not assigned to the user, so its source names the delegation itself.
	AddItem(m_roles, roleId, roleName, (kind == SK_DELEGATED_ROLE) ? rolePath : path);
	AddPermissions(rolePtr->GetLocalPermissions(), rolePath);

	const imtauth::IRoleInfoProvider* parentProviderPtr = rolePtr->GetParentRolesProvider();
	if (parentProviderPtr == nullptr){
		parentProviderPtr = roleProviderPtr;
	}

	chain << roleId;
	for (const QByteArray& parentRoleId : rolePtr->GetIncludedRoles()){
		AddRole(parentProviderPtr, parentRoleId, SK_PARENT_ROLE, rolePath, chain);
	}
}


void CAssignmentCollector::AddGroup(
			const imtauth::IUserGroupInfoProvider* groupProviderPtr,
			const QByteArray& groupId,
			StepKind kind,
			const QByteArray& productId,
			const Path& path,
			QByteArrayList chain)
{
	if (chain.contains(groupId)){
		chain << groupId;
		AddWarning(QByteArrayLiteral("CycleInParentGroups"), chain, QStringLiteral("Cycle in parent groups"));
		return;
	}

	imtauth::IUserGroupInfoSharedPtr groupPtr = (groupProviderPtr != nullptr) ? groupProviderPtr->GetUserGroup(groupId) : imtauth::IUserGroupInfoSharedPtr();
	if (!groupPtr.IsValid()){
		AddWarning(QByteArrayLiteral("GroupNotFound"), QByteArrayList() << groupId, QStringLiteral("Group not found"));
		return;
	}

	QString groupName = groupPtr->GetName();
	if (groupName.isEmpty()){
		groupName = QString::fromUtf8(groupId);
	}

	AddItem(m_groups, groupId, groupName, path);

	const Path groupPath = Path(path) << Step{kind, groupId, groupName};

	AddGroupContents(*groupPtr, productId, groupPath, chain << groupId, groupProviderPtr);
}


void CAssignmentCollector::AddGroupContents(
			const imtauth::IUserGroupInfo& group,
			const QByteArray& productId,
			const Path& groupPath,
			const QByteArrayList& chain,
			const imtauth::IUserGroupInfoProvider* fallbackGroupProviderPtr)
{
	QByteArrayList products;
	if (productId.isEmpty()){
		products = group.GetProducts();
	}
	else{
		products << productId;
	}

	for (const QByteArray& product : std::as_const(products)){
		for (const QByteArray& roleId : group.GetRoles(product)){
			AddRole(group.GetRoleProvider(), roleId, SK_ROLE, groupPath, QByteArrayList());
		}
	}

	const imtauth::IUserGroupInfoProvider* parentProviderPtr = group.GetUserGroupProvider();
	if (parentProviderPtr == nullptr){
		parentProviderPtr = fallbackGroupProviderPtr;
	}

	for (const QByteArray& parentGroupId : group.GetParentGroups()){
		AddGroup(parentProviderPtr, parentGroupId, SK_PARENT_GROUP, productId, groupPath, chain);
	}
}


} // namespace imtauthgql


