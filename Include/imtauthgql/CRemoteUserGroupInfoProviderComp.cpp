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


imtauth::IUserGroupInfoSharedPtr CRemoteUserGroupInfoProviderComp::GetUserGroup(const QByteArray& groupId) const
{
	if (!m_userGroupInfoFactCompPtr.IsValid()){
		return nullptr;
	}

	namespace groupssdl = sdl::imtauth::Groups;

	groupssdl::GroupItemRequestArguments arguments;
	arguments.input.Version_1_0.emplace();
	arguments.input.Version_1_0->id = QByteArray(groupId);

	imtgql::CGqlRequest gqlRequest;
	if (groupssdl::CGroupItemGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		typedef groupssdl::CGroupData Response;

		QString errorMessage;
		Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			return nullptr;
		}

		if (!response.Version_1_0){
			return nullptr;
		}

		imtauth::IUserGroupInfoSharedPtr groupInfoPtr = m_userGroupInfoFactCompPtr.CreateInstance();
		if (!groupInfoPtr.IsValid()){
			return nullptr;
		}

		QJsonObject object;
		if (!response.Version_1_0->WriteToJsonObject(object)){
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

		return groupInfoPtr;
	}

	return nullptr;
}


} // namespace imtauthgql


