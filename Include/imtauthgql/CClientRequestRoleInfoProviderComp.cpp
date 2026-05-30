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

	namespace rolessdl = sdl::V1_0::imtauth;

	rolessdl::RoleItemRequestArguments arguments;
	arguments.input.id = objectId;

	arguments.input.productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();

	QString errorMessage;
	imtgql::CGqlRequest gqlRequest;

	imtgql::IGqlContext* gqlContextPtr = imtgql::CGqlRequestContextManager::GetContext();
	if (gqlContextPtr != nullptr){
		istd::IChangeableUniquePtr clonedPtr = gqlContextPtr->CloneMe();
		imtgql::IGqlContextUniquePtr castedPtr;
		castedPtr.MoveCastedPtr(std::move(clonedPtr));
		gqlRequest.SetGqlContext(imtgql::IGqlContextSharedPtr::CreateFromUnique(castedPtr));
	}

	if (!rolessdl::CRoleItemGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return nullptr;
	}

	rolessdl::CRoleData payload = SendModelRequest<rolessdl::CRoleData>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return nullptr;
	}

	imtauth::IRoleUniquePtr roleInfoPtr = m_roleFactoryCompPtr.CreateInstance();
	if (!roleInfoPtr.IsValid()){
		return nullptr;
	}

	if (payload.roleId){
		roleInfoPtr->SetRoleId(*payload.roleId);
	}

	if (payload.productId){
		roleInfoPtr->SetProductId(*payload.productId);
	}

	if (payload.name){
		roleInfoPtr->SetRoleName(*payload.name);
	}

	if (payload.description){
		roleInfoPtr->SetRoleDescription(*payload.description);
	}

	if (payload.isDefault){
		roleInfoPtr->SetDefault(*payload.isDefault);
	}

	if (payload.isGuest){
		roleInfoPtr->SetGuest(*payload.isGuest);
	}

	if (payload.permissions){
		QByteArray permissions = *payload.permissions;
		if (!permissions.isEmpty()){
			roleInfoPtr->SetLocalPermissions(permissions.split(';'));
		}
	}

	if (payload.parentRoles){
		for (const QByteArray& roleId : payload.parentRoles->ToList()){
			roleInfoPtr->IncludeRole(roleId);
		}
	}

	return roleInfoPtr;
}


} // namespace imtauthgql


