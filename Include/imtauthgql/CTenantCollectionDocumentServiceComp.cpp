// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantCollectionDocumentServiceComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantCollectionDocumentService.h>


// Qt includes
#include <QMap>
#include <QSet>

// ImtCore includes
#include <imtauth/ITenantInfo.h>
#include <imtauth/IUserManager.h>
#include <imtgql/IGqlContext.h>
#include <imtdoc/CDocumentSavedEvent.h>
#include <imtauth/imtauth.h>
#include <imtauthgql/TenantAccess.h>


namespace
{


	QString TenantInvitationStatusToString(imtauth::ITenantInvitation::TenantInvitationStatus status)
{
	switch (status){
		case imtauth::ITenantInvitation::TIS_ACCEPTED:
			return QStringLiteral("Accepted");
		case imtauth::ITenantInvitation::TIS_REJECTED:
			return QStringLiteral("Rejected");
		case imtauth::ITenantInvitation::TIS_REVOKED:
			return QStringLiteral("Revoked");
		case imtauth::ITenantInvitation::TIS_EXPIRED:
			return QStringLiteral("Expired");
		case imtauth::ITenantInvitation::TIS_PENDING:
		default:
			return QStringLiteral("Pending");
	}
}


// An unsaved organization is edited outside of any organization, a stored one only from inside it.
bool IsOrganizationContextValid(
			const imtdoc::IDocumentService& documentService,
			const QByteArray& userId,
			const QByteArray& documentId,
			const QByteArray& contextTenantId)
{
	QByteArray storedTenantId;
	const imtdoc::IDocumentService::DocumentList documentList = documentService.GetOpenedDocumentList(userId);
	for (const imtdoc::IDocumentService::DocumentListItem& documentItem : documentList){
		if (documentItem.documentId == documentId){
			storedTenantId = documentItem.url.path().mid(1).toUtf8();
			break;
		}
	}

	return storedTenantId == contextTenantId;
}


} // anonymous namespace


namespace imtauthgql
{


// protected methods

// reimplemented (CTenantCollectionDocumentServiceGqlHandlerCompBase)

sdl::V1_0::imtauth::CTenantData CTenantCollectionDocumentServiceComp::OnGetTenantRepresentation(
			const sdl::V1_0::imtauth::CGetTenantRepresentationGqlRequest& getTenantRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::GetTenantRepresentationRequestArguments arguments = getTenantRepresentationRequest.GetRequestedArguments();
	QByteArray userId = GetUserId(gqlRequest);

	QByteArray objectId;
	if (arguments.input->id){
		objectId = *arguments.input->id;
	}

	if (objectId.isEmpty()){
		errorMessage = QStringLiteral("Missing document ID");
		return sdl::V1_0::imtauth::CTenantData();
	}

	QByteArray userObjectId;
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	QByteArray contextTenantId;
	if (gqlContextPtr != nullptr){
		contextTenantId = gqlContextPtr->GetTenantId();
		userObjectId = gqlContextPtr->GetUserId();
	}

	// A just created organization is still read back outside of any organization before the client switches into it.
	if (!contextTenantId.isEmpty() && !IsOrganizationContextValid(*m_documentManagerCompPtr, userId, objectId, contextTenantId)){
		errorMessage = QStringLiteral("An organization can be edited only while you work in it. Switch to the organization first.");
		return sdl::V1_0::imtauth::CTenantData();
	}

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userId, objectId, documentPtr);
	if (!documentPtr.IsValid()){
		errorMessage = QStringLiteral("Document not found");
		return sdl::V1_0::imtauth::CTenantData();
	}

	imtauth::ITenantInfo* tenantPtr = dynamic_cast<imtauth::ITenantInfo*>(documentPtr.GetPtr());
	if (tenantPtr == nullptr){
		errorMessage = QStringLiteral("Invalid document type");
		return sdl::V1_0::imtauth::CTenantData();
	}

	SyncStoredOwner(*tenantPtr);

	sdl::V1_0::imtauth::CTenantData response;
	QByteArray tenantId = tenantPtr->GetTenantId();

	response.id = tenantId;

	bool isSystemAdmin = false;
	if (gqlContextPtr != nullptr){
		const imtauth::IUserInfo* contextUserInfoPtr = gqlContextPtr->GetUserInfo();
		if (contextUserInfoPtr != nullptr){
			isSystemAdmin = contextUserInfoPtr->IsAdmin();
		}
	}

	const bool isCreator = (!tenantId.isEmpty() && tenantPtr->GetCreatorId() == userObjectId);
	const bool isOwner = (!tenantId.isEmpty() && tenantPtr->GetOwnerId() == userObjectId);
	bool isMember = false;
	if (!tenantId.isEmpty() && m_membershipManagerCompPtr.IsValid()){
		isMember = m_membershipManagerCompPtr->IsMember(userObjectId, tenantId);
	}

	// Prevent invited users (and any non-members) from reading tenant details
	// before they become active members.
	if (!tenantId.isEmpty() && !isSystemAdmin && !isCreator && !isOwner && !isMember){
		errorMessage = QStringLiteral("Access denied");
		return sdl::V1_0::imtauth::CTenantData();
	}

	response.name = tenantPtr->GetTenantName();
	response.description = tenantPtr->GetTenantDescription();
	response.ownerId = tenantPtr->GetOwnerId();
	response.creatorId = tenantPtr->GetCreatorId();
	response.isActive = tenantPtr->IsActive();
	response.createdAt = tenantPtr->GetCreatedAt();
	response.updatedAt = tenantPtr->GetUpdatedAt();

	// Pass current user ID so GUI can determine role-based access
	if (gqlContextPtr != nullptr){
		response.currentUserId = gqlContextPtr->GetUserId();
	}

	// Populate the organization permissions available to the *current user* within this tenant
	// (used by TenantEditor to know what org features the user can manage)
	if (m_membershipManagerCompPtr.IsValid() && gqlContextPtr != nullptr){
		QByteArray currentUid = gqlContextPtr->GetUserId();
		if (!currentUid.isEmpty()){
			imtauth::ITenantMembershipUniquePtr myMs = m_membershipManagerCompPtr->FindMembership(currentUid, tenantId);
			if (myMs.IsValid()){
				response.currentUserOrganizationPermissions.Emplace().FromList(myMs->GetOrganizationPermissions());
			}
		}
	}

	if (m_membershipManagerCompPtr.IsValid()){
		QByteArrayList membershipIds = m_membershipManagerCompPtr->GetMembershipsByTenant(tenantId);
		response.members.Emplace();

		QByteArray ownerId = tenantPtr->GetOwnerId();
		QByteArray creatorId = tenantPtr->GetCreatorId();

		bool ownerFound = false;

		for (const QByteArray& membershipId : membershipIds){
			imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
			if (membershipPtr.IsValid() && membershipPtr->IsActive()){
				QByteArray userId = membershipPtr->GetUserId();

				// Skip Creator (unless they are also the Owner)
				if (!creatorId.isEmpty() && userId == creatorId && userId != ownerId){
					continue;
				}

				sdl::V1_0::imtauth::CTenantMemberEntry memberEntry;
				memberEntry.id = userId;

				if (m_userCollectionCompPtr.IsValid()){
					memberEntry.name = imtauth::GetUserName(*m_userCollectionCompPtr, userId);
				}

				response.members->push_back(memberEntry);

				if (!ownerId.isEmpty() && userId == ownerId){
					ownerFound = true;
				}
			}
		}

		// Ensure the Owner always appears in the members list even if no membership entry exists
		if (!ownerId.isEmpty() && !ownerFound){
			sdl::V1_0::imtauth::CTenantMemberEntry ownerEntry;
			ownerEntry.id = ownerId;
			if (m_userCollectionCompPtr.IsValid()){
				ownerEntry.name = imtauth::GetUserName(*m_userCollectionCompPtr, ownerId);
			}
			response.members->push_back(ownerEntry);
		}
	}

	if (m_invitationManagerCompPtr.IsValid()){
		imtauth::ITenantInvitationManager::Statuses statuses;
		statuses.append(imtauth::ITenantInvitation::TIS_PENDING);
		QByteArrayList invitationIds = m_invitationManagerCompPtr->GetInvitationsByTenant(tenantId, statuses);
		response.pendingInvitations.Emplace();

		// Collect IDs of users that are already part of the tenant so we can
		// suppress stale pending invitations for them (e.g. an invitation that
		// pre-dates the user becoming the tenant's owner/creator/member).
		QSet<QByteArray> existingMemberUserIds;
		if (!tenantPtr->GetOwnerId().isEmpty()){
			existingMemberUserIds.insert(tenantPtr->GetOwnerId());
		}
		if (!tenantPtr->GetCreatorId().isEmpty()){
			existingMemberUserIds.insert(tenantPtr->GetCreatorId());
		}
		if (m_membershipManagerCompPtr.IsValid()){
			QByteArrayList membershipIds = m_membershipManagerCompPtr->GetMembershipsByTenant(tenantId);
			for (const QByteArray& mId : membershipIds){
				imtauth::ITenantMembershipUniquePtr mPtr = m_membershipManagerCompPtr->GetMembership(mId);
				if (mPtr.IsValid() && mPtr->IsActive()){
					existingMemberUserIds.insert(mPtr->GetUserId());
				}
			}
		}
		for (const QByteArray& invitationId : invitationIds){
			imtauth::ITenantInvitationUniquePtr invitationPtr = m_invitationManagerCompPtr->GetInvitation(invitationId);
			if (invitationPtr.IsValid()){
				// Suppress invitations targeting users that are already
				// part of the tenant (owner/creator/active member).
				if (existingMemberUserIds.contains(invitationPtr->GetUserId())){
					continue;
				}
				sdl::V1_0::imtauth::CTenantInvitationEntry invitationEntry;
				invitationEntry.id = invitationPtr->GetInvitationId();
				invitationEntry.userId = invitationPtr->GetUserId();
				invitationEntry.status = TenantInvitationStatusToString(m_invitationManagerCompPtr->GetEffectiveStatus(*invitationPtr));
				invitationEntry.invitedByUserId = invitationPtr->GetInvitedByUserId();
				invitationEntry.createdAt = invitationPtr->GetCreatedAt();
				invitationEntry.expiresAt = invitationPtr->GetExpiresAt();

				if (m_userCollectionCompPtr.IsValid()){
					invitationEntry.userName = imtauth::GetUserName(*m_userCollectionCompPtr, invitationPtr->GetUserId());
					invitationEntry.invitedByName = imtauth::GetUserName(*m_userCollectionCompPtr, invitationPtr->GetInvitedByUserId());
				}

				response.pendingInvitations->push_back(invitationEntry);
			}
		}
	}

	// availableRoles will be provided by the client-side RoleCollectionDataProvider

	// Tenant-scoped permissions (selected subset of product permissions)
	response.tenantPermissions.Emplace().FromList(tenantPtr->GetTenantPermissions());

	// Hierarchy fields
	response.parentTenantId = tenantPtr->GetParentTenantId();
	response.isSystemTenant = (tenantPtr->GetTenantId() == imtauth::GetSystemTenantId());

	return response;
}


sdl::V1_0::imtbase::CDocumentOperationStatus CTenantCollectionDocumentServiceComp::OnUpdateTenantFromRepresentation(
			const sdl::V1_0::imtauth::CUpdateTenantFromRepresentationGqlRequest& updateTenantFromRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtauth::UpdateTenantFromRepresentationRequestArguments arguments = updateTenantFromRepresentationRequest.GetRequestedArguments();
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

	QByteArray contextUserId;
	QByteArray contextTenantId;
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	QByteArrayList contextUserPermissions;
	bool isSystemAdmin = false;
	if (gqlContextPtr != nullptr){
		contextUserId = gqlContextPtr->GetUserId();
		contextTenantId = gqlContextPtr->GetTenantId();
		const imtauth::IUserInfo* contextUserInfoPtr = gqlContextPtr->GetUserInfo();
		if (contextUserInfoPtr != nullptr){
			contextUserPermissions = contextUserInfoPtr->GetPermissions(gqlContextPtr->GetProductId());
			isSystemAdmin = contextUserInfoPtr->IsAdmin();
		}
	}

	QByteArray userLogin = GetUserId(gqlRequest);

	if (!IsOrganizationContextValid(*m_documentManagerCompPtr, userLogin, documentId, contextTenantId)){
		errorMessage = QStringLiteral("An organization can be edited only while you work in it. Switch to the organization first.");
		return response;
	}

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userLogin, documentId, documentPtr);
	if (!documentPtr.IsValid()){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	imtauth::ITenantInfo* tenantPtr = dynamic_cast<imtauth::ITenantInfo*>(documentPtr.GetPtr());
	if (tenantPtr == nullptr){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	SyncStoredOwner(*tenantPtr);

	sdl::V1_0::imtauth::CTenantData tenantData;
	if (arguments.input->tenant){
		tenantData = *arguments.input->tenant;
	}

	QByteArray tenantId = tenantPtr->GetTenantId();
	QByteArrayList effectiveUserPermissions = contextUserPermissions;
	if (!isSystemAdmin && !tenantId.isEmpty()){
		// Scope permissions to the current tenant to prevent cross-tenant leakage.
		QSet<QByteArray> tenantPermissionSet;
		for (const QByteArray& permissionId : tenantPtr->GetTenantPermissions()){
			if (!permissionId.isEmpty()){
				tenantPermissionSet.insert(permissionId);
			}
		}

		QByteArrayList scopedPermissions;
		for (const QByteArray& permissionId : contextUserPermissions){
			// A permission held in the old format ("EditUser") is the one the
			// tenant grants as a path ("/Administration/EditUser"), so the two
			// are matched by the common rule instead of by equality.
			if (imtauth::HasPermission(tenantPermissionSet, permissionId)){
				scopedPermissions.push_back(permissionId);
			}
		}

		effectiveUserPermissions = scopedPermissions;
	}

	// Additionally grant organization-specific permissions from the user's membership in this tenant.
	// These control org-internal operations (member mgmt, roles, groups, etc.) and are not part of tenant's product permissions.
	if (!isSystemAdmin && !tenantId.isEmpty() && m_membershipManagerCompPtr.IsValid()){
		imtauth::ITenantMembershipUniquePtr msPtr = m_membershipManagerCompPtr->FindMembership(contextUserId, tenantId);
		if (msPtr.IsValid() && msPtr->IsActive()){
			for (const QByteArray& p : msPtr->GetOrganizationPermissions()){
				if (!p.isEmpty() && !effectiveUserPermissions.contains(p)){
					effectiveUserPermissions.push_back(p);
				}
			}
		}
	}

	// Role-based access control for existing tenants
	bool isCreator = !tenantId.isEmpty() && (tenantPtr->GetCreatorId() == contextUserId);
	bool isOwner = !tenantId.isEmpty() && (tenantPtr->GetOwnerId() == contextUserId);
	bool isAdmin = false;
	bool isMember = false;
	if (!tenantId.isEmpty() && m_membershipManagerCompPtr.IsValid()){
		// Check stored role for Admin
		imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->FindMembership(contextUserId, tenantId);
		if (membershipPtr.IsValid() && membershipPtr->IsActive()){
			isMember = true;
			// isAdmin now based on having key org permission (replacing env role)
		isAdmin = isOwner || isCreator || HasAnyOrganizationPermission(membershipPtr->GetOrganizationPermissions(), GetEditOrganizationMemberPermissions());
		}
	}
	bool isNewTenant = tenantId.isEmpty();

	const bool canChangeOrganizationName = isSystemAdmin || HasAnyOrganizationPermission(effectiveUserPermissions, {s_changeOrganizationNamePermission});
	const bool canChangeOrganizationDescription = isSystemAdmin || HasAnyOrganizationPermission(effectiveUserPermissions, {s_changeOrganizationDescriptionPermission});
	const bool canChangeOrganizationState = isSystemAdmin || HasAnyOrganizationPermission(effectiveUserPermissions, GetEditOrganizationPermissions());
	const bool canInviteOrganizationMember = isSystemAdmin || HasAnyOrganizationPermission(effectiveUserPermissions, {s_inviteOrganizationMemberPermission});
	const bool canExcludeOrganizationMember = isSystemAdmin || HasAnyOrganizationPermission(effectiveUserPermissions, {s_excludeOrganizationMemberPermission});
	const bool canChangeOrganizationMemberRole = isSystemAdmin || HasAnyOrganizationPermission(effectiveUserPermissions, {s_changeOrganizationMemberRolePermission});

	// Block updates from non-members (e.g., invited users who haven't accepted yet)
	if (!isNewTenant && !isOwner && !isCreator && !isMember){
		return response;
	}

	const bool hasNameChange = tenantData.name && (*tenantData.name != tenantPtr->GetTenantName());
	const bool hasDescriptionChange = tenantData.description && (*tenantData.description != tenantPtr->GetTenantDescription());
	const bool hasActiveChange = tenantData.isActive && (*tenantData.isActive != tenantPtr->IsActive());

	if (hasNameChange && !isNewTenant && !isCreator && !isOwner && !canChangeOrganizationName){
		errorMessage = QStringLiteral("Permission denied: /OrganizationManagement/Edit/ChangeName");
		return response;
	}

	if (hasDescriptionChange && !isNewTenant && !isCreator && !isOwner && !canChangeOrganizationDescription){
		errorMessage = QStringLiteral("Permission denied: /OrganizationManagement/Edit/ChangeDescription");
		return response;
	}

	if (hasActiveChange && !isNewTenant && !isCreator && !isOwner && !canChangeOrganizationState){
		errorMessage = QStringLiteral("Permission denied: ChangeOrganization");
		return response;
	}

	if (hasNameChange){
		tenantPtr->SetTenantName(*tenantData.name);
	}

	if (hasDescriptionChange){
		tenantPtr->SetTenantDescription(*tenantData.description);
	}

	if (hasActiveChange){
		tenantPtr->SetActive(*tenantData.isActive);
	}

	// CreatorId and OwnerId are set once on first save.
	// Prefer explicit ownerId from representation; otherwise fallback to context user.
	QByteArray resolvedOwnerId = tenantPtr->GetOwnerId();
	if (resolvedOwnerId.isEmpty()){
		if (tenantData.ownerId && !tenantData.ownerId->isEmpty()){
			resolvedOwnerId = *tenantData.ownerId;
		}
		else if (!contextUserId.isEmpty()){
			resolvedOwnerId = contextUserId;
		}

		if (!resolvedOwnerId.isEmpty()){
			tenantPtr->SetOwnerId(resolvedOwnerId);
		}
	}

	QByteArray resolvedCreatorId = tenantPtr->GetCreatorId();
	if (resolvedCreatorId.isEmpty()){
		if (!contextUserId.isEmpty()){
			resolvedCreatorId = contextUserId;
		}
		else if (!resolvedOwnerId.isEmpty()){
			resolvedCreatorId = resolvedOwnerId;
		}

		if (!resolvedCreatorId.isEmpty()){
			tenantPtr->SetCreatorId(resolvedCreatorId);
		}
	}

	if (isNewTenant && (tenantPtr->GetOwnerId().isEmpty() || tenantPtr->GetCreatorId().isEmpty())){
		errorMessage = QStringLiteral("Unable to save tenant: OwnerId and CreatorId must be set");
		return response;
	}

	// Only Owner/Admin can manage members (add/remove/change roles)
	// Exception: any member can remove themselves (leave)
	if (!tenantId.isEmpty() && tenantData.members && m_membershipManagerCompPtr.IsValid()){
		QByteArrayList currentMembershipIds = m_membershipManagerCompPtr->GetMembershipsByTenant(tenantId);
		QMap<QByteArray, QByteArray> userIdToMembershipId;
		QSet<QByteArray> currentUserIds;
		for (const QByteArray& membershipId : currentMembershipIds){
			imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
			if (membershipPtr.IsValid() && membershipPtr->IsActive()){
				currentUserIds.insert(membershipPtr->GetUserId());
				userIdToMembershipId[membershipPtr->GetUserId()] = membershipId;
			}
		}

		QSet<QByteArray> newUserIds;
		if (tenantData.members){
			for (const sdl::V1_0::imtauth::CTenantMemberEntry& member : tenantData.members->ToList()){
				newUserIds.insert(*member.id);
			}
		}

		QSet<QByteArray> removedUserIds = currentUserIds;
		for (const QByteArray& userId : newUserIds){
			removedUserIds.remove(userId);
		}

		QSet<QByteArray> addedUserIds = newUserIds;
		for (const QByteArray& userId : currentUserIds){
			addedUserIds.remove(userId);
		}

		// Determine if current user is leaving (self-removal)
		bool isSelfLeaving = isMember && !isOwner && !isCreator && currentUserIds.contains(contextUserId) && !newUserIds.contains(contextUserId);

		bool removesOthers = false;
		for (const QByteArray& removedUserId : removedUserIds){
			if (removedUserId != contextUserId){
				removesOthers = true;
				break;
			}
		}

		if (removesOthers && !isCreator && !isOwner && !isAdmin && !canExcludeOrganizationMember){
			errorMessage = QStringLiteral("Permission denied: /OrganizationManagement/Members/Edit/Exclude");
			return response;
		}

		if (!addedUserIds.isEmpty() && !isCreator && !isOwner && !isAdmin && !canInviteOrganizationMember){
			errorMessage = QStringLiteral("Permission denied: /OrganizationManagement/Members/Edit/Invite");
			return response;
		}

		if (isOwner || isCreator || isAdmin || isNewTenant){
			// Creator/Owner/Admin can remove any member (except creator/owner) and add new ones
			QByteArray ownerId = tenantPtr->GetOwnerId();
			QByteArray creatorId = tenantPtr->GetCreatorId();
			for (const QByteArray& existingUserId : currentUserIds){
				if (!newUserIds.contains(existingUserId) && existingUserId != ownerId && existingUserId != creatorId){
					m_membershipManagerCompPtr->RemoveMembership(userIdToMembershipId.value(existingUserId));
				}
			}

			for (const QByteArray& addUserId : newUserIds){
				if (!currentUserIds.contains(addUserId)){
					if (m_invitationManagerCompPtr.IsValid()){
						m_invitationManagerCompPtr->CreateInvitation(contextUserId, addUserId, tenantId);
					}
				}
			}
		} else if (isSelfLeaving){
			// Non-admin member can only remove themselves
			m_membershipManagerCompPtr->RemoveMembership(userIdToMembershipId.value(contextUserId));
		}

		// Apply tenant permissions (only Creator can change permissions)
		if (isCreator && tenantData.tenantPermissions){
			tenantPtr->SetTenantPermissions(tenantData.tenantPermissions->ToList());
		}
	}

	m_documentManagerCompPtr->SetDocumentData(userLogin, documentId, *documentPtr);

	// For new tenants do NOT auto-save — user will save manually.
	// For existing tenants, save immediately after each change (like tickets).
	if (!tenantId.isEmpty()){
		imtdoc::IDocumentService::TaskParams saveParams;
		saveParams.userId = userLogin;
		saveParams.documentId = documentId;
		QByteArray saveTaskId = m_documentManagerCompPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_SAVE, saveParams);
		m_documentManagerCompPtr->WaitForTaskFinished(saveTaskId);
	}

	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Success;

	return response;
}


// reimplemented (imtdoc::IDocumentServiceEventHandler)

bool CTenantCollectionDocumentServiceComp::ProcessEvent(imtdoc::CEventBase* eventPtr)
{
	imtdoc::CDocumentSavedEvent* savedEventPtr = dynamic_cast<imtdoc::CDocumentSavedEvent*>(eventPtr);
	if (savedEventPtr != nullptr){
		QByteArray documentId = savedEventPtr->GetDocumentId();
		QByteArray userId = savedEventPtr->GetUserId();
		QByteArray objectId = savedEventPtr->GetDocumentUrl().toString().toUtf8().replace("collection:///", "");

		istd::IChangeableSharedPtr documentPtr;
		imtauth::ITenantInfo* documentTicketPtr = nullptr;
		if (m_documentManagerCompPtr->GetDocumentData(userId, documentId, documentPtr) == imtdoc::IDocumentService::OS_OK){
			documentTicketPtr = dynamic_cast<imtauth::ITenantInfo*>(documentPtr.GetPtr());
		}

		if (documentTicketPtr != nullptr){
			QByteArray tenantId = documentTicketPtr->GetTenantId();
			if (tenantId.isEmpty()){
				documentTicketPtr->SetTenantId(objectId);

				m_documentManagerCompPtr->SetDocumentData(userId, documentId, *documentPtr);

				imtdoc::IDocumentService::TaskParams saveParams;
				saveParams.userId = userId;
				saveParams.documentId = documentId;
				QByteArray saveTaskId = m_documentManagerCompPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_SAVE, saveParams);
				m_documentManagerCompPtr->WaitForTaskFinished(saveTaskId);

				// Auto-create OWNER membership for the tenant creator
				QByteArray ownerId = documentTicketPtr->GetOwnerId();
				if (m_membershipManagerCompPtr.IsValid() && !ownerId.isEmpty()){
					m_membershipManagerCompPtr->AddMembership(ownerId, objectId);
				}
			}
		}
	}

	return true;
}


// private methods

void CTenantCollectionDocumentServiceComp::SyncStoredOwner(imtauth::ITenantInfo& tenant) const
{
	const QByteArray tenantId = tenant.GetTenantId();
	if (tenantId.isEmpty() || !m_tenantCollectionCompPtr.IsValid()){
		return;
	}

	imtbase::IObjectCollection::DataPtr storedDataPtr;
	if (!m_tenantCollectionCompPtr->GetObjectData(tenantId, storedDataPtr)){
		return;
	}

	const imtauth::ITenantInfo* storedTenantPtr = dynamic_cast<const imtauth::ITenantInfo*>(storedDataPtr.GetPtr());
	if (storedTenantPtr != nullptr && !storedTenantPtr->GetOwnerId().isEmpty()){
		tenant.SetOwnerId(storedTenantPtr->GetOwnerId());
	}
}


} // namespace imtauthgql

