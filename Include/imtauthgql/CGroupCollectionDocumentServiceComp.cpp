// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CGroupCollectionDocumentServiceComp.h>


// Qt includes
#include <QUuid>

// ImtCore includes
#include <imtauth/CUserGroupInfo.h>
#include <imtauth/IUserGroupInfo.h>


namespace imtauthgql
{


// protected methods

// reimplemented (CGroupCollectionDocumentServiceGqlHandlerCompBase)

sdl::V1_0::imtauth::CGroupData CGroupCollectionDocumentServiceComp::OnGetGroupRepresentation(
		const sdl::V1_0::imtauth::CGetGroupRepresentationGqlRequest& getGroupRepresentationRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	sdl::V1_0::imtauth::GetGroupRepresentationRequestArguments arguments = getGroupRepresentationRequest.GetRequestedArguments();
	QByteArray userId = GetUserId(gqlRequest);

	QByteArray objectId;
	if (arguments.input.id){
		objectId = *arguments.input.id;
	}

	if (objectId.isEmpty()){
		errorMessage = QStringLiteral("Missing document ID");
		return sdl::V1_0::imtauth::CGroupData();
	}

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userId, objectId, documentPtr);
	if (!documentPtr.IsValid()){
		errorMessage = QStringLiteral("Document not found");
		return sdl::V1_0::imtauth::CGroupData();
	}

	const imtauth::CIdentifiableUserGroupInfo* groupPtr = dynamic_cast<const imtauth::CIdentifiableUserGroupInfo*>(documentPtr.GetPtr());
	if (groupPtr == nullptr){
		errorMessage = QStringLiteral("Invalid document type");
		return sdl::V1_0::imtauth::CGroupData();
	}

	sdl::V1_0::imtauth::CGroupData response;
	response.id = groupPtr->GetObjectUuid();
	response.name = groupPtr->GetName();
	response.description = groupPtr->GetDescription();

	response.users.Emplace();
	for (const QByteArray& userIdInGroup : groupPtr->GetUsers()){
		response.users->push_back(userIdInGroup);
	}

	response.parentGroups.Emplace();
	for (const QByteArray& parentGroupId : groupPtr->GetParentGroups()){
		response.parentGroups->push_back(parentGroupId);
	}

	// Roles are productId-scoped on IUserBaseInfo; without a tenant context
	// here we expose all products' roles concatenated.
	response.roles.Emplace();
	for (const QByteArray& productId : groupPtr->GetProducts()){
		for (const QByteArray& roleId : groupPtr->GetRoles(productId)){
			response.roles->push_back(roleId);
		}
	}

	return response;
}


sdl::V1_0::imtbase::CDocumentOperationStatus CGroupCollectionDocumentServiceComp::OnUpdateGroupFromRepresentation(
		const sdl::V1_0::imtauth::CUpdateGroupFromRepresentationGqlRequest& updateGroupFromRepresentationRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	sdl::V1_0::imtauth::UpdateGroupFromRepresentationRequestArguments arguments = updateGroupFromRepresentationRequest.GetRequestedArguments();
	sdl::V1_0::imtbase::CDocumentOperationStatus response;
	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Failed;

	QByteArray documentId;
	if (arguments.input.documentId){
		documentId = *arguments.input.documentId;
	}

	if (documentId.isEmpty()){
		errorMessage = QStringLiteral("Missing document ID");
		return response;
	}

	QByteArray userLogin = GetUserId(gqlRequest);

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userLogin, documentId, documentPtr);
	if (!documentPtr.IsValid()){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	imtauth::CIdentifiableUserGroupInfo* groupPtr = dynamic_cast<imtauth::CIdentifiableUserGroupInfo*>(documentPtr.GetPtr());
	if (groupPtr == nullptr){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	sdl::V1_0::imtauth::CGroupData groupData;
	if (arguments.input.group){
		groupData = *arguments.input.group;
	}

	if (groupData.id){
		groupPtr->SetObjectUuid(*groupData.id);
	}
	if (groupData.name){
		groupPtr->SetName(*groupData.name);
	}
	if (groupData.description){
		groupPtr->SetDescription(*groupData.description);
	}

	if (groupData.users){
		imtauth::IUserGroupInfo::UserIds userIds;
		for (const auto& userIdPtr : *groupData.users){
			if (userIdPtr){
				userIds.append(*userIdPtr);
			}
		}
		groupPtr->SetUsers(userIds);
	}

	if (groupData.parentGroups){
		// Replace parent groups: remove current and add new.
		imtauth::IUserGroupInfo::GroupIds currentParents = groupPtr->GetParentGroups();
		for (const QByteArray& parentGroupId : currentParents){
			groupPtr->RemoveParentGroup(parentGroupId);
		}
		for (const auto& parentGroupIdPtr : *groupData.parentGroups){
			if (parentGroupIdPtr){
				groupPtr->AddParentGroup(*parentGroupIdPtr);
			}
		}
	}

	if (groupData.roles && groupData.productId){
		QByteArray productId = *groupData.productId;
		imtauth::IUserBaseInfo::RoleIds roleIds;
		for (const auto& roleIdPtr : *groupData.roles){
			if (roleIdPtr){
				roleIds.append(*roleIdPtr);
			}
		}
		groupPtr->SetRoles(productId, roleIds);
	}

	m_documentManagerCompPtr->SetDocumentData(userLogin, documentId, *documentPtr);

	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Success;

	return response;
}


} // namespace imtauthgql
