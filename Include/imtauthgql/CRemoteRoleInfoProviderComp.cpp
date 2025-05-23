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
	arguments.input.Version_1_0.emplace();
	
	arguments.input.Version_1_0->id = QByteArray(roleId);
	arguments.input.Version_1_0->productId = QByteArray(productId);
	
	QString errorMessage;
	imtgql::CGqlRequest gqlRequest;
	if (rolessdl::CRoleItemGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		rolessdl::CRoleData::V1_0 response;
		if (!SendModelRequest<rolessdl::CRoleData::V1_0, rolessdl::CRoleData>(gqlRequest, response, errorMessage)){
			return nullptr;
		}

		imtauth::IRoleUniquePtr roleInfoPtr = m_roleInfoFactCompPtr.CreateInstance();
		if (!roleInfoPtr.IsValid()){
			return nullptr;
		}

		roleInfoPtr->SetDefault(bool(response.isDefault && *response.isDefault));
		roleInfoPtr->SetGuest(bool(response.isGuest) && *response.isGuest);
		if (response.permissions){
			roleInfoPtr->SetLocalPermissions(response.permissions->split(';'));
		}
		if (response.productId){
			roleInfoPtr->SetProductId(*response.productId);
		}
		if (response.description){
			roleInfoPtr->SetRoleDescription(*response.description);
		}
		if (response.roleId){
			roleInfoPtr->SetRoleId(*response.roleId);
		}
		if (response.name){
			roleInfoPtr->SetRoleName(*response.name);
		}

		return dynamic_cast<imtauth::IRole*>(roleInfoPtr.PopPtr());
	}

	return nullptr;
}


const imtauth::IRole* CRemoteRoleInfoProviderComp::GetRole(const QByteArray& objectId) const
{
	return GetRole(objectId, "");
}


} // namespace imtauthgql


