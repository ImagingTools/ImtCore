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

	namespace userssdl = sdl::V1_0::imtauth;

	userssdl::GroupItemRequestArguments arguments;
	arguments.input.id = groupId;

	QByteArray productId;
	if (m_applicationInfoCompPtr.IsValid()){
		productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
	}

	arguments.input.productId = productId;

	imtgql::CGqlRequest gqlRequest;
	if (!userssdl::CGroupItemGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		return nullptr;
	}

	imtgql::IGqlContext* gqlContextPtr = imtgql::CGqlRequestContextManager::GetContext();
	if (gqlContextPtr != nullptr){
		istd::IChangeableUniquePtr clonedPtr = gqlContextPtr->CloneMe();
		imtgql::IGqlContextUniquePtr castedPtr;
		castedPtr.MoveCastedPtr(std::move(clonedPtr));
		gqlRequest.SetGqlContext(imtgql::IGqlContextSharedPtr::CreateFromUnique(castedPtr));
	}

	QString errorMessage;
	userssdl::CGroupData payload = SendModelRequest<userssdl::CGroupData>(gqlRequest, errorMessage);
	if (!errorMessage.isEmpty()){
		return nullptr;
	}

	if (!payload.HasValue()){
		return nullptr;
	}

	imtauth::IUserGroupInfoUniquePtr userGroupInfoPtr = m_userGroupFactoryCompPtr.CreateInstance();
	if (!userGroupInfoPtr.IsValid()){
		return nullptr;
	}

	if (payload.name){
		userGroupInfoPtr->SetName(*payload.name);
	}

	if (payload.description){
		userGroupInfoPtr->SetDescription(*payload.description);
	}

	if (payload.roles){
		userGroupInfoPtr->SetRoles(productId, payload.roles->ToList());
	}

	if (payload.users){
		userGroupInfoPtr->SetUsers(payload.users->ToList());
	}

	if (payload.parentGroups){
		for (const QByteArray& groupId : payload.parentGroups->ToList()){
			userGroupInfoPtr->AddParentGroup(groupId);
		}
	}

	return imtauth::IUserGroupInfoSharedPtr::CreateFromUnique(userGroupInfoPtr);
}


} // namespace imtauthgql


