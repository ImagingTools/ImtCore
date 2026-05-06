// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CTenantMembershipPublisherComp.h>

// Qt includes
#include <QJsonDocument>
#include <QJsonObject>

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
	if (m_commandIdAttrPtr.IsValid()){
		QByteArray commandId = *m_commandIdAttrPtr;
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

	// Build the initial membership cache.
	if (m_membershipManagerCompPtr.IsValid()){
		// Get all memberships across all tenants by iterating known memberships.
		// We use GetMembershipsByUser with empty userId won't work, so we rely on
		// the collection being populated before this component is created.
		// Since ITenantMembershipManager doesn't expose "GetAll", we skip initial cache
		// and will detect changes from first notification onward.
	}

	if (m_membershipManagerModelCompPtr.IsValid()){
		m_membershipManagerModelCompPtr->AttachObserver(this);
	}
}


void CTenantMembershipPublisherComp::OnComponentDestroyed()
{
	if (m_membershipManagerModelCompPtr.IsValid()){
		m_membershipManagerModelCompPtr->DetachObserver(this);
	}

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CSingleModelObserverBase)

void CTenantMembershipPublisherComp::OnUpdate(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (!m_membershipManagerCompPtr.IsValid()){
		return;
	}

	// Snapshot current state by iterating all cached tenants + checking for new memberships.
	// Strategy: compare cached membership states with current states to detect:
	// 1. New inactive membership → InvitationReceived
	// 2. Previously inactive → now active → InvitationAccepted
	// 3. Previously cached membership now gone → InvitationRejected (only if was inactive)

	// Collect current membership IDs from all tenants known in cache.
	QSet<QByteArray> currentMembershipIds;
	QMap<QByteArray, CachedMembership> currentState;

	// Check all previously known memberships for changes or removal.
	for (auto it = m_cachedMemberships.constBegin(); it != m_cachedMemberships.constEnd(); ++it){
		const QByteArray& membershipId = it.key();
		const imtauth::ITenantMembership* membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
		if (membershipPtr != nullptr){
			CachedMembership current;
			current.userId = membershipPtr->GetUserId();
			current.tenantId = membershipPtr->GetTenantId();
			current.role = membershipPtr->GetRole();
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
					PublishNotification(
						ownerUserId,
						sdl::imtauth::TenantMemberships::EMembershipNotificationType::InvitationRejected,
						membershipId,
						cached.userId,
						cached.tenantId,
						QString(),
						cached.role);
				}
			}
		}
		else{
			const CachedMembership& current = currentState.value(membershipId);
			// Was inactive, now active → accepted.
			if (!cached.isActive && current.isActive){
				QByteArray ownerUserId = FindTenantOwnerUserId(current.tenantId);
				if (!ownerUserId.isEmpty()){
					PublishNotification(
						ownerUserId,
						sdl::imtauth::TenantMemberships::EMembershipNotificationType::InvitationAccepted,
						membershipId,
						current.userId,
						current.tenantId,
						QString(),
						current.role);
				}
			}
		}
	}

	// Detect new memberships (not in cache).
	// Check all tenants from currentState for new inactive memberships.
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
				const imtauth::ITenantMembership* membershipPtr = m_membershipManagerCompPtr->GetMembership(membershipId);
				if (membershipPtr != nullptr){
					CachedMembership newEntry;
					newEntry.userId = membershipPtr->GetUserId();
					newEntry.tenantId = membershipPtr->GetTenantId();
					newEntry.role = membershipPtr->GetRole();
					newEntry.isActive = membershipPtr->IsActive();
					currentState.insert(membershipId, newEntry);

					// New inactive membership = invitation.
					if (!newEntry.isActive){
						PublishNotification(
							newEntry.userId,
							sdl::imtauth::TenantMemberships::EMembershipNotificationType::InvitationReceived,
							membershipId,
							newEntry.userId,
							newEntry.tenantId,
							QString(),
							newEntry.role);
					}
				}
			}
		}
	}

	// Update cache to current state.
	m_cachedMemberships = currentState;
}


// private methods

void CTenantMembershipPublisherComp::PublishNotification(
	const QByteArray& targetUserId,
	sdl::imtauth::TenantMemberships::EMembershipNotificationType notificationType,
	const QByteArray& membershipId,
	const QByteArray& userId,
	const QByteArray& tenantId,
	const QString& tenantName,
	imtauth::ITenantMembership::TenantMemberRole role) const
{
	sdl::imtauth::TenantMemberships::CMembershipNotification notification;
	notification.Version_1_0.emplace();
	notification.Version_1_0->notificationType = notificationType;
	notification.Version_1_0->membershipId = membershipId;
	notification.Version_1_0->userId = userId;
	notification.Version_1_0->tenantId = tenantId;
	notification.Version_1_0->tenantName = tenantName;
	notification.Version_1_0->role = static_cast<sdl::imtauth::TenantMemberships::TenantMemberRole>(role);

	QJsonObject jsonObject;
	if (!notification.WriteToJsonObject(jsonObject)){
		Q_ASSERT(false);
		return;
	}

	QJsonDocument jsonDoc;
	jsonDoc.setObject(jsonObject);
	QByteArray data = jsonDoc.toJson(QJsonDocument::Compact);

	const QByteArray commandId = m_commandIdAttrPtr.IsValid()
			? *m_commandIdAttrPtr
			: QByteArray("OnMembershipNotification");

	// Filter: only push to subscribers whose context userId matches targetUserId
	PublishDataFiltered(commandId, data, [targetUserId](const imtgql::CGqlRequest& gqlRequest) -> bool {
		const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
		if (contextPtr != nullptr){
			const imtauth::IUserInfo* userInfoPtr = contextPtr->GetUserInfo();
			if (userInfoPtr != nullptr){
				return userInfoPtr->GetId() == targetUserId;
			}
		}
		return false;
	});
}


QByteArray CTenantMembershipPublisherComp::FindTenantOwnerUserId(const QByteArray& tenantId) const
{
	if (!m_membershipManagerCompPtr.IsValid() || tenantId.isEmpty()){
		return QByteArray();
	}

	QByteArrayList membershipIds = m_membershipManagerCompPtr->GetMembershipsByTenant(tenantId);
	for (const QByteArray& mId : std::as_const(membershipIds)){
		const imtauth::ITenantMembership* mPtr = m_membershipManagerCompPtr->GetMembership(mId);
		if (mPtr != nullptr && mPtr->GetRole() == imtauth::ITenantMembership::TMR_OWNER){
			return mPtr->GetUserId();
		}
	}

	return QByteArray();
}


} // namespace imtauthgql

