// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CClientRequestGroupInfoProviderComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtbase/CCollectionInfo.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequestContextManager.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Groups.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtauth::IUserGroupInfoProvider)

const imtbase::ICollectionInfo& CClientRequestGroupInfoProviderComp::GetUserGroupList() const
{
	static imtbase::CCollectionInfo emptyCollection;

	return emptyCollection;
}


imtauth::IUserGroupInfoSharedPtr CClientRequestGroupInfoProviderComp::GetUserGroup(const QByteArray& groupId, const iprm::IParamsSet* /*paramsPtr*/) const
{
	if (!m_userGroupFactoryCompPtr.IsValid()){
		return nullptr;
	}

	namespace userssdl = sdl::imtauth::Groups;

	userssdl::GroupItemRequestArguments arguments;
	arguments.input.Version_1_0.Emplace();
	arguments.input.Version_1_0->id = groupId;

	QByteArray productId;
	if (m_applicationInfoCompPtr.IsValid()){
		productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
	}

	arguments.input.Version_1_0->productId = productId;

	imtgql::CGqlRequest gqlRequest;
	if (!userssdl::CGroupItemGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return nullptr;
	}

	imtgql::IGqlContext* gqlContextPtr = imtgql::CGqlRequestContextManager::GetContext();
	if (gqlContextPtr != nullptr){
		gqlRequest.SetGqlContext(dynamic_cast<imtgql::IGqlContext*>(gqlContextPtr->CloneMe().PopInterfacePtr()));
	}

	QString errorMessage;
	userssdl::CGroupData payload = SendModelRequest<userssdl::CGroupData>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return nullptr;
	}

	if (!payload.Version_1_0.HasValue()){
		return nullptr;
	}

	imtauth::IUserGroupInfoUniquePtr userGroupInfoPtr = m_userGroupFactoryCompPtr.CreateInstance();
	if (!userGroupInfoPtr.IsValid()){
		return nullptr;
	}

	if (payload.Version_1_0->name){
		userGroupInfoPtr->SetName(*payload.Version_1_0->name);
	}

	if (payload.Version_1_0->description){
		userGroupInfoPtr->SetDescription(*payload.Version_1_0->description);
	}

	if (payload.Version_1_0->roles){
		userGroupInfoPtr->SetRoles(productId, payload.Version_1_0->roles->ToList());
	}

	if (payload.Version_1_0->users){
		userGroupInfoPtr->SetUsers(payload.Version_1_0->users->ToList());
	}

	if (payload.Version_1_0->parentGroups){
		for (const QByteArray& groupId : payload.Version_1_0->parentGroups->ToList()){
			userGroupInfoPtr->AddParentGroup(groupId);
		}
	}

	return imtauth::IUserGroupInfoSharedPtr::CreateFromUnique(userGroupInfoPtr);
}


} // namespace imtauthgql


