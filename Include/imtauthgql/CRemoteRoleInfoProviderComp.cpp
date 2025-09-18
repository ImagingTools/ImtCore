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
		typedef rolessdl::CRoleData Response;

		QString errorMessage;
		Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			return nullptr;
		}

		if (!response.Version_1_0){
			return nullptr;
		}

		imtauth::IRoleUniquePtr roleInfoPtr = m_roleInfoFactCompPtr.CreateInstance();
		if (!roleInfoPtr.IsValid()){
			return nullptr;
		}

		roleInfoPtr->SetDefault(bool(response.Version_1_0->isDefault && *response.Version_1_0->isDefault));
		roleInfoPtr->SetGuest(bool(response.Version_1_0->isGuest) && *response.Version_1_0->isGuest);
		if (response.Version_1_0->permissions){
			roleInfoPtr->SetLocalPermissions(response.Version_1_0->permissions->split(';'));
		}
		if (response.Version_1_0->productId){
			roleInfoPtr->SetProductId(*response.Version_1_0->productId);
		}
		if (response.Version_1_0->description){
			roleInfoPtr->SetRoleDescription(*response.Version_1_0->description);
		}
		if (response.Version_1_0->roleId){
			roleInfoPtr->SetRoleId(*response.Version_1_0->roleId);
		}
		if (response.Version_1_0->name){
			roleInfoPtr->SetRoleName(*response.Version_1_0->name);
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


