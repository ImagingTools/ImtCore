// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantCollectionDocumentServiceComp.h>


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

// reimplemented (CGraphQlHandlerCompBase)

sdl::imtauth::Tenants::CTenantData CTenantCollectionDocumentServiceComp::OnGetTenantRepresentation(
		const sdl::imtauth::TenantCollectionDocumentService::CGetTenantRepresentationGqlRequest& getTenantRepresentationRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	sdl::imtauth::TenantCollectionDocumentService::GetTenantRepresentationRequestArguments arguments = getTenantRepresentationRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtauth::Tenants::CTenantData();
	}

	QByteArray userId = GetUserId(gqlRequest);

	QByteArray objectId;
	if (arguments.input.Version_1_0->id){
		objectId = *arguments.input.Version_1_0->id;
	}

	if (objectId.isEmpty()){
		errorMessage = QStringLiteral("Missing document ID");
		return sdl::imtauth::Tenants::CTenantData();
	}

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userId, objectId, documentPtr);
	if (!documentPtr.IsValid()){
		errorMessage = QStringLiteral("Document not found");
		return sdl::imtauth::Tenants::CTenantData();
	}

	const imtauth::ITenantInfo* tenantPtr = dynamic_cast<const imtauth::ITenantInfo*>(documentPtr.GetPtr());
	if (tenantPtr == nullptr){
		errorMessage = QStringLiteral("Invalid document type");
		return sdl::imtauth::Tenants::CTenantData();
	}

	sdl::imtauth::Tenants::CTenantData response;
	response.Version_1_0.Emplace();

	QByteArray tenantId = tenantPtr->GetTenantId();

	response.Version_1_0->id = tenantId;
	response.Version_1_0->name = tenantPtr->GetTenantName();
	response.Version_1_0->description = tenantPtr->GetTenantDescription();
	response.Version_1_0->ownerId = tenantPtr->GetOwnerId();
	response.Version_1_0->creatorId = tenantPtr->GetCreatorId();
	response.Version_1_0->isActive = tenantPtr->IsActive();
	response.Version_1_0->createdAt = tenantPtr->GetCreatedAt();
	response.Version_1_0->updatedAt = tenantPtr->GetUpdatedAt();

	// Pass current user ID so GUI can determine role-based access
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		response.Version_1_0->currentUserId = gqlContextPtr->GetUserId();
	}
	if (m_membershipManagerCompPtr.IsValid()){
		QByteArrayList membershipIds = m_membershipManagerCompPtr->GetMembershipsByTenant(tenantId);
		response.Version_1_0->members.Emplace();
		response.Version_1_0->memberRoles.Emplace();

		QByteArray ownerId = tenantPtr->GetOwnerId();
		QByteArray creatorId = tenantPtr->GetCreatorId();

		for (const QByteArray& membershipId : membershipIds){
			imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
			if (membershipPtr.IsValid() && membershipPtr->IsActive()){
				QByteArray userId = membershipPtr->GetUserId();

				sdl::imtauth::Tenants::CTenantMemberEntry::V1_0 memberEntry;
				memberEntry.id = userId;

				if (m_userCollectionCompPtr.IsValid()){
					memberEntry.name = imtauth::GetUserName(*m_userCollectionCompPtr, userId);
				}

				response.Version_1_0->members->push_back(memberEntry);

				// Assign environment role: Creator > Owner > stored role (Admin/Member)
				sdl::imtauth::Tenants::CTenantMemberRoleEntry::V1_0 roleEntry;
				roleEntry.userId = userId;
				if (!creatorId.isEmpty() && userId == creatorId){
					roleEntry.role = QStringLiteral("Creator");
				} else if (!ownerId.isEmpty() && userId == ownerId){
					roleEntry.role = QStringLiteral("Owner");
				} else {
					QString storedRole = QString::fromUtf8(membershipPtr->GetRoleId());
					roleEntry.role = storedRole.isEmpty() ? QStringLiteral("Member") : storedRole;
				}
				response.Version_1_0->memberRoles->push_back(roleEntry);
			}
		}
	}

	if (m_invitationManagerCompPtr.IsValid()){
		imtauth::ITenantInvitationManager::Statuses statuses;
		statuses.append(imtauth::ITenantInvitation::TIS_PENDING);
		QByteArrayList invitationIds = m_invitationManagerCompPtr->GetInvitationsByTenant(tenantId, statuses);
		response.Version_1_0->pendingInvitations.Emplace();
		for (const QByteArray& invitationId : invitationIds){
			imtauth::ITenantInvitationUniquePtr invitationPtr = m_invitationManagerCompPtr->GetInvitation(invitationId);
			if (invitationPtr.IsValid()){
				sdl::imtauth::Tenants::CTenantInvitationEntry::V1_0 invitationEntry;
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

				response.Version_1_0->pendingInvitations->push_back(invitationEntry);
			}
		}
	}

	// availableRoles will be provided by the client-side RoleCollectionDataProvider

	// Tenant-scoped permissions (selected subset of product permissions)
	response.Version_1_0->tenantPermissions.Emplace().FromList(tenantPtr->GetTenantPermissions());

	return response;
}


sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus CTenantCollectionDocumentServiceComp::OnUpdateTenantFromRepresentation(
		const sdl::imtauth::TenantCollectionDocumentService::CUpdateTenantFromRepresentationGqlRequest& updateTenantFromRepresentationRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	sdl::imtauth::TenantCollectionDocumentService::UpdateTenantFromRepresentationRequestArguments arguments = updateTenantFromRepresentationRequest.GetRequestedArguments();
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

	QByteArray contextUserId;
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		contextUserId = gqlContextPtr->GetUserId();
	}

	QByteArray userLogin = GetUserId(gqlRequest);

	istd::IChangeableSharedPtr documentPtr;
	m_documentManagerCompPtr->GetDocumentData(userLogin, documentId, documentPtr);
	if (!documentPtr.IsValid()){
		response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	imtauth::ITenantInfo* tenantPtr = dynamic_cast<imtauth::ITenantInfo*>(documentPtr.GetPtr());
	if (tenantPtr == nullptr){
		response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	sdl::imtauth::Tenants::CTenantData::V1_0 tenantData;
	if (arguments.input.Version_1_0->tenant){
		tenantData = *arguments.input.Version_1_0->tenant;
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
			QByteArray roleId = membershipPtr->GetRoleId();
			isAdmin = isOwner || isCreator || (roleId == "Admin");
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
			for (const sdl::imtauth::Tenants::CTenantMemberEntry::V1_0& member : tenantData.members->ToList()){
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
		m_documentManagerCompPtr->SaveDocument(userLogin, documentId);
	}

	response.Version_1_0->status = sdl::imtbase::CollectionDocumentService::EDocumentOperationStatus::Success;

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
				m_documentManagerCompPtr->SaveDocument(userId, documentId);

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

