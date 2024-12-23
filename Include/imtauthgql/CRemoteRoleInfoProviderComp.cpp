#include <imtauthgql/CRemoteRoleInfoProviderComp.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <iser/CJsonMemReadArchive.h>

// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtgql/CGqlRequest.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Roles.h>


namespace imtauthgql
{


// protected methods

// reimplemented (IRoleInfoProvider)

const imtbase::ICollectionInfo& CRemoteRoleInfoProviderComp::GetRoleList() const
{
	static imtbase::CCollectionInfo emptyCollection;
	return emptyCollection;
}


const imtauth::IRole* CRemoteRoleInfoProviderComp::GetRole(const QByteArray& roleId, const QByteArray& productId) const
{
	if (!m_roleInfoFactCompPtr.IsValid()){
		return nullptr;
	}

	namespace rolessdl = sdl::imtauth::Roles;

	rolessdl::V1_0::RoleItemRequestArguments arguments;
	arguments.input.Id = QByteArray(roleId);
	arguments.input.ProductId = QByteArray(productId);

	imtgql::CGqlRequest gqlRequest;
	if (rolessdl::V1_0::CRoleItemGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		rolessdl::CRoleDataPayload::V1_0 response;
		if (!SendModelRequest<rolessdl::CRoleDataPayload::V1_0, rolessdl::CRoleDataPayload>(gqlRequest, response)){
			return nullptr;
		}

		rolessdl::CRoleData::V1_0 roleData;
		if (response.RoleData){
			roleData = *response.RoleData;
		}

		istd::TDelPtr<imtauth::IRole> roleInfoPtr;
		roleInfoPtr.SetPtr(m_roleInfoFactCompPtr.CreateInstance());
		if (!roleInfoPtr.IsValid()){
			return nullptr;
		}

		roleInfoPtr->SetDefault(bool(roleData.IsDefault && *roleData.IsDefault));
		roleInfoPtr->SetGuest(bool(roleData.IsGuest) && *roleData.IsGuest);
		if (roleData.Permissions){
			roleInfoPtr->SetLocalPermissions(roleData.Permissions->split(';'));
		}
		if (roleData.ProductId){
			roleInfoPtr->SetProductId(*roleData.ProductId);
		}
		if (roleData.Description){
			roleInfoPtr->SetRoleDescription(*roleData.Description);
		}
		if (roleData.RoleId){
			roleInfoPtr->SetRoleId(*roleData.RoleId);
		}
		if (roleData.Name){
			roleInfoPtr->SetRoleName(*roleData.Name);
		}

		return roleInfoPtr.PopPtr();
	}

	return nullptr;
}


const imtauth::IRole* CRemoteRoleInfoProviderComp::GetRole(const QByteArray& objectId) const
{
	return GetRole(objectId, "");
}


} // namespace imtauthgql


