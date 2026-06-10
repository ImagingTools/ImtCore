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

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userId, objectId, documentPtr);
	if (!documentPtr.IsValid()){
		errorMessage = QStringLiteral("Document not found");
		return sdl::V1_0::imtauth::CTenantData();
	}

	const imtauth::ITenantInfo* tenantPtr = dynamic_cast<const imtauth::ITenantInfo*>(documentPtr.GetPtr());
	if (tenantPtr == nullptr){
		errorMessage = QStringLiteral("Invalid document type");
		return sdl::V1_0::imtauth::CTenantData();
	}

	sdl::V1_0::imtauth::CTenantData response;
	QByteArray tenantId = tenantPtr->GetTenantId();

	// Server-side access control: user can only open TenantEditor if
	// they have switched to this tenant (current tenant in JWT must match)
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();

	response.id = tenantId;
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
	if (m_membershipManagerCompPtr.IsValid()){
		QByteArrayList membershipIds = m_membershipManagerCompPtr->GetMembershipsByTenant(tenantId);
		response.members.Emplace();
		response.memberRoles.Emplace();

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

				// Assign environment role: Owner > stored role (Admin/Member)
				sdl::V1_0::imtauth::CTenantMemberRoleEntry roleEntry;
				roleEntry.userId = userId;
				if (!ownerId.isEmpty() && userId == ownerId){
					roleEntry.role = TenantEnvironmentRoleToString(imtauth::TER_OWNER);
					ownerFound = true;
				} else {
					roleEntry.role = TenantEnvironmentRoleToString(membershipPtr->GetEnvironmentRole());
				}
				response.memberRoles->push_back(roleEntry);
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

			sdl::V1_0::imtauth::CTenantMemberRoleEntry ownerRoleEntry;
			ownerRoleEntry.userId = ownerId;
			ownerRoleEntry.role = TenantEnvironmentRoleToString(imtauth::TER_OWNER);
			response.memberRoles->push_back(ownerRoleEntry);
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
				invitationEntry.role = QString::fromUtf8(invitationPtr->GetRoleId());
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
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		contextUserId = gqlContextPtr->GetUserId();
	}

	QByteArray userLogin = GetUserId(gqlRequest);

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

	sdl::V1_0::imtauth::CTenantData tenantData;
	if (arguments.input->tenant){
		tenantData = *arguments.input->tenant;
	}

	QByteArray tenantId = tenantPtr->GetTenantId();

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
			isAdmin = isOwner || isCreator || (membershipPtr->GetEnvironmentRole() == imtauth::TER_ADMIN);
		}
	}
	bool isNewTenant = tenantId.isEmpty();

	// Block updates from non-members (e.g., invited users who haven't accepted yet)
	if (!isNewTenant && !isOwner && !isCreator && !isMember){
		return response;
	}

	// Only Creator/Owner (or new tenant creator) can change Name/Description/Active
	if (isCreator || isOwner || isNewTenant){
		if (tenantData.name){
			tenantPtr->SetTenantName(*tenantData.name);
		}

		if (tenantData.description){
			tenantPtr->SetTenantDescription(*tenantData.description);
		}

		if (tenantData.isActive){
			tenantPtr->SetActive(*tenantData.isActive);
		}
	}

	// CreatorId and OwnerId are set once on first save — the creator becomes both Creator and Owner.
	if (tenantPtr->GetCreatorId().isEmpty()){
		tenantPtr->SetCreatorId(contextUserId);
	}
	if (tenantPtr->GetOwnerId().isEmpty()){
		tenantPtr->SetOwnerId(contextUserId);
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

		// Determine if current user is leaving (self-removal)
		bool isSelfLeaving = isMember && !isOwner && !isCreator && currentUserIds.contains(contextUserId) && !newUserIds.contains(contextUserId);

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
						m_invitationManagerCompPtr->CreateInvitation(contextUserId, addUserId, tenantId, QByteArray());
					}
				}
			}
		} else if (isSelfLeaving){
			// Non-admin member can only remove themselves
			m_membershipManagerCompPtr->RemoveMembership(userIdToMembershipId.value(contextUserId));
		}

		// Apply role updates from memberRoles (only Creator/Owner/Admin can change roles)
		if ((isCreator || isOwner || isAdmin) && tenantData.memberRoles){
			// Rebuild userIdToMembershipId after additions
			QMap<QByteArray, QByteArray> updatedUserIdToMembershipId;
			QByteArrayList updatedMembershipIds = m_membershipManagerCompPtr->GetMembershipsByTenant(tenantId);
			for (const QByteArray& membershipId : updatedMembershipIds){
				imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
				if (membershipPtr.IsValid()){
					updatedUserIdToMembershipId[membershipPtr->GetUserId()] = membershipId;
				}
			}

			QByteArray ownerId = tenantPtr->GetOwnerId();
			QByteArray creatorId = tenantPtr->GetCreatorId();
			for (const auto& roleEntry : *tenantData.memberRoles){
				if (roleEntry->userId && roleEntry->role){
					QByteArray userId = *roleEntry->userId;
					// Creator and Owner roles are immutable — skip any attempt to change them
					if (userId == creatorId || userId == ownerId){
						continue;
					}
					QString roleStr = *roleEntry->role;
					if (updatedUserIdToMembershipId.contains(userId)){
						QByteArray newRoleId = roleStr.toUtf8();
						m_membershipManagerCompPtr->UpdateMembershipRole(updatedUserIdToMembershipId.value(userId), newRoleId);
					}
				}
			}
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
					m_membershipManagerCompPtr->AddMembership(ownerId, objectId, QByteArray());
				}
			}
		}
	}

	return true;
}


} // namespace imtauthgql

