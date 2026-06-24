// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CUserCollectionDocumentServiceComp.h>


// Qt includes
#include <QMutexLocker>
#include <QSet>
#include <QUuid>

// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtauth/IUserInfo.h>
#include <imtdoc/CDocumentSavedEvent.h>
#include <imtgql/IGqlContext.h>

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

	// Resolve tenantId: prefer explicit input field, fallback to JWT context
	QByteArray tenantId;
	if (arguments.input->tenantId){
		tenantId = *arguments.input->tenantId;
	}
	if (tenantId.isEmpty()){
		const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
		if (gqlContextPtr != nullptr){
			tenantId = gqlContextPtr->GetTenantId();
		}
	}

	if (!tenantId.isEmpty()){
		QMutexLocker locker(&m_pendingTenantIdsMutex);
		m_pendingTenantIds.insert(documentId, tenantId);
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

	// Handle system info: remove existing systems and apply the new ones
	imtauth::IUserInfo::SystemInfoList currentSystemList = userPtr->GetSystemInfos();
	for (const imtauth::IUserInfo::SystemInfo& systemInfo : currentSystemList){
		while (userPtr->RemoveFromSystem(systemInfo.systemId)){}
	}

	if (userData.systemInfos){
		QSet<QByteArray> seenSystemIds;
		for (const istd::TNullableValue<sdl::V1_0::imtauth::CSystemInfo>& sdlSystemInfo : *userData.systemInfos){
			if (!sdlSystemInfo.HasValue()){
				continue;
			}

			imtauth::IUserInfo::SystemInfo systemInfo;
			if (sdlSystemInfo->id){
				systemInfo.systemId = *sdlSystemInfo->id;
			}

			const QByteArray systemKey = systemInfo.systemId.isEmpty()
					? QByteArrayLiteral("__internal__")
					: systemInfo.systemId;
			if (seenSystemIds.contains(systemKey)){
				continue;
			}
			seenSystemIds.insert(systemKey);

			if (!systemInfo.systemId.isEmpty() && sdlSystemInfo->name){
				systemInfo.systemName = *sdlSystemInfo->name;
			}
			if (sdlSystemInfo->enabled){
				systemInfo.enabled = *sdlSystemInfo->enabled;
			}

			userPtr->AddToSystem(systemInfo);
		}

		if (seenSystemIds.isEmpty()){
			imtauth::IUserInfo::SystemInfo systemInfo;
			userPtr->AddToSystem(systemInfo);
		}
	}
	else{
		// No system info provided — add default internal system
		imtauth::IUserInfo::SystemInfo systemInfo;
		userPtr->AddToSystem(systemInfo);
	}

	// Handle password: hash when user is in internal system (empty systemId, enabled)
	if (userData.password){
		QByteArray username = userPtr->GetId();
		if (!username.isEmpty() && m_hashCalculatorCompPtr.IsValid()){
			for (const imtauth::IUserInfo::SystemInfo& systemInfo : userPtr->GetSystemInfos()){
				if (systemInfo.enabled && systemInfo.systemId.isEmpty()){
					QString password = *userData.password;
					if (!password.isEmpty()){
						password = m_hashCalculatorCompPtr->GenerateHash(username + password.toUtf8());
						userPtr->SetPasswordHash(password.toUtf8());
					}
					break;
				}
			}
		}
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


// reimplemented (imtdoc::IDocumentServiceEventHandler)

bool CUserCollectionDocumentServiceComp::ProcessEvent(imtdoc::CEventBase* eventPtr)
{
	imtdoc::CDocumentSavedEvent* savedEventPtr = dynamic_cast<imtdoc::CDocumentSavedEvent*>(eventPtr);
	if (savedEventPtr == nullptr){
		return true;
	}

	if (!m_membershipManagerCompPtr.IsValid()){
		return true;
	}

	QByteArray documentId = savedEventPtr->GetDocumentId();
	QByteArray userId = savedEventPtr->GetUserId();

	// Retrieve the tenantId that was stored during OnUpdateUserFromRepresentation
	QByteArray tenantId;
	{
		QMutexLocker locker(&m_pendingTenantIdsMutex);
		tenantId = m_pendingTenantIds.take(documentId);
	}

	if (tenantId.isEmpty()){
		return true;
	}

	// Get the saved user document to extract the user's objectUuid
	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userId, documentId, documentPtr);
	if (!documentPtr.IsValid()){
		return true;
	}

	const imtauth::CIdentifiableUserInfo* userPtr = dynamic_cast<const imtauth::CIdentifiableUserInfo*>(documentPtr.GetPtr());
	if (userPtr == nullptr){
		return true;
	}

	QByteArray newUserId = userPtr->GetObjectUuid();
	if (newUserId.isEmpty()){
		return true;
	}

	// Check if user is already a member of this tenant
	if (m_membershipManagerCompPtr->IsMember(newUserId, tenantId)){
		return true;
	}

	QByteArray membershipId = m_membershipManagerCompPtr->AddMembership(newUserId, tenantId, "Member");
	if (membershipId.isEmpty()){
		SendWarningMessage(0, QString("Auto-membership creation failed for user '%1' in tenant '%2'").arg(QString::fromUtf8(newUserId), QString::fromUtf8(tenantId)), "CUserCollectionDocumentServiceComp");
	}

	return true;
}


} // namespace imtauthgql
