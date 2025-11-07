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

	imtcollection::CGetElementIdsPayload payload;
	bool ok = SendModelRequestInternal<imtcollection::GetElementIdsRequestArguments, imtcollection::CGetElementIdsPayload, imtcollection::CGetElementIdsGqlRequest>(arguments, payload);
	if (!ok){
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
	arguments.input.Version_1_0->typeId = QByteArrayLiteral("Role");
	arguments.input.Version_1_0->productId = productId;
	arguments.input.Version_1_0->name = roleName;
	arguments.input.Version_1_0->description = roleDescription;

	rolessdl::CRoleData::V1_0 roleData;

	QString roleId = roleName;
	roleId.replace(QRegularExpression("\\s+"), "");
	roleData.roleId = roleId.toUtf8();

	roleData.name = roleName;
	roleData.description = roleDescription;
	roleData.permissions = permissions.join(';');

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
	namespace imtcollection = sdl::imtbase::ImtCollection;

	imtcollection::RemoveElementsRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->elementIds.Emplace();
	arguments.input.Version_1_0->elementIds->push_back(roleId);
	arguments.input.Version_1_0->collectionId = QByteArrayLiteral("Roles");

	imtcollection::CRemoveElementsPayload  payload;
	bool ok = SendModelRequestInternal<imtcollection::RemoveElementsRequestArguments, imtcollection::CRemoveElementsPayload , imtcollection::CRemoveElementsGqlRequest>(arguments, payload);
	if (!ok){
		return false;
	}

	if (!payload.Version_1_0->success.HasValue()){
		return false;
	}

	return *payload.Version_1_0->success;
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


template<class Arguments, class Payload, class SdlRequest>
bool CClientRequestRoleManagerComp::SendModelRequestInternal(Arguments arguments, Payload& payload) const
{
	imtgql::CGqlRequest gqlRequest;

	if (m_accessTokenProviderCompPtr.IsValid()){
		QByteArray accessToken = m_accessTokenProviderCompPtr->GetToken("");
		gqlRequest.SetHeader("x-authentication-token", accessToken);
	}

	if (!SdlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return false;
	}

	QString errorMessage;
	payload = SendModelRequest<Payload>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return false;
	}

	if (!payload.Version_1_0.HasValue()){
		return false;
	}

	return true;
}

} // namespace imtauthgql


