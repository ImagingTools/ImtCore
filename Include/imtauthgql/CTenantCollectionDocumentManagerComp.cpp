// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantCollectionDocumentManagerComp.h>


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
	if (tenantId.isEmpty()){
		tenantId = objectId;
	}

	response.Version_1_0->id = tenantId;
	response.Version_1_0->name = tenantPtr->GetTenantName();
	response.Version_1_0->description = tenantPtr->GetTenantDescription();
	response.Version_1_0->ownerId = tenantPtr->GetOwnerId();
	response.Version_1_0->isActive = tenantPtr->IsActive();
	response.Version_1_0->createdAt = tenantPtr->GetCreatedAt();
	response.Version_1_0->updatedAt = tenantPtr->GetUpdatedAt();
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

	response.Version_1_0->availableRoles.Emplace();
	static const char* roleNames[] = {"Owner", "Admin", "Member", "Guest"};
	for (const char* roleName : roleNames){
		sdl::imtauth::Tenants::CTenantRoleOption::V1_0 opt;
		opt.id = QByteArray(roleName);
		opt.name = QString::fromLatin1(roleName);
		response.Version_1_0->availableRoles->push_back(opt);
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

	if (tenantData.name){
		tenantPtr->SetTenantName(*tenantData.name);
	}

	if (tenantData.description){
		tenantPtr->SetTenantDescription(*tenantData.description);
	}

	// OwnerId is set once on first save — the creator is the owner (immutable).
	if (tenantPtr->GetOwnerId().isEmpty()){
		tenantPtr->SetOwnerId(contextUserId);
	}

	if (tenantData.isActive){
		tenantPtr->SetActive(*tenantData.isActive);
	}

	if (!tenantId.isEmpty() && tenantData.members && m_membershipManagerCompPtr.IsValid()){
		QByteArrayList currentMembershipIds = m_membershipManagerCompPtr->GetMembershipsByTenant(tenantId);
		QMap<QByteArray, QByteArray> userIdToMembershipId;
		QSet<QByteArray> currentUserIds;
		for (const QByteArray& membershipId : currentMembershipIds){
			imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
			if (membershipPtr.IsValid()){
				currentUserIds.insert(membershipPtr->GetUserId());
				userIdToMembershipId[membershipPtr->GetUserId()] = membershipId;
			}
		}

		QSet<QByteArray> newUserIds;
		for (const auto& memberEntry : *tenantData.members){
			if (memberEntry->id){
				newUserIds.insert(*memberEntry->id);
			}
		}

		for (const QByteArray& existingUserId : currentUserIds){
			if (!newUserIds.contains(existingUserId)){
				m_membershipManagerCompPtr->RemoveMembership(userIdToMembershipId.value(existingUserId));
			}
		}

		for (const QByteArray& addUserId : newUserIds){
			if (!currentUserIds.contains(addUserId)){
				m_membershipManagerCompPtr->InviteMembership(addUserId, tenantId, imtauth::ITenantMembership::TMR_MEMBER);
			}
		}

		// Apply role updates from memberRoles
		if (tenantData.memberRoles){
			// Rebuild userIdToMembershipId after additions
			QMap<QByteArray, QByteArray> updatedUserIdToMembershipId;
			QByteArrayList updatedMembershipIds = m_membershipManagerCompPtr->GetMembershipsByTenant(tenantId);
			for (const QByteArray& membershipId : updatedMembershipIds){
				imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
				if (membershipPtr.IsValid()){
					updatedUserIdToMembershipId[membershipPtr->GetUserId()] = membershipId;
				}
			}

			for (const auto& roleEntry : *tenantData.memberRoles){
				if (roleEntry->userId && roleEntry->role){
					QByteArray userId = *roleEntry->userId;
					QString roleStr = *roleEntry->role;
					if (updatedUserIdToMembershipId.contains(userId)){
						imtauth::ITenantMembership::TenantMemberRole newRole = StringToTenantMemberRole(roleStr);
						m_membershipManagerCompPtr->UpdateMembershipRole(updatedUserIdToMembershipId.value(userId), newRole);
					}
				}
			}
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
			}
		}
	}

	return true;
}


} // namespace imtauthgql


