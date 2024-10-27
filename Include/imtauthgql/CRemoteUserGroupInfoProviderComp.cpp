#include <imtauthgql/CRemoteUserGroupInfoProviderComp.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <iser/CJsonMemReadArchive.h>

// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtgql/CGqlRequest.h>
#include <imtbase/CTreeItemModel.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Groups.h>


namespace imtauthgql
{


// protected methods

// reimplemented (IUserGroupInfoProvider)

const imtbase::ICollectionInfo& CRemoteUserGroupInfoProviderComp::GetUserGroupList() const
{
	static imtbase::CCollectionInfo emptyCollection;
	return emptyCollection;
}


const imtauth::IUserGroupInfo* CRemoteUserGroupInfoProviderComp::GetUserGroup(const QByteArray& groupId) const
{
	if (!m_userGroupInfoFactCompPtr.IsValid()){
		return nullptr;
	}

	namespace groupssdl = sdl::imtauth::Groups::V1_0;

	groupssdl::GroupItemRequestArguments arguments;
	arguments.input.SetId(groupId);

	imtgql::CGqlRequest gqlRequest;
	if (groupssdl::CGroupItemGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		groupssdl::CGroupDataPayload response;

		if (!SendModelRequest(gqlRequest, response)){
			return nullptr;
		}

		groupssdl::CGroupData groupData = response.GetGroupData();

		istd::TDelPtr<imtauth::IUserGroupInfo> groupInfoPtr;
		groupInfoPtr.SetPtr(m_userGroupInfoFactCompPtr.CreateInstance());
		if (!groupInfoPtr.IsValid()){
			return nullptr;
		}

		QJsonObject object;
		if (!groupData.WriteToJsonObject(object)){
			return nullptr;
		}

		QJsonDocument document(object);
		if (document.isNull()){
			return nullptr;
		}

		iser::CJsonMemReadArchive archive(document.toJson());
		if (!groupInfoPtr->Serialize(archive)){
			return nullptr;
		}

		return groupInfoPtr.PopPtr();
	}

	return nullptr;
}


} // namespace imtauthgql


