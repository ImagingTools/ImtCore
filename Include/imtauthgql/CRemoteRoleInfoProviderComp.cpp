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


const imtauth::IRole* CRemoteRoleInfoProviderComp::GetRole(const QByteArray& /*roleId*/, const QByteArray& /*productId*/) const
{
	return nullptr;
}


const imtauth::IRole* CRemoteRoleInfoProviderComp::GetRole(const QByteArray& objectId) const
{
	if (!m_roleInfoFactCompPtr.IsValid()){
		return nullptr;
	}

	namespace rolessdl = sdl::imtauth::Roles::V1_0;

	rolessdl::RoleItemRequestArguments arguments;
	arguments.input.SetId(objectId);

	imtgql::CGqlRequest gqlRequest;
	if (rolessdl::CRoleItemGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		rolessdl::CRoleDataPayload response;
		if (!SendModelRequest(gqlRequest, response)){
			return nullptr;
		}

		rolessdl::CRoleData roleData = response.GetRoleData();

		QJsonObject object;
		if (!roleData.WriteToJsonObject(object)){
			return nullptr;
		}

		QJsonDocument document(object);
		if (document.isNull()){
			return nullptr;
		}

		istd::TDelPtr<imtauth::IRole> roleInfoPtr;
		roleInfoPtr.SetPtr(m_roleInfoFactCompPtr.CreateInstance());
		if (!roleInfoPtr.IsValid()){
			return nullptr;
		}

		iser::CJsonMemReadArchive archive(document.toJson());
		if (!roleInfoPtr->Serialize(archive)){
			return nullptr;
		}

		return roleInfoPtr.PopPtr();
	}

	return nullptr;
}


} // namespace imtauthgql


