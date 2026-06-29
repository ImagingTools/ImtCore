// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantNotificationPublisherComp.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/TenantMemberships.h>

// Qt includes
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutexLocker>

// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtgql::IGqlSubscriberController)

bool CTenantNotificationPublisherComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	if (m_commandIdsAttrPtr.IsValid()){
		QByteArray commandId = m_commandIdsAttrPtr[0];
		if (!commandId.isEmpty() && gqlRequest.GetCommandId() == commandId){
			return true;
		}
	}
	return BaseClass::IsRequestSupported(gqlRequest);
}


// reimplemented (icomp::CComponentBase)

void CTenantNotificationPublisherComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	// Initialize the membership cache so that subsequent OnUpdate() calls can
	// detect NEW memberships (those not present in the cache).
	if (m_membershipManagerCompPtr.IsValid() && m_tenantManagerCompPtr.IsValid()){
		QMutexLocker locker(&m_cacheMutex);
		QByteArrayList tenantIds = m_tenantManagerCompPtr->GetTenantIds();
		for (const QByteArray& tenantId : tenantIds){
			QByteArrayList membershipIds = m_membershipManagerCompPtr->GetMembershipsByTenant(tenantId);
			for (const QByteArray& membershipId : membershipIds){
				imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
				if (membershipPtr.IsValid()){
					CachedMembership entry;
					entry.userId = membershipPtr->GetUserId();
					entry.tenantId = membershipPtr->GetTenantId();
					entry.roleId = membershipPtr->GetRoleId();
					entry.isActive = membershipPtr->IsActive();
					m_cachedMemberships.insert(membershipId, entry);
				}
			}

			// Also cache tenant owner
			imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);
			if (tenantPtr.IsValid()){
				CachedTenantOwner ownerEntry;
				ownerEntry.ownerId = tenantPtr->GetOwnerId();
				m_cachedTenantOwners.insert(tenantId, ownerEntry);
			}
		}
	}

	// Initialize the invitation cache
	if (m_invitationManagerCompPtr.IsValid() && m_tenantManagerCompPtr.IsValid()){
		QMutexLocker locker(&m_cacheMutex);
		QByteArrayList tenantIds = m_tenantManagerCompPtr->GetTenantIds();
		for (const QByteArray& tenantId : tenantIds){
			QByteArrayList invitationIds = m_invitationManagerCompPtr->GetInvitationsByTenant(tenantId);
			for (const QByteArray& invitationId : invitationIds){
				imtauth::ITenantInvitationUniquePtr invitationPtr = m_invitationManagerCompPtr->GetInvitation(invitationId);
				if (invitationPtr.IsValid()){
					CachedInvitation entry;
					entry.userId = invitationPtr->GetUserId();
					entry.tenantId = invitationPtr->GetTenantId();
					entry.roleId = invitationPtr->GetRoleId();
					entry.invitedByUserId = invitationPtr->GetInvitedByUserId();
					entry.status = invitationPtr->GetStatus();
					m_cachedInvitations.insert(invitationId, entry);
				}
			}
		}
	}

	if (m_invitationManagerModelCompPtr.IsValid()){
		RegisterModel(m_invitationManagerModelCompPtr.GetPtr(), 0);
	}

	if (m_tenantManagerModelCompPtr.IsValid()){
		RegisterModel(m_tenantManagerModelCompPtr.GetPtr(), 1);
	}

	if (m_membershipManagerModelCompPtr.IsValid()){
		RegisterModel(m_membershipManagerModelCompPtr.GetPtr(), 2);
	}
}


void CTenantNotificationPublisherComp::OnComponentDestroyed()
{
	BaseClass2::UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CSingleModelObserverBase)

void CTenantNotificationPublisherComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet & changeSet)
{
	struct PendingNotification
	{
		QByteArray targetUserId;
		sdl::V1_0::imtauth::EMembershipNotificationType notificationType;
		QByteArray membershipId;
		QByteArray userId;
		QByteArray tenantId;
		QString tenantName;
		QByteArray roleId;
	};

	QList<PendingNotification> pendingNotifications;

	{
		QMutexLocker locker(&m_cacheMutex);

		// --- Handle membership changes ---
		if (m_membershipManagerCompPtr.IsValid()){

			QMap<QByteArray, CachedMembership> currentState;

			// Check all previously known memberships for changes or removal.
			for (auto it = m_cachedMemberships.constBegin(); it != m_cachedMemberships.constEnd(); ++it){
				const QByteArray& membershipId = it.key();
				imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
				if (membershipPtr.IsValid()){
					CachedMembership current;
					current.userId = membershipPtr->GetUserId();
					current.tenantId = membershipPtr->GetTenantId();
					current.roleId = membershipPtr->GetRoleId();
					current.isActive = membershipPtr->IsActive();
					currentState.insert(membershipId, current);
				}
			}

			// Detect state transitions.
			for (auto it = m_cachedMemberships.constBegin(); it != m_cachedMemberships.constEnd(); ++it){
				const QByteArray& membershipId = it.key();
				const CachedMembership& cached = it.value();

				if (!currentState.contains(membershipId)){
					// Membership was removed — if it was inactive, this is a rejection.
					if (!cached.isActive){
						QByteArray ownerUserId = FindTenantOwnerUserId(cached.tenantId);
						if (!ownerUserId.isEmpty()){
							pendingNotifications.append({
								ownerUserId,
								sdl::V1_0::imtauth::EMembershipNotificationType::InvitationRejected,
								membershipId,
								cached.userId,
								cached.tenantId,
								QString(),
								cached.roleId});
						}
					}
					else{
						// Active membership was removed → notify the affected user
						// (e.g. the admin/owner removed them) so their UI can react.
						if (!cached.userId.isEmpty()){
							pendingNotifications.append({
								cached.userId,
								sdl::V1_0::imtauth::EMembershipNotificationType::MembershipRemoved,
								membershipId,
								cached.userId,
								cached.tenantId,
								QString(),
								cached.roleId});
						}
						// Also notify the tenant owner so the members view updates.
						QByteArray ownerUserId = FindTenantOwnerUserId(cached.tenantId);
						if (!ownerUserId.isEmpty() && ownerUserId != cached.userId){
							pendingNotifications.append({
								ownerUserId,
								sdl::V1_0::imtauth::EMembershipNotificationType::MembershipRemoved,
								membershipId,
								cached.userId,
								cached.tenantId,
								QString(),
								cached.roleId});
						}
					}
				}
				else{
					const CachedMembership& current = currentState.value(membershipId);
					// Was inactive, now active → accepted.
					if (!cached.isActive && current.isActive){
						QByteArray ownerUserId = FindTenantOwnerUserId(current.tenantId);
						if (!ownerUserId.isEmpty()){
							pendingNotifications.append({
								ownerUserId,
								sdl::V1_0::imtauth::EMembershipNotificationType::InvitationAccepted,
								membershipId,
								current.userId,
								current.tenantId,
								QString(),
								current.roleId});
						}
					}
					// Role changed on an active membership → notify the affected
					// user AND the tenant owner so both sides refresh their UI.
					if (cached.isActive && current.isActive && cached.roleId != current.roleId){
						if (!current.userId.isEmpty()){
							pendingNotifications.append({
								current.userId,
								sdl::V1_0::imtauth::EMembershipNotificationType::MembershipRoleChanged,
								membershipId,
								current.userId,
								current.tenantId,
								QString(),
								current.roleId});
						}
						QByteArray ownerUserId = FindTenantOwnerUserId(current.tenantId);
						if (!ownerUserId.isEmpty() && ownerUserId != current.userId){
							pendingNotifications.append({
								ownerUserId,
								sdl::V1_0::imtauth::EMembershipNotificationType::MembershipRoleChanged,
								membershipId,
								current.userId,
								current.tenantId,
								QString(),
								current.roleId});
						}
					}
				}
			}

			// Detect new memberships (not in cache).
			QSet<QByteArray> tenantsToCheck;
			for (auto it = currentState.constBegin(); it != currentState.constEnd(); ++it){
				tenantsToCheck.insert(it.value().tenantId);
			}
			for (auto it = m_cachedMemberships.constBegin(); it != m_cachedMemberships.constEnd(); ++it){
				tenantsToCheck.insert(it.value().tenantId);
			}

			for (const QByteArray& tenantId : std::as_const(tenantsToCheck)){
				QByteArrayList membershipIds = m_membershipManagerCompPtr->GetMembershipsByTenant(tenantId);
				for (const QByteArray& membershipId : std::as_const(membershipIds)){
					if (!m_cachedMemberships.contains(membershipId)){
						// New membership detected.
						imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
						if (membershipPtr.IsValid()){
							CachedMembership newEntry;
							newEntry.userId = membershipPtr->GetUserId();
							newEntry.tenantId = membershipPtr->GetTenantId();
							newEntry.roleId = membershipPtr->GetRoleId();
							newEntry.isActive = membershipPtr->IsActive();
							currentState.insert(membershipId, newEntry);
						}
					}
				}
			}

			// Update cache to current state.
			m_cachedMemberships = currentState;
		}

		// --- Handle invitation changes ---
		if (m_invitationManagerCompPtr.IsValid()){

			QMap<QByteArray, CachedInvitation> currentInvitations;

			// Collect all known tenants to scan for invitations
			QSet<QByteArray> tenantsToCheck;
			for (auto it = m_cachedInvitations.constBegin(); it != m_cachedInvitations.constEnd(); ++it){
				tenantsToCheck.insert(it.value().tenantId);
			}
			for (auto it = m_cachedMemberships.constBegin(); it != m_cachedMemberships.constEnd(); ++it){
				tenantsToCheck.insert(it.value().tenantId);
			}
			if (m_tenantManagerCompPtr.IsValid()){
				QByteArrayList allTenantIds = m_tenantManagerCompPtr->GetTenantIds();
				for (const QByteArray& tid : allTenantIds){
					tenantsToCheck.insert(tid);
				}
			}

			for (const QByteArray& tenantId : std::as_const(tenantsToCheck)){
				QByteArrayList invitationIds = m_invitationManagerCompPtr->GetInvitationsByTenant(tenantId);
				for (const QByteArray& invitationId : std::as_const(invitationIds)){
					imtauth::ITenantInvitationUniquePtr invitationPtr = m_invitationManagerCompPtr->GetInvitation(invitationId);
					if (invitationPtr.IsValid()){
						CachedInvitation entry;
						entry.userId = invitationPtr->GetUserId();
						entry.tenantId = invitationPtr->GetTenantId();
						entry.roleId = invitationPtr->GetRoleId();
						entry.invitedByUserId = invitationPtr->GetInvitedByUserId();
						entry.status = invitationPtr->GetStatus();
						currentInvitations.insert(invitationId, entry);
					}
				}
			}

			// Detect new invitations (not in cache)
			for (auto it = currentInvitations.constBegin(); it != currentInvitations.constEnd(); ++it){
				const QByteArray& invitationId = it.key();
				const CachedInvitation& current = it.value();

				if (!m_cachedInvitations.contains(invitationId)){
					// New invitation — notify the invited user
					if (current.status == imtauth::ITenantInvitation::TIS_PENDING){
						QString tenantName;
						if (m_tenantManagerCompPtr.IsValid()){
							imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(current.tenantId);
							if (tenantPtr.IsValid()){
								tenantName = tenantPtr->GetTenantName();
							}
						}
						pendingNotifications.append({
							current.userId,
							sdl::V1_0::imtauth::EMembershipNotificationType::InvitationReceived,
							invitationId,
							current.userId,
							current.tenantId,
							tenantName,
							current.roleId});
					}
				}
				else{
					const CachedInvitation& cached = m_cachedInvitations.value(invitationId);
					// Status transition detection
					if (cached.status != current.status){
						QByteArray ownerUserId = FindTenantOwnerUserId(current.tenantId);
						QString tenantName;
						if (m_tenantManagerCompPtr.IsValid()){
							imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(current.tenantId);
							if (tenantPtr.IsValid()){
								tenantName = tenantPtr->GetTenantName();
							}
						}

						if (current.status == imtauth::ITenantInvitation::TIS_ACCEPTED){
							// Notify owner that invitation was accepted
							if (!ownerUserId.isEmpty()){
								pendingNotifications.append({
									ownerUserId,
									sdl::V1_0::imtauth::EMembershipNotificationType::InvitationAccepted,
									invitationId,
									current.userId,
									current.tenantId,
									tenantName,
									current.roleId});
							}
							// Also notify the inviter (when different from owner)
							// so admins who created the invitation see the result.
							if (!current.invitedByUserId.isEmpty()
									&& current.invitedByUserId != ownerUserId){
								pendingNotifications.append({
									current.invitedByUserId,
									sdl::V1_0::imtauth::EMembershipNotificationType::InvitationAccepted,
									invitationId,
									current.userId,
									current.tenantId,
									tenantName,
									current.roleId});
							}
						}
						else if (current.status == imtauth::ITenantInvitation::TIS_REJECTED){
							// Notify owner that invitation was rejected
							if (!ownerUserId.isEmpty()){
								pendingNotifications.append({
									ownerUserId,
									sdl::V1_0::imtauth::EMembershipNotificationType::InvitationRejected,
									invitationId,
									current.userId,
									current.tenantId,
									tenantName,
									current.roleId});
							}
							// Also notify the inviter (when different from owner).
							if (!current.invitedByUserId.isEmpty()
									&& current.invitedByUserId != ownerUserId){
								pendingNotifications.append({
									current.invitedByUserId,
									sdl::V1_0::imtauth::EMembershipNotificationType::InvitationRejected,
									invitationId,
									current.userId,
									current.tenantId,
									tenantName,
									current.roleId});
							}
						}
						else if (current.status == imtauth::ITenantInvitation::TIS_REVOKED){
							// Notify the invitee that the invitation was revoked by the sender
							pendingNotifications.append({
								current.userId,
								sdl::V1_0::imtauth::EMembershipNotificationType::InvitationRevoked,
								invitationId,
								current.userId,
								current.tenantId,
								tenantName,
								current.roleId});
						}
					}
				}
			}

			// Update invitation cache
			m_cachedInvitations = currentInvitations;
		}

		// --- Handle ownership changes ---
		if (m_tenantManagerCompPtr.IsValid()){

			QSet<QByteArray> tenantIds;
			for (auto it = m_cachedMemberships.constBegin(); it != m_cachedMemberships.constEnd(); ++it){
				tenantIds.insert(it.value().tenantId);
			}

			for (const QByteArray& tenantId : std::as_const(tenantIds)){
				imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);
				if (!tenantPtr.IsValid()){
					continue;
				}

				QByteArray currentOwnerId = tenantPtr->GetOwnerId();

				if (m_cachedTenantOwners.contains(tenantId)){
					const CachedTenantOwner& cached = m_cachedTenantOwners.value(tenantId);
					if (cached.ownerId != currentOwnerId){
						// Ownership has changed — notify both old and new owner.
						QString tenantName = tenantPtr->GetTenantName();

						// Notify the old owner
						if (!cached.ownerId.isEmpty()){
							pendingNotifications.append({
								cached.ownerId,
								sdl::V1_0::imtauth::EMembershipNotificationType::OwnershipTransferred,
								QByteArray(),
								currentOwnerId,
								tenantId,
								tenantName,
								QByteArray()});
						}

						// Notify the new owner
						if (!currentOwnerId.isEmpty()){
							pendingNotifications.append({
								currentOwnerId,
								sdl::V1_0::imtauth::EMembershipNotificationType::OwnershipTransferred,
								QByteArray(),
								currentOwnerId,
								tenantId,
								tenantName,
								QByteArray()});
						}
					}
				}

				// Update cache
				CachedTenantOwner ownerEntry;
				ownerEntry.ownerId = currentOwnerId;
				m_cachedTenantOwners.insert(tenantId, ownerEntry);
			}
		}
	}

	// Publish notifications outside the cache lock to avoid potential deadlocks
	// with the base class m_mutex used by PublishDataFiltered.
	for (const PendingNotification& notification : std::as_const(pendingNotifications)){
		PublishNotification(
			notification.targetUserId,
			notification.notificationType,
			notification.membershipId,
			notification.userId,
			notification.tenantId,
			notification.tenantName,
			notification.roleId);
	}
}


// private methods

void CTenantNotificationPublisherComp::PublishNotification(
	const QByteArray& targetUserId,
	sdl::V1_0::imtauth::EMembershipNotificationType notificationType,
	const QByteArray& membershipId,
	const QByteArray& userId,
	const QByteArray& tenantId,
	const QString& tenantName,
	const QByteArray& roleId) const
{
	sdl::V1_0::imtauth::CMembershipNotification notification;
	notification.notificationType = notificationType;
	notification.membershipId = membershipId;
	notification.userId = userId;
	notification.tenantId = tenantId;
	notification.tenantName = tenantName;
	notification.role = roleId;

	QJsonObject jsonObject;
	if (!notification.WriteToJsonObject(jsonObject)){
		Q_ASSERT(false);
		return;
	}

	QJsonDocument jsonDoc;
	jsonDoc.setObject(jsonObject);
	QByteArray data = jsonDoc.toJson(QJsonDocument::Compact);

	const QByteArray commandId = m_commandIdsAttrPtr.IsValid() && m_commandIdsAttrPtr.GetCount() > 0
			? m_commandIdsAttrPtr[0]
			: QByteArray("OnMembershipNotification");

	// Filter: only push to subscribers whose context userId matches targetUserId
	PublishDataFiltered(commandId, data, [targetUserId](const imtgql::CGqlRequest& gqlRequest) -> bool {
		const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
		if (contextPtr != nullptr){
			const QByteArray contextUserId = contextPtr->GetUserId();
			return contextUserId == targetUserId;
		}
		return false;
	});
}


QByteArray CTenantNotificationPublisherComp::FindTenantOwnerUserId(const QByteArray& tenantId) const
{
	if (!m_tenantManagerCompPtr.IsValid() || tenantId.isEmpty()){
		return QByteArray();
	}

	imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);
	if (tenantPtr.IsValid()){
		return tenantPtr->GetOwnerId();
	}

	return QByteArray();
}


} // namespace imtauthgql

