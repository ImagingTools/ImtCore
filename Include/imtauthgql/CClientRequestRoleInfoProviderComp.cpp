// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CClientRequestRoleInfoProviderComp.h>


// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtgql/CGqlRequestContextManager.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Roles.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtauth::IRoleInfoProvider)

const imtbase::ICollectionInfo& CClientRequestRoleInfoProviderComp::GetRoleList() const
{
	static imtbase::CCollectionInfo emptyCollection;

	return emptyCollection;
}


imtauth::IRoleUniquePtr CClientRequestRoleInfoProviderComp::GetRole(const QByteArray& objectId, const iprm::IParamsSet* /*paramsPtr*/) const
{
	if (!m_applicationInfoCompPtr.IsValid()){
		return nullptr;
	}

	namespace rolessdl = sdl::imtauth::Roles;

	rolessdl::RoleItemRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = objectId;

	arguments.input.Version_1_0->productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();

	QString errorMessage;
	imtgql::CGqlRequest gqlRequest;

	imtgql::IGqlContext* gqlContextPtr = imtgql::CGqlRequestContextManager::GetContext();
	if (gqlContextPtr != nullptr){
		gqlRequest.SetGqlContext(dynamic_cast<imtgql::IGqlContext*>(gqlContextPtr->CloneMe().PopInterfacePtr()));
	}

	if (!rolessdl::CRoleItemGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return nullptr;
	}

	rolessdl::CRoleData payload = SendModelRequest<rolessdl::CRoleData>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return nullptr;
	}

	if (!payload.Version_1_0.HasValue()){
		return nullptr;
	}

	imtauth::IRoleUniquePtr roleInfoPtr = m_roleFactoryCompPtr.CreateInstance();
	if (!roleInfoPtr.IsValid()){
		return nullptr;
	}

	if (payload.Version_1_0->roleId){
		roleInfoPtr->SetRoleId(*payload.Version_1_0->roleId);
	}

	if (payload.Version_1_0->productId){
		roleInfoPtr->SetProductId(*payload.Version_1_0->productId);
	}

	if (payload.Version_1_0->name){
		roleInfoPtr->SetRoleName(*payload.Version_1_0->name);
	}

	if (payload.Version_1_0->description){
		roleInfoPtr->SetRoleDescription(*payload.Version_1_0->description);
	}

	if (payload.Version_1_0->isDefault){
		roleInfoPtr->SetDefault(*payload.Version_1_0->isDefault);
	}

	if (payload.Version_1_0->isGuest){
		roleInfoPtr->SetGuest(*payload.Version_1_0->isGuest);
	}

	if (payload.Version_1_0->permissions){
		QByteArray permissions = *payload.Version_1_0->permissions;
		if (!permissions.isEmpty()){
			roleInfoPtr->SetLocalPermissions(permissions.split(';'));
		}
	}

	if (payload.Version_1_0->parentRoles){
		for (const QByteArray& roleId : payload.Version_1_0->parentRoles->ToList()){
			roleInfoPtr->IncludeRole(roleId);
		}
	}

	return roleInfoPtr.PopInterfacePtr();
}


} // namespace imtauthgql


