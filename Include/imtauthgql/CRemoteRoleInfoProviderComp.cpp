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

	rolessdl::RoleItemRequestArguments arguments;
	arguments.input.Version_1_0->Id = QByteArray(roleId);
	arguments.input.Version_1_0->ProductId = QByteArray(productId);

	imtgql::CGqlRequest gqlRequest;
	if (rolessdl::CRoleItemGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		rolessdl::CRoleData::V1_0 response;
		if (!SendModelRequest<rolessdl::CRoleData::V1_0, rolessdl::CRoleData>(gqlRequest, response)){
			return nullptr;
		}

		istd::TDelPtr<imtauth::IRole> roleInfoPtr;
		roleInfoPtr.SetPtr(m_roleInfoFactCompPtr.CreateInstance());
		if (!roleInfoPtr.IsValid()){
			return nullptr;
		}

		roleInfoPtr->SetDefault(bool(response.IsDefault && *response.IsDefault));
		roleInfoPtr->SetGuest(bool(response.IsGuest) && *response.IsGuest);
		if (response.Permissions){
			roleInfoPtr->SetLocalPermissions(response.Permissions->split(';'));
		}
		if (response.ProductId){
			roleInfoPtr->SetProductId(*response.ProductId);
		}
		if (response.Description){
			roleInfoPtr->SetRoleDescription(*response.Description);
		}
		if (response.RoleId){
			roleInfoPtr->SetRoleId(*response.RoleId);
		}
		if (response.Name){
			roleInfoPtr->SetRoleName(*response.Name);
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


