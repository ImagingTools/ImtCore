// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantMembershipPublisherComp.h>

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

bool CTenantMembershipPublisherComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
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

void CTenantMembershipPublisherComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_membershipManagerModelCompPtr.IsValid()){
		m_membershipManagerModelCompPtr->AttachObserver(this);
	}

	if (m_tenantManagerModelCompPtr.IsValid()){
		m_tenantManagerModelCompPtr->AttachObserver(this);
	}
}


void CTenantMembershipPublisherComp::OnComponentDestroyed()
{
	if (m_tenantManagerModelCompPtr.IsValid()){
		m_tenantManagerModelCompPtr->DetachObserver(this);
	}

	if (m_membershipManagerModelCompPtr.IsValid()){
		m_membershipManagerModelCompPtr->DetachObserver(this);
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CSingleModelObserverBase)

void CTenantMembershipPublisherComp::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	struct PendingNotification
	{
		QByteArray targetUserId;
		sdl::imtauth::TenantMemberships::EMembershipNotificationType notificationType;
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

			QSet<QByteArray> currentMembershipIds;
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
					currentMembershipIds.insert(membershipId);
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
								sdl::imtauth::TenantMemberships::EMembershipNotificationType::InvitationRejected,
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
								sdl::imtauth::TenantMemberships::EMembershipNotificationType::InvitationAccepted,
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

							// New inactive membership = invitation.
							if (!newEntry.isActive){
								pendingNotifications.append({
									newEntry.userId,
									sdl::imtauth::TenantMemberships::EMembershipNotificationType::InvitationReceived,
									membershipId,
									newEntry.userId,
									newEntry.tenantId,
									QString(),
									newEntry.roleId});
							}
						}
					}
				}
			}

			// Update cache to current state.
			m_cachedMemberships = currentState;
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
								sdl::imtauth::TenantMemberships::EMembershipNotificationType::OwnershipTransferred,
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
								sdl::imtauth::TenantMemberships::EMembershipNotificationType::OwnershipTransferred,
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

void CTenantMembershipPublisherComp::PublishNotification(
	const QByteArray& targetUserId,
	sdl::imtauth::TenantMemberships::EMembershipNotificationType notificationType,
	const QByteArray& membershipId,
	const QByteArray& userId,
	const QByteArray& tenantId,
	const QString& tenantName,
	const QByteArray& roleId) const
{
	sdl::imtauth::TenantMemberships::CMembershipNotification notification;
	notification.Version_1_0.emplace();
	notification.Version_1_0->notificationType = notificationType;
	notification.Version_1_0->membershipId = membershipId;
	notification.Version_1_0->userId = userId;
	notification.Version_1_0->tenantId = tenantId;
	notification.Version_1_0->tenantName = tenantName;
	notification.Version_1_0->role = roleId;

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
			return contextPtr->GetUserId() == targetUserId;
		}
		return false;
	});
}


QByteArray CTenantMembershipPublisherComp::FindTenantOwnerUserId(const QByteArray& tenantId) const
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

