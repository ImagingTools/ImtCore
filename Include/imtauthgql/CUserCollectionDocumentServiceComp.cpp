// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CUserCollectionDocumentServiceComp.h>


// Qt includes
#include <QUuid>

// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtauth/IUserInfo.h>


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
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::V1_0::imtauth::CUserData();
	}

	QByteArray userId = GetUserId(gqlRequest);

	QByteArray objectId;
	if (arguments.input.Version_1_0->id){
		objectId = *arguments.input.Version_1_0->id;
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
	response.Version_1_0.Emplace();

	response.Version_1_0->id = userPtr->GetObjectUuid();
	response.Version_1_0->name = userPtr->GetName();
	response.Version_1_0->username = userPtr->GetId();
	response.Version_1_0->email = userPtr->GetMail();

	response.Version_1_0->groups.Emplace();
	for (const QByteArray& groupId : userPtr->GetGroups()){
		response.Version_1_0->groups->push_back(groupId);
	}

	// Roles and permissions across all products this user has any role in.
	response.Version_1_0->roles.Emplace();
	response.Version_1_0->permissions.Emplace();
	for (const QByteArray& productId : userPtr->GetProducts()){
		for (const QByteArray& roleId : userPtr->GetRoles(productId)){
			response.Version_1_0->roles->push_back(roleId);
		}
		for (const QByteArray& permissionId : userPtr->GetLocalPermissions(productId)){
			response.Version_1_0->permissions->push_back(permissionId);
		}
	}

	response.Version_1_0->systemInfos.Emplace();
	for (const imtauth::IUserInfo::SystemInfo& systemInfo : userPtr->GetSystemInfos()){
		sdl::V1_0::imtauth::CSystemInfo systemEntry;
		systemEntry.id = systemInfo.systemId;
		systemEntry.name = systemInfo.systemName;
		systemEntry.enabled = systemInfo.enabled;
		response.Version_1_0->systemInfos->push_back(systemEntry);
	}

	return response;
}


sdl::V1_0::imtbase::CDocumentOperationStatus CUserCollectionDocumentServiceComp::OnUpdateUserFromRepresentation(
		const sdl::V1_0::imtauth::CUpdateUserFromRepresentationGqlRequest& updateUserFromRepresentationRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	sdl::V1_0::imtauth::UpdateUserFromRepresentationRequestArguments arguments = updateUserFromRepresentationRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::V1_0::imtbase::CDocumentOperationStatus();
	}

	sdl::V1_0::imtbase::CDocumentOperationStatus response;
	response.Version_1_0.Emplace();
	response.Version_1_0->status = sdl::V1_0::imtbase::EDocumentOperationStatus::Failed;

	QByteArray documentId;
	if (arguments.input.Version_1_0->documentId){
		documentId = *arguments.input.Version_1_0->documentId;
	}

	if (documentId.isEmpty()){
		errorMessage = QStringLiteral("Missing document ID");
		return response;
	}

	QByteArray userLogin = GetUserId(gqlRequest);

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userLogin, documentId, documentPtr);
	if (!documentPtr.IsValid()){
		response.Version_1_0->status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	imtauth::CIdentifiableUserInfo* userPtr = dynamic_cast<imtauth::CIdentifiableUserInfo*>(documentPtr.GetPtr());
	if (userPtr == nullptr){
		response.Version_1_0->status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	sdl::V1_0::imtauth::CUserData userData;
	if (arguments.input.Version_1_0->user){
		userData = *arguments.input.Version_1_0->user;
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

	response.Version_1_0->status = sdl::V1_0::imtbase::EDocumentOperationStatus::Success;

	return response;
}


} // namespace imtauthgql
