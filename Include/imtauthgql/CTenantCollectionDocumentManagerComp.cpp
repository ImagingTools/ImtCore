// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantCollectionDocumentManagerComp.h>


// Qt includes
#include <QMap>
#include <QSet>

// ImtCore includes
#include <imtauth/ITenantInfo.h>
#include <imtauth/IUserManager.h>
#include <imtbase/ICollectionInfo.h>
#include <imtgql/IGqlContext.h>
#include <imtdoc/CDocumentSavedEvent.h>
#include <imtauth/imtauth.h>


namespace
{


QString TenantMemberRoleToString(imtauth::ITenantMembership::TenantMemberRole role)
{
	switch (role){
		case imtauth::ITenantMembership::TMR_OWNER:
			return QStringLiteral("Owner");
		case imtauth::ITenantMembership::TMR_ADMIN:
			return QStringLiteral("Admin");
		case imtauth::ITenantMembership::TMR_MEMBER:
			return QStringLiteral("Member");
		case imtauth::ITenantMembership::TMR_GUEST:
			return QStringLiteral("Guest");
		default:
			return QStringLiteral("Member");
	}
}


imtauth::ITenantMembership::TenantMemberRole StringToTenantMemberRole(const QString& role)
{
	if (role == QStringLiteral("Owner")){
		return imtauth::ITenantMembership::TMR_OWNER;
	}
	if (role == QStringLiteral("Admin")){
		return imtauth::ITenantMembership::TMR_ADMIN;
	}
	if (role == QStringLiteral("Guest")){
		return imtauth::ITenantMembership::TMR_GUEST;
	}
	return imtauth::ITenantMembership::TMR_MEMBER;
}


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

sdl::imtauth::Tenants::CTenantData CTenantCollectionDocumentManagerComp::OnGetTenantRepresentation(
		const sdl::imtauth::TenantCollectionDocumentManager::CGetTenantRepresentationGqlRequest& getTenantRepresentationRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	sdl::imtauth::TenantCollectionDocumentManager::GetTenantRepresentationRequestArguments arguments = getTenantRepresentationRequest.GetRequestedArguments();
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

				sdl::imtauth::Tenants::CTenantMemberRoleEntry::V1_0 roleEntry;
				roleEntry.userId = userId;
				roleEntry.role = TenantMemberRoleToString(membershipPtr->GetRole());
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
				invitationEntry.role = TenantMemberRoleToString(invitationPtr->GetRole());
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

	response.Version_1_0->availableRoles.Emplace();
	static const char* roleNames[] = {"Admin", "Member", "Guest"};
	for (const char* roleName : roleNames){
		sdl::imtauth::Tenants::CTenantRoleOption::V1_0 opt;
		opt.id = QByteArray(roleName);
		opt.name = QString::fromLatin1(roleName);
		response.Version_1_0->availableRoles->push_back(opt);
	}

	// Tenant-scoped permissions (selected subset of product permissions)
	response.Version_1_0->tenantPermissions.Emplace().FromList(tenantPtr->GetTenantPermissions());

	// All available product permissions for selection UI
	response.Version_1_0->allProductPermissions.Emplace();
	if (m_featureInfoProviderCompPtr.IsValid()){
		const imtbase::ICollectionInfo& featureList = m_featureInfoProviderCompPtr->GetFeatureList();
		imtbase::ICollectionInfo::Ids featureIds = featureList.GetElementIds();
		for (const QByteArray& featureId : featureIds){
			imtlic::IFeatureInfoSharedPtr featurePtr = m_featureInfoProviderCompPtr->GetFeatureInfo(featureId);
			if (featurePtr.IsValid() && featurePtr->IsPermission()){
				sdl::imtauth::Tenants::CTenantPermissionOption::V1_0 permOpt;
				permOpt.id = featurePtr->GetFeatureId();
				permOpt.name = featurePtr->GetFeatureName();
				permOpt.description = featurePtr->GetFeatureDescription();
				response.Version_1_0->allProductPermissions->push_back(permOpt);
			}
		}
	}

	return response;
}


sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus CTenantCollectionDocumentManagerComp::OnUpdateTenantFromRepresentation(
		const sdl::imtauth::TenantCollectionDocumentManager::CUpdateTenantFromRepresentationGqlRequest& updateTenantFromRepresentationRequest,
		const ::imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	sdl::imtauth::TenantCollectionDocumentManager::UpdateTenantFromRepresentationRequestArguments arguments = updateTenantFromRepresentationRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus();
	}

	sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus response;
	response.Version_1_0.Emplace();
	response.Version_1_0->status = sdl::imtbase::CollectionDocumentManager::EDocumentOperationStatus::Failed;

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
		response.Version_1_0->status = sdl::imtbase::CollectionDocumentManager::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	imtauth::ITenantInfo* tenantPtr = dynamic_cast<imtauth::ITenantInfo*>(documentPtr.GetPtr());
	if (tenantPtr == nullptr){
		response.Version_1_0->status = sdl::imtbase::CollectionDocumentManager::EDocumentOperationStatus::InvalidDocumentId;
		return response;
	}

	sdl::imtauth::Tenants::CTenantData::V1_0 tenantData;
	if (arguments.input.Version_1_0->tenant){
		tenantData = *arguments.input.Version_1_0->tenant;
	}

	QByteArray tenantId = tenantPtr->GetTenantId();

	// Role-based access control for existing tenants
	bool isOwner = !tenantId.isEmpty() && (tenantPtr->GetOwnerId() == contextUserId);
	bool isAdmin = false;
	bool isMember = false;
	if (!tenantId.isEmpty() && m_membershipManagerCompPtr.IsValid()){
		isAdmin = m_membershipManagerCompPtr->HasMinimumRole(contextUserId, tenantId, imtauth::ITenantMembership::TMR_ADMIN);
		isMember = m_membershipManagerCompPtr->HasMinimumRole(contextUserId, tenantId, imtauth::ITenantMembership::TMR_GUEST);
	}
	bool isNewTenant = tenantId.isEmpty();

	// Block updates from non-members (e.g., invited users who haven't accepted yet)
	if (!isNewTenant && !isOwner && !isMember){
		return response;
	}

	// Only Owner (or new tenant creator) can change Name/Description/Active
	if (isOwner || isNewTenant){
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

	// OwnerId is set once on first save — the creator is the owner (immutable).
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
		bool isSelfLeaving = isMember && !isOwner && currentUserIds.contains(contextUserId) && !newUserIds.contains(contextUserId);

		if (isOwner || isAdmin || isNewTenant){
			// Owner/Admin can remove any member (except owner) and add new ones
			QByteArray ownerId = tenantPtr->GetOwnerId();
			for (const QByteArray& existingUserId : currentUserIds){
				if (!newUserIds.contains(existingUserId) && existingUserId != ownerId){
					m_membershipManagerCompPtr->RemoveMembership(userIdToMembershipId.value(existingUserId));
				}
			}

			for (const QByteArray& addUserId : newUserIds){
				if (!currentUserIds.contains(addUserId)){
					if (m_invitationManagerCompPtr.IsValid()){
						m_invitationManagerCompPtr->CreateInvitation(contextUserId, addUserId, tenantId, imtauth::ITenantMembership::TMR_MEMBER);
					}
				}
			}
		} else if (isSelfLeaving){
			// Non-admin member can only remove themselves
			m_membershipManagerCompPtr->RemoveMembership(userIdToMembershipId.value(contextUserId));
		}

		// Apply role updates from memberRoles (only Owner/Admin can change roles)
		if ((isOwner || isAdmin) && tenantData.memberRoles){
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
			for (const auto& roleEntry : *tenantData.memberRoles){
				if (roleEntry->userId && roleEntry->role){
					QByteArray userId = *roleEntry->userId;
					// Owner role is immutable — skip any attempt to change it
					if (userId == ownerId){
						continue;
					}
					QString roleStr = *roleEntry->role;
					// Owner role cannot be assigned — it is set once at creation
					if (roleStr == QStringLiteral("Owner")){
						continue;
					}
					if (updatedUserIdToMembershipId.contains(userId)){
						imtauth::ITenantMembership::TenantMemberRole newRole = StringToTenantMemberRole(roleStr);
						m_membershipManagerCompPtr->UpdateMembershipRole(updatedUserIdToMembershipId.value(userId), newRole);
					}
				}
			}
		}

		// Apply tenant permissions (only Owner/Admin)
		if ((isOwner || isAdmin) && tenantData.tenantPermissions){
			tenantPtr->SetTenantPermissions(tenantData.tenantPermissions->ToList());
		}
	}

	m_documentManagerCompPtr->SetDocumentData(userLogin, documentId, *documentPtr);

	// For new tenants do NOT auto-save — user will save manually.
	// For existing tenants, save immediately after each change (like tickets).
	if (!tenantId.isEmpty()){
		m_documentManagerCompPtr->SaveDocument(userLogin, documentId);
	}

	response.Version_1_0->status = sdl::imtbase::CollectionDocumentManager::EDocumentOperationStatus::Success;

	return response;
}


// reimplemented (imtdoc::IDocumentManagerEventHandler)

bool CTenantCollectionDocumentManagerComp::ProcessEvent(imtdoc::CEventBase* eventPtr)
{
	imtdoc::CDocumentSavedEvent* savedEventPtr = dynamic_cast<imtdoc::CDocumentSavedEvent*>(eventPtr);
	if (savedEventPtr != nullptr){
		QByteArray documentId = savedEventPtr->GetDocumentId();
		QByteArray userId = savedEventPtr->GetUserId();
		QByteArray objectId = savedEventPtr->GetDocumentUrl().toString().toUtf8().replace("collection:///", "");

		istd::IChangeableSharedPtr documentPtr;
		imtauth::ITenantInfo* documentTicketPtr = nullptr;
		if (m_documentManagerCompPtr->GetDocumentData(userId, documentId, documentPtr) == imtdoc::IDocumentManager::OS_OK){
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
					m_membershipManagerCompPtr->AddMembership(ownerId, objectId, imtauth::ITenantMembership::TMR_OWNER);
				}
			}
		}
	}

	return true;
}


} // namespace imtauthgql

