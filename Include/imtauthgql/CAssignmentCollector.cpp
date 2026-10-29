// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CAssignmentCollector.h>


// ImtCore includes
#include <imtauth/imtauth.h>
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
	m_names.insert(groupId, group.GetName());

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
	m_names.insert(roleId, role.GetRoleName());

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


void CAssignmentCollector::AddRequiredPermissions(
			Assignments& permissions,
			const QHash<QByteArray, QByteArrayList>& requirements,
			const QHash<QByteArray, QString>& names,
			const QByteArrayList& availablePermissions)
{
	QList<Item> items = CreateItemList(permissions);

	QByteArrayList pendingIds;
	for (Item& item : items){
		item.name = names.value(item.id, item.name.isEmpty() ? QString::fromUtf8(item.id) : item.name);
		pendingIds << item.id;
	}

	// A permission whose paths grew is visited again, so its requirements get the new paths too.
	while (!pendingIds.isEmpty()){
		const QByteArray permissionId = pendingIds.takeFirst();

		QList<Path> permissionPaths;
		for (const Item& item : std::as_const(items)){
			if (item.id == permissionId){
				if (item.direct){
					permissionPaths << Path();
				}
				permissionPaths += item.sources;
				break;
			}
		}

		const Step requirementStep{SK_REQUIREMENT, permissionId, names.value(permissionId, QString::fromUtf8(permissionId))};

		for (const QByteArray& requiredId : requirements.value(permissionId)){
			if (requiredId == permissionId){
				continue;
			}

			if (!availablePermissions.isEmpty() && !imtauth::HasPermission(availablePermissions, requiredId, false)){
				continue;
			}

			qsizetype itemIndex = -1;
			for (qsizetype i = 0; i < items.size(); ++i){
				if (items[i].id == requiredId){
					itemIndex = i;
					break;
				}
			}

			if (itemIndex < 0){
				Item item;
				item.id = requiredId;
				item.name = names.value(requiredId, QString::fromUtf8(requiredId));
				items << item;
				itemIndex = items.size() - 1;
			}

			bool isGrown = false;
			for (const Path& path : std::as_const(permissionPaths)){
				// A chain that already passed through the required permission is a requirement cycle.
				if (path.contains(Step{SK_REQUIREMENT, requiredId, QString()})){
					continue;
				}

				const Path requiredPath = Path(path) << requirementStep;
				if (!items[itemIndex].sources.contains(requiredPath)){
					items[itemIndex].sources << requiredPath;
					isGrown = true;
				}
			}

			if (isGrown && !pendingIds.contains(requiredId)){
				pendingIds << requiredId;
			}
		}
	}

	permissions = CreateAssignmentList(items);
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


QList<CAssignmentCollector::Item> CAssignmentCollector::CreateItemList(const Assignments& assignments)
{
	QList<Item> retVal;
	for (const istd::TNullableValue<sdl::V1_0::imtauth::CAssignment>& assignment : assignments){
		if (!assignment.HasValue() || !assignment->id){
			continue;
		}

		Item item;
		item.id = *assignment->id;
		item.name = assignment->name ? *assignment->name : QString();
		item.direct = assignment->direct && *assignment->direct;

		if (assignment->sources){
			for (const istd::TNullableValue<sdl::V1_0::imtauth::CAssignmentPath>& sdlPath : *assignment->sources){
				if (!sdlPath.HasValue() || !sdlPath->steps){
					continue;
				}

				Path path;
				for (const istd::TNullableValue<sdl::V1_0::imtauth::CAssignmentStep>& sdlStep : *sdlPath->steps){
					if (sdlStep.HasValue() && sdlStep->kind && sdlStep->id){
						path << Step{GetStepKind(*sdlStep->kind), *sdlStep->id, sdlStep->name ? *sdlStep->name : QString()};
					}
				}

				if (!path.isEmpty()){
					item.sources << path;
				}
			}
		}

		retVal << item;
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
	case SK_REQUIREMENT:
		return sdl::V1_0::imtauth::AssignmentSourceKind::Requirement;
	case SK_ROLE:
	default:
		return sdl::V1_0::imtauth::AssignmentSourceKind::Role;
	}
}


CAssignmentCollector::StepKind CAssignmentCollector::GetStepKind(sdl::V1_0::imtauth::AssignmentSourceKind kind)
{
	switch (kind){
	case sdl::V1_0::imtauth::AssignmentSourceKind::Group:
		return SK_GROUP;
	case sdl::V1_0::imtauth::AssignmentSourceKind::ParentGroup:
		return SK_PARENT_GROUP;
	case sdl::V1_0::imtauth::AssignmentSourceKind::ParentRole:
		return SK_PARENT_ROLE;
	case sdl::V1_0::imtauth::AssignmentSourceKind::DelegatedRole:
		return SK_DELEGATED_ROLE;
	case sdl::V1_0::imtauth::AssignmentSourceKind::Requirement:
		return SK_REQUIREMENT;
	case sdl::V1_0::imtauth::AssignmentSourceKind::Role:
	default:
		return SK_ROLE;
	}
}


imtsdl::TElementList<sdl::V1_0::imtauth::CAccessWarning> CAssignmentCollector::CreateAccessWarningList() const
{
	imtsdl::TElementList<sdl::V1_0::imtauth::CAccessWarning> retVal;
	for (const Warning& warning : m_warnings){
		sdl::V1_0::imtauth::CAccessWarning sdlWarning;
		sdlWarning.code = QString::fromUtf8(warning.code);
		sdlWarning.ids.Emplace().FromList(warning.ids);

		QStringList names;
		for (const QByteArray& id : warning.ids){
			const QString name = m_names.value(id);
			names << (name.isEmpty() ? QString::fromUtf8(id) : name);
		}
		sdlWarning.message = names.isEmpty() ? warning.message : warning.message + QStringLiteral(": ") + names.join(QStringLiteral(" → "));
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
	m_names.insert(roleId, roleName);

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
	m_names.insert(groupId, groupName);

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


