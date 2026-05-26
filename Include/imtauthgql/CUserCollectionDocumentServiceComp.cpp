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

// reimplemented (CGraphQlHandlerCompBase)

sdl::imtauth::Users::CUserData CUserCollectionDocumentServiceComp::OnGetUserRepresentation(
		const sdl::imtauth::UserCollectionDocumentService::CGetUserRepresentationGqlRequest& getUserRepresentationRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	sdl::imtauth::UserCollectionDocumentService::GetUserRepresentationRequestArguments arguments = getUserRepresentationRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtauth::Users::CUserData();
	}

	QByteArray userId = GetUserId(gqlRequest);

	QByteArray objectId;
	if (arguments.input.Version_1_0->id){
		objectId = *arguments.input.Version_1_0->id;
	}

	if (objectId.isEmpty()){
		errorMessage = QStringLiteral("Missing document ID");
		return sdl::imtauth::Users::CUserData();
	}

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userId, objectId, documentPtr);
	if (!documentPtr.IsValid()){
		errorMessage = QStringLiteral("Document not found");
		return sdl::imtauth::Users::CUserData();
	}

	const imtauth::CIdentifiableUserInfo* userPtr = dynamic_cast<const imtauth::CIdentifiableUserInfo*>(documentPtr.GetPtr());
	if (userPtr == nullptr){
		errorMessage = QStringLiteral("Invalid document type");
		return sdl::imtauth::Users::CUserData();
	}

	sdl::imtauth::Users::CUserData response;
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
		sdl::imtauth::Users::CSystemInfo::V1_0 systemEntry;
		systemEntry.id = systemInfo.systemId;
		systemEntry.name = systemInfo.systemName;
		systemEntry.enabled = systemInfo.enabled;
		response.Version_1_0->systemInfos->push_back(systemEntry);
	}

	return response;
}


sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus CUserCollectionDocumentServiceComp::OnUpdateUserFromRepresentation(
		const sdl::imtauth::UserCollectionDocumentService::CUpdateUserFromRepresentationGqlRequest& updateUserFromRepresentationRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	sdl::imtauth::UserCollectionDocumentService::UpdateUserFromRepresentationRequestArguments arguments = updateUserFromRepresentationRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus();
	}

	sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus response;
	response.Version_1_0.Emplace();
	response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::Failed;

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
		response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	imtauth::CIdentifiableUserInfo* userPtr = dynamic_cast<imtauth::CIdentifiableUserInfo*>(documentPtr.GetPtr());
	if (userPtr == nullptr){
		response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	// Make sure the document has a stable UUID — generate one for newly
	// created documents that have not yet been persisted.
	if (userPtr->GetObjectUuid().isEmpty()){
		userPtr->SetObjectUuid(QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8());
	}

	sdl::imtauth::Users::CUserData::V1_0 userData;
	if (arguments.input.Version_1_0->user){
		userData = *arguments.input.Version_1_0->user;
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

	response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::Success;

	return response;
}


} // namespace imtauthgql
