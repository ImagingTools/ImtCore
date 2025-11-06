#include <imtauthgql/CClientRequestRoleManagerComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtauthgql
{


// public methods

// reimplemented (imtauth::IRoleManager)

QByteArrayList CClientRequestRoleManagerComp::GetRoleIds() const
{
	namespace imtcollection = sdl::imtbase::ImtCollection;

	imtcollection::GetElementIdsRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->collectionId = QByteArrayLiteral("Roles");

	imtgql::CGqlRequest gqlRequest;
	if (!imtcollection::CGetElementIdsGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return QByteArrayList();
	}

	QString errorMessage;
	imtcollection::CGetElementIdsPayload payload = SendModelRequest<imtcollection::CGetElementIdsPayload>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return QByteArrayList();
	}

	if (!payload.Version_1_0.HasValue()){
		return QByteArrayList();
	}

	if (!payload.Version_1_0->elementIds.HasValue()){
		return QByteArrayList();
	}

	return payload.Version_1_0->elementIds->ToList();
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
		QByteArrayList permissions = roleData.permissions->split(';');
		roleInfoPtr->SetLocalPermissions(permissions);
	}

	return roleInfoPtr.PopInterfacePtr();
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
	arguments.input.Version_1_0->productId = productId;

	rolessdl::CRoleData::V1_0 roleData;

	QString roleId = roleName;
	roleId.replace(QRegularExpression("\\s+"), "");
	roleData.roleId = roleId.toUtf8();

	roleData.name = roleName;
	roleData.description = roleDescription;
	roleData.permissions = permissions.join(';');

	arguments.input.Version_1_0->item = roleData;

	imtgql::CGqlRequest gqlRequest;
	if (!rolessdl::CRoleAddGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return QByteArray();
	}

	QString errorMessage;
	sdl::imtbase::ImtCollection::CAddedNotificationPayload payload = SendModelRequest<sdl::imtbase::ImtCollection::CAddedNotificationPayload>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return QByteArray();
	}

	if (!payload.Version_1_0.HasValue()){
		return QByteArray();
	}

	if (!payload.Version_1_0->id.HasValue()){
		return QByteArray();
	}

	return *payload.Version_1_0->id;
}


bool CClientRequestRoleManagerComp::RemoveRole(const QByteArray& roleId)
{
	namespace imtcollection = sdl::imtbase::ImtCollection;

	imtcollection::RemoveElementsRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->elementIds.Emplace();
	arguments.input.Version_1_0->elementIds->push_back(roleId);
	arguments.input.Version_1_0->collectionId = QByteArrayLiteral("Roles");

	imtgql::CGqlRequest gqlRequest;
	if (!imtcollection::CRemoveElementsGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	QString errorMessage;
	imtcollection::CRemoveElementsPayload response = SendModelRequest<imtcollection::CRemoveElementsPayload>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (!response.Version_1_0.HasValue()){
		return false;
	}

	if (!response.Version_1_0->success.HasValue()){
		return false;
	}

	return *response.Version_1_0->success;
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

	imtgql::CGqlRequest gqlRequest;
	if (!rolessdl::CRoleItemGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	QString errorMessage;
	rolessdl::CRoleData response = SendModelRequest<rolessdl::CRoleData>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (!response.Version_1_0.HasValue()){
		return false;
	}

	roleData = *response.Version_1_0;

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
	arguments.input.Version_1_0->productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
	arguments.input.Version_1_0->item = roleData;

	imtgql::CGqlRequest gqlRequest;
	if (!rolessdl::CRoleUpdateGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	QString errorMessage;
	sdl::imtbase::ImtCollection::CUpdatedNotificationPayload response = SendModelRequest<sdl::imtbase::ImtCollection::CUpdatedNotificationPayload>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (!response.Version_1_0.HasValue()){
		return false;
	}

	if (!response.Version_1_0->id){
		return false;
	}

	return !response.Version_1_0->id->isEmpty();
}


} // namespace imtauthgql


