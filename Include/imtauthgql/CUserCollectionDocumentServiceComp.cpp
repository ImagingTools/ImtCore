// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CUserCollectionDocumentServiceComp.h>


// Qt includes
#include <QUuid>

// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtauth/IUserInfo.h>

// Generated includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/UserCollectionDocumentService.h>


namespace imtauthgql
{


// protected methods

// reimplemented (CUserCollectionDocumentServiceGqlHandlerCompBase)

sdl::V1_0::imtauth::CUserData CUserCollectionDocumentServiceComp::OnGetUserRepresentation(
		const sdl::V1_0::imtauth::CGetUserRepresentationGqlRequest& getUserRepresentationRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	sdl::V1_0::imtauth::GetUserRepresentationRequestArguments arguments = getUserRepresentationRequest.GetRequestedArguments();
	QByteArray userId = GetUserId(gqlRequest);

	QByteArray objectId;
	if (arguments.input->id){
		objectId = *arguments.input->id;
	}

	if (objectId.isEmpty()){
		errorMessage = QStringLiteral("Missing document ID");
		return sdl::V1_0::imtauth::CUserData();
	}

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userId, objectId, documentPtr);
	if (!documentPtr.IsValid()){
		errorMessage = QStringLiteral("Document not found");
		return sdl::V1_0::imtauth::CUserData();
	}

	const imtauth::CIdentifiableUserInfo* userPtr = dynamic_cast<const imtauth::CIdentifiableUserInfo*>(documentPtr.GetPtr());
	if (userPtr == nullptr){
		errorMessage = QStringLiteral("Invalid document type");
		return sdl::V1_0::imtauth::CUserData();
	}

	sdl::V1_0::imtauth::CUserData response;
	response.id = userPtr->GetObjectUuid();
	response.name = userPtr->GetName();
	response.username = userPtr->GetId();
	response.email = userPtr->GetMail();

	response.groups.Emplace();
	for (const QByteArray& groupId : userPtr->GetGroups()){
		response.groups->push_back(groupId);
	}

	// Roles and permissions across all products this user has any role in.
	response.roles.Emplace();
	response.permissions.Emplace();
	for (const QByteArray& productId : userPtr->GetProducts()){
		for (const QByteArray& roleId : userPtr->GetRoles(productId)){
			response.roles->push_back(roleId);
		}
		for (const QByteArray& permissionId : userPtr->GetLocalPermissions(productId)){
			response.permissions->push_back(permissionId);
		}
	}

	response.systemInfos.Emplace();
	for (const imtauth::IUserInfo::SystemInfo& systemInfo : userPtr->GetSystemInfos()){
		sdl::V1_0::imtauth::CSystemInfo systemEntry;
		systemEntry.id = systemInfo.systemId;
		systemEntry.name = systemInfo.systemName;
		systemEntry.enabled = systemInfo.enabled;
		response.systemInfos->push_back(systemEntry);
	}

	return response;
}


sdl::V1_0::imtbase::CDocumentOperationStatus CUserCollectionDocumentServiceComp::OnUpdateUserFromRepresentation(
		const sdl::V1_0::imtauth::CUpdateUserFromRepresentationGqlRequest& updateUserFromRepresentationRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	sdl::V1_0::imtauth::UpdateUserFromRepresentationRequestArguments arguments = updateUserFromRepresentationRequest.GetRequestedArguments();
	sdl::V1_0::imtbase::CDocumentOperationStatus response;
	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Failed;

	QByteArray documentId;
	if (arguments.input->documentId){
		documentId = *arguments.input->documentId;
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

	imtauth::CIdentifiableUserInfo* userPtr = dynamic_cast<imtauth::CIdentifiableUserInfo*>(documentPtr.GetPtr());
	if (userPtr == nullptr){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	sdl::V1_0::imtauth::CUserData userData;
	if (arguments.input->user){
		userData = *arguments.input->user;
	}

	if (userData.id){
		userPtr->SetObjectUuid(*userData.id);
	}
	if (userData.name){
		userPtr->SetName(*userData.name);
	}
	if (userData.email){
		userPtr->SetMail(*userData.email);
	}
	if (userData.username){
		userPtr->SetId(*userData.username);
	}

	if (userData.groups){
		// Sync group membership: add new groups, remove ones that disappeared.
		imtauth::IUserGroupInfo::GroupIds currentGroups = userPtr->GetGroups();
		QList<QByteArray> newGroupsList;
		for (const auto& groupIdPtr : *userData.groups){
			if (groupIdPtr){
				newGroupsList.append(*groupIdPtr);
			}
		}
		for (const QByteArray& currentGroupId : currentGroups){
			if (!newGroupsList.contains(currentGroupId)){
				userPtr->RemoveFromGroup(currentGroupId);
			}
		}
		for (const QByteArray& newGroupId : newGroupsList){
			if (!currentGroups.contains(newGroupId)){
				userPtr->AddToGroup(newGroupId);
			}
		}
	}

	if (userData.roles && userData.productId){
		QByteArray productId = *userData.productId;
		imtauth::IUserBaseInfo::RoleIds roleIds;
		for (const auto& roleIdPtr : *userData.roles){
			if (roleIdPtr){
				roleIds.append(*roleIdPtr);
			}
		}
		userPtr->SetRoles(productId, roleIds);
	}

	if (userData.permissions && userData.productId){
		QByteArray productId = *userData.productId;
		imtauth::IUserBaseInfo::FeatureIds permissions;
		for (const auto& permissionIdPtr : *userData.permissions){
			if (permissionIdPtr){
				permissions.append(*permissionIdPtr);
			}
		}
		userPtr->SetLocalPermissions(productId, permissions);
	}

	m_documentManagerCompPtr->SetDocumentData(userLogin, documentId, *documentPtr);

	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Success;

	return response;
}


} // namespace imtauthgql
