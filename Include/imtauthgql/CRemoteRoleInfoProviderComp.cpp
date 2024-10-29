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

	namespace rolessdl = sdl::imtauth::Roles::V1_0;

	rolessdl::RoleItemRequestArguments arguments;
	arguments.input.SetId(roleId);
	arguments.input.SetProductId(productId);

	imtgql::CGqlRequest gqlRequest;
	if (rolessdl::CRoleItemGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		rolessdl::CRoleDataPayload response;
		// TODO: remove !!!
		imtgql::CGqlObject itemObject;
		itemObject.InsertField("id");
		gqlRequest.AddField("item", itemObject);
		if (!SendModelRequest(gqlRequest, response)){
			return nullptr;
		}

		rolessdl::CRoleData roleData = response.GetRoleData();

		istd::TDelPtr<imtauth::IRole> roleInfoPtr;
		roleInfoPtr.SetPtr(m_roleInfoFactCompPtr.CreateInstance());
		if (!roleInfoPtr.IsValid()){
			return nullptr;
		}

		roleInfoPtr->SetDefault(roleData.GetIsDefault());
		roleInfoPtr->SetGuest(roleData.GetIsGuest());
		roleInfoPtr->SetLocalPermissions(roleData.GetPermissions().split(';'));
		roleInfoPtr->SetProductId(roleData.GetProductId());
		roleInfoPtr->SetRoleDescription(roleData.GetDescription());
		roleInfoPtr->SetRoleId(roleData.GetRoleId());
		roleInfoPtr->SetRoleName(roleData.GetName());

		return roleInfoPtr.PopPtr();
	}

	return nullptr;
}


const imtauth::IRole* CRemoteRoleInfoProviderComp::GetRole(const QByteArray& objectId) const
{
	return GetRole(objectId, "");
}


} // namespace imtauthgql


