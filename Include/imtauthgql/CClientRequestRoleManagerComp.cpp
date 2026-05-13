// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CClientRequestRoleManagerComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtauthgql
{


// public methods

// reimplemented (imtauth::IRoleManager)

QByteArrayList CClientRequestRoleManagerComp::GetRoleIds() const
{
	return GetElementIds(QByteArrayLiteral("Roles"));
}


imtauth::IRoleUniquePtr CClientRequestRoleManagerComp::GetRole(const QByteArray& roleId) const
{
	if (!m_roleFactoryCompPtr.IsValid()){
		return nullptr;
	}

	sdl::imtauth::Roles::CRoleData::V1_0 roleData;
	bool ok = GetRoleDataSdl(roleId, roleData);
	if (!ok){
		return nullptr;
	}

	imtauth::IRoleUniquePtr roleInfoPtr = m_roleFactoryCompPtr.CreateInstance();
	if (!roleInfoPtr.IsValid()){
		return nullptr;
	}

	QByteArray productId;
	if (roleData.productId){
		productId = *roleData.productId;
	}

	if (productId.isEmpty()){
		return nullptr;
	}

	roleInfoPtr->SetProductId(productId);

	if (roleData.name){
		roleInfoPtr->SetRoleName(*roleData.name);
	}

	if (roleData.roleId){
		roleInfoPtr->SetRoleId(*roleData.roleId);
	}

	if (roleData.description){
		roleInfoPtr->SetRoleDescription(*roleData.description);
	}

	if (roleData.permissions){
		QByteArray permissions = *roleData.permissions;
		if (!permissions.isEmpty()){
			roleInfoPtr->SetLocalPermissions(permissions.split(';'));
		}
	}

	return roleInfoPtr;
}


QByteArray CClientRequestRoleManagerComp::CreateRole(
			const QByteArray& productId,
			const QString& roleName,
			const QString& roleDescription,
			const QByteArrayList& permissions)
{
	namespace rolessdl = sdl::imtauth::Roles;

	rolessdl::RoleAddRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	arguments.input.Version_1_0->typeId = QByteArrayLiteral("Role");
	arguments.input.Version_1_0->productId = productId;
	arguments.input.Version_1_0->name = roleName;
	arguments.input.Version_1_0->description = roleDescription;

	rolessdl::CRoleData::V1_0 roleData;

	QString roleId = roleName;
	roleId.replace(QRegularExpression("\\s+"), "");
	roleData.roleId = roleId.toUtf8();
	roleData.productId = productId;

	roleData.name = roleName;
	roleData.description = roleDescription;

	if (!permissions.isEmpty()){
		roleData.permissions = permissions.join(';');
	}

	arguments.input.Version_1_0->item = roleData;

	sdl::imtbase::ImtCollection::CAddedNotificationPayload payload;
	bool ok = SendModelRequestInternal<rolessdl::RoleAddRequestArguments, sdl::imtbase::ImtCollection::CAddedNotificationPayload, rolessdl::CRoleAddGqlRequest>(arguments, payload);
	if (!ok){
		return QByteArray();
	}

	if (!payload.Version_1_0->id.HasValue()){
		return QByteArray();
	}

	return *payload.Version_1_0->id;
}


bool CClientRequestRoleManagerComp::RemoveRole(const QByteArray& roleId)
{
	return RemoveElements(QByteArrayLiteral("Roles"), {roleId});
}


QByteArrayList CClientRequestRoleManagerComp::GetRolePermissions(const QByteArray& roleId) const
{
	imtauth::IRoleUniquePtr roleInfoPtr = GetRole(roleId);
	if (!roleInfoPtr.IsValid()){
		return QByteArrayList();
	}

	return roleInfoPtr->GetPermissions();
}


bool CClientRequestRoleManagerComp::AddPermissionsToRole(const QByteArray& roleId, const QByteArrayList& permissions)
{
	if (permissions.isEmpty()){
		return false;
	}

	sdl::imtauth::Roles::CRoleData::V1_0 roleData;
	bool ok = GetRoleDataSdl(roleId, roleData);
	if (!ok){
		return false;
	}

	if (!roleData.permissions.HasValue()){
		return false;
	}

	QByteArrayList currentPermissions = roleData.permissions->split(';');

	QByteArrayList result = currentPermissions;
	for (const QByteArray& permissionId : permissions){
		if (!result.contains(permissionId)){
			result << permissionId;
		}
	}

	roleData.permissions = result.join(';');

	return SetRoleDataSdl(roleId, roleData);
}


bool CClientRequestRoleManagerComp::RemovePermissionsFromRole(const QByteArray& roleId, const QByteArrayList& permissions)
{
	if (permissions.isEmpty()){
		return false;
	}

	sdl::imtauth::Roles::CRoleData::V1_0 roleData;
	bool ok = GetRoleDataSdl(roleId, roleData);
	if (!ok){
		return false;
	}

	if (!roleData.permissions.HasValue()){
		return false;
	}

	QByteArrayList currentPermissions = roleData.permissions->split(';');

	QByteArrayList result = currentPermissions;
	for (const QByteArray& permissionId : permissions){
		if (result.contains(permissionId)){
			result.removeAll(permissionId);
		}
	}

	roleData.permissions = result.join(';');

	return SetRoleDataSdl(roleId, roleData);
}


// private methods

bool CClientRequestRoleManagerComp::GetRoleDataSdl(const QByteArray& roleId, sdl::imtauth::Roles::CRoleData::V1_0& roleData) const
{
	if (!m_applicationInfoCompPtr.IsValid()){
		return false;
	}

	namespace rolessdl = sdl::imtauth::Roles;

	rolessdl::RoleItemRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = roleId;
	arguments.input.Version_1_0->productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();

	rolessdl::CRoleData payload;
	bool ok = SendModelRequestInternal<rolessdl::RoleItemRequestArguments, rolessdl::CRoleData, rolessdl::CRoleItemGqlRequest>(arguments, payload);
	if (!ok){
		return false;
	}

	roleData = *payload.Version_1_0;

	return true;
}


bool CClientRequestRoleManagerComp::SetRoleDataSdl(const QByteArray& roleId, const sdl::imtauth::Roles::CRoleData::V1_0& roleData) const
{
	if (!m_applicationInfoCompPtr.IsValid()){
		return false;
	}

	namespace rolessdl = sdl::imtauth::Roles;

	rolessdl::RoleUpdateRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = roleId;
	arguments.input.Version_1_0->typeId = QByteArray("Role");
	arguments.input.Version_1_0->productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
	arguments.input.Version_1_0->item = roleData;

	rolessdl::CRoleData payload;
	bool ok = SendModelRequestInternal<rolessdl::RoleUpdateRequestArguments, rolessdl::CRoleData, rolessdl::CRoleUpdateGqlRequest>(arguments, payload);
	if (!ok){
		return false;
	}

	if (!payload.Version_1_0->id){
		return false;
	}

	return !payload.Version_1_0->id->isEmpty();
}


} // namespace imtauthgql


