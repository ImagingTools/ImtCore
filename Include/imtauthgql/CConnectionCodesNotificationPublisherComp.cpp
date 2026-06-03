// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CConnectionCodesNotificationPublisherComp.h>

// Qt includes
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutexLocker>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtgql::IGqlSubscriberController)

bool CConnectionCodesNotificationPublisherComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
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

void CConnectionCodesNotificationPublisherComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	// Seed the cache with all existing connect codes so that OnModelChanged()
	// can detect new entries and status transitions.
	if (m_connectionRequestManagerCompPtr.IsValid() && m_tenantManagerCompPtr.IsValid()){
		QMutexLocker locker(&m_cacheMutex);
		QByteArrayList tenantIds = m_tenantManagerCompPtr->GetTenantIds();
		for (const QByteArray& tenantId : std::as_const(tenantIds)){
			imtauth::TenantConnectionRequests requests = m_connectionRequestManagerCompPtr->GetOutgoingRequests(tenantId);
			for (const imtauth::TenantConnectionRequestInfo& req : std::as_const(requests)){
				// Only track entries that have a connect code
				if (!req.connectCode.isEmpty()){
					CachedCode entry;
					entry.sourceTenantId = req.sourceTenantId;
					entry.status = req.status;
					m_cachedCodes.insert(req.requestId, entry);
				}
			}
		}
	}

	if (m_connectionRequestManagerModelCompPtr.IsValid()){
		RegisterModel(m_connectionRequestManagerModelCompPtr.GetPtr(), 0);
	}
}


void CConnectionCodesNotificationPublisherComp::OnComponentDestroyed()
{
	BaseClass2::UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CConnectionCodesNotificationPublisherComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (!m_connectionRequestManagerCompPtr.IsValid() || !m_tenantManagerCompPtr.IsValid()){
		return;
	}

	struct PendingNotification
	{
		QByteArray targetUserId;
		sdl::V1_0::imtauth::EConnectionCodesNotificationType notificationType;
		QByteArray tenantId;
		QByteArray requestId;
	};

	QList<PendingNotification> pendingNotifications;

	{
		QMutexLocker locker(&m_cacheMutex);

		QMap<QByteArray, CachedCode> currentState;

		// Collect all current connect codes across all tenants.
		QByteArrayList tenantIds = m_tenantManagerCompPtr->GetTenantIds();
		for (const QByteArray& tenantId : std::as_const(tenantIds)){
			imtauth::TenantConnectionRequests requests = m_connectionRequestManagerCompPtr->GetOutgoingRequests(tenantId);
			for (const imtauth::TenantConnectionRequestInfo& req : std::as_const(requests)){
				if (!req.connectCode.isEmpty()){
					CachedCode entry;
					entry.sourceTenantId = req.sourceTenantId;
					entry.status = req.status;
					currentState.insert(req.requestId, entry);
				}
			}
		}

		// Detect new codes (not in cache).
		for (auto it = currentState.constBegin(); it != currentState.constEnd(); ++it){
			const QByteArray& requestId = it.key();
			const CachedCode& current = it.value();

			if (!m_cachedCodes.contains(requestId)){
				// New connect code appeared.
				if (current.status == imtauth::TCS_PENDING){
					QByteArray ownerUserId = FindTenantOwnerUserId(current.sourceTenantId);
					if (!ownerUserId.isEmpty()){
						pendingNotifications.append({
							ownerUserId,
							sdl::V1_0::imtauth::EConnectionCodesNotificationType::CodeCreated,
							current.sourceTenantId,
							requestId});
					}
				}
			}
			else{
				// Existing code — check for status transition.
				const CachedCode& cached = m_cachedCodes.value(requestId);
				if (cached.status != current.status){
					QByteArray ownerUserId = FindTenantOwnerUserId(current.sourceTenantId);
					if (!ownerUserId.isEmpty()){
						if (current.status == imtauth::TCS_REVOKED){
							pendingNotifications.append({
								ownerUserId,
								sdl::V1_0::imtauth::EConnectionCodesNotificationType::CodeRevoked,
								current.sourceTenantId,
								requestId});
						}
						else if (current.status == imtauth::TCS_EXPIRED){
							pendingNotifications.append({
								ownerUserId,
								sdl::V1_0::imtauth::EConnectionCodesNotificationType::CodeExpired,
								current.sourceTenantId,
								requestId});
						}
					}
				}
			}
		}

		// Detect removed codes (were in cache but no longer present).
		for (auto it = m_cachedCodes.constBegin(); it != m_cachedCodes.constEnd(); ++it){
			const QByteArray& requestId = it.key();
			const CachedCode& cached = it.value();

			if (!currentState.contains(requestId)){
				// Code was removed from the collection.
				QByteArray ownerUserId = FindTenantOwnerUserId(cached.sourceTenantId);
				if (!ownerUserId.isEmpty()){
					pendingNotifications.append({
						ownerUserId,
						sdl::V1_0::imtauth::EConnectionCodesNotificationType::CodeRevoked,
						cached.sourceTenantId,
						requestId});
				}
			}
		}

		// Update cache to current state.
		m_cachedCodes = currentState;
	}

	// Publish notifications outside the cache lock to avoid potential deadlocks.
	for (const PendingNotification& notification : std::as_const(pendingNotifications)){
		PublishNotification(
			notification.targetUserId,
			notification.notificationType,
			notification.tenantId,
			notification.requestId);
	}
}


// private methods

void CConnectionCodesNotificationPublisherComp::PublishNotification(
			const QByteArray& targetUserId,
			sdl::V1_0::imtauth::EConnectionCodesNotificationType notificationType,
			const QByteArray& tenantId,
			const QByteArray& requestId) const
{
	sdl::V1_0::imtauth::CConnectionCodesNotification notification;
	notification.notificationType = notificationType;
	notification.tenantId = tenantId;
	notification.requestId = requestId;

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
			: QByteArray("OnConnectionCodesNotification");

	// Filter: only push to subscribers whose context userId matches the tenant owner.
	PublishDataFiltered(commandId, data, [targetUserId](const imtgql::CGqlRequest& gqlRequest) -> bool {
		const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
		if (contextPtr != nullptr){
			const QByteArray contextUserId = contextPtr->GetUserId();
			return contextUserId == targetUserId;
		}
		return false;
	});
}


QByteArray CConnectionCodesNotificationPublisherComp::FindTenantOwnerUserId(const QByteArray& tenantId) const
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
