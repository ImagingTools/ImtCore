// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CCrossTenantMessageNotificationPublisherComp.h>

// Qt includes
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutexLocker>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>
#include <imtauthsdl/SDL/1.0/CPP/Tenants.h>


namespace imtauthgql
{


namespace
{


sdl::V1_0::imtauth::CrossTenantMessageType ToSdlMessageType(imtauth::CrossTenantMessageType type)
{
	switch (type){
	case imtauth::CTMT_ORDER_REQUEST:
		return sdl::V1_0::imtauth::CrossTenantMessageType::OrderRequest;
	case imtauth::CTMT_ORDER_CONFIRMATION:
		return sdl::V1_0::imtauth::CrossTenantMessageType::OrderConfirmation;
	case imtauth::CTMT_ORDER_REJECTION:
		return sdl::V1_0::imtauth::CrossTenantMessageType::OrderRejection;
	case imtauth::CTMT_ORDER_STATUS_UPDATE:
		return sdl::V1_0::imtauth::CrossTenantMessageType::OrderStatusUpdate;
	case imtauth::CTMT_ORDER_CANCELLATION:
		return sdl::V1_0::imtauth::CrossTenantMessageType::OrderCancellation;
	case imtauth::CTMT_DOCUMENT_SHARE:
		return sdl::V1_0::imtauth::CrossTenantMessageType::DocumentShare;
	default:
		return sdl::V1_0::imtauth::CrossTenantMessageType::Custom;
	}
}


sdl::V1_0::imtauth::CrossTenantMessageStatus ToSdlMessageStatus(imtauth::CrossTenantMessageStatus status)
{
	switch (status){
	case imtauth::CTMS_VALIDATED:
		return sdl::V1_0::imtauth::CrossTenantMessageStatus::Validated;
	case imtauth::CTMS_DELIVERED:
		return sdl::V1_0::imtauth::CrossTenantMessageStatus::Delivered;
	case imtauth::CTMS_ACKNOWLEDGED:
		return sdl::V1_0::imtauth::CrossTenantMessageStatus::Acknowledged;
	case imtauth::CTMS_PROCESSED:
		return sdl::V1_0::imtauth::CrossTenantMessageStatus::Processed;
	case imtauth::CTMS_FAILED:
		return sdl::V1_0::imtauth::CrossTenantMessageStatus::Failed;
	case imtauth::CTMS_EXPIRED:
		return sdl::V1_0::imtauth::CrossTenantMessageStatus::Expired;
	default:
		return sdl::V1_0::imtauth::CrossTenantMessageStatus::Created;
	}
}


} // anonymous namespace


// protected methods

// reimplemented (imtgql::IGqlSubscriberController)

bool CCrossTenantMessageNotificationPublisherComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
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

void CCrossTenantMessageNotificationPublisherComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	// Initialize the message cache so subsequent OnModelChanged() calls can
	// detect NEW messages (those not present in the cache) and status changes.
	if (m_messageBrokerCompPtr.IsValid() && m_tenantManagerCompPtr.IsValid()){
		QMutexLocker locker(&m_cacheMutex);
		QByteArrayList tenantIds = m_tenantManagerCompPtr->GetTenantIds();
		for (const QByteArray& tenantId : std::as_const(tenantIds)){
			imtauth::CrossTenantMessages messages = m_messageBrokerCompPtr->GetIncomingMessages(tenantId);
			for (const imtauth::CrossTenantMessageInfo& message : std::as_const(messages)){
				m_cachedMessageStatus.insert(message.messageId, message.status);
			}
		}
	}

	if (m_messageBrokerModelCompPtr.IsValid()){
		RegisterModel(m_messageBrokerModelCompPtr.GetPtr(), 0);
	}
}


void CCrossTenantMessageNotificationPublisherComp::OnComponentDestroyed()
{
	BaseClass2::UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CCrossTenantMessageNotificationPublisherComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (!m_messageBrokerCompPtr.IsValid() || !m_tenantManagerCompPtr.IsValid()){
		return;
	}

	struct PendingNotification
	{
		QByteArray targetUserId;
		sdl::V1_0::imtauth::ECrossTenantMessageNotificationType notificationType;
		imtauth::CrossTenantMessageInfo message;
		QString tenantName;
	};

	QList<PendingNotification> pendingNotifications;

	{
		QMutexLocker locker(&m_cacheMutex);

		QMap<QByteArray, imtauth::CrossTenantMessageStatus> currentState;

		QByteArrayList tenantIds = m_tenantManagerCompPtr->GetTenantIds();
		for (const QByteArray& tenantId : std::as_const(tenantIds)){
			// Every message has exactly one target tenant, so iterating the
			// inbox of all tenants visits each message exactly once.
			imtauth::CrossTenantMessages messages = m_messageBrokerCompPtr->GetIncomingMessages(tenantId);
			for (const imtauth::CrossTenantMessageInfo& message : std::as_const(messages)){
				currentState.insert(message.messageId, message.status);

				if (!m_cachedMessageStatus.contains(message.messageId)){
					// New incoming message — notify the target tenant owner.
					QByteArray ownerUserId = FindTenantOwnerUserId(message.targetTenantId);
					if (!ownerUserId.isEmpty()){
						pendingNotifications.append({
							ownerUserId,
							sdl::V1_0::imtauth::ECrossTenantMessageNotificationType::MessageReceived,
							message,
							FindTenantName(message.sourceTenantId)});
					}
				}
				else if (m_cachedMessageStatus.value(message.messageId) != message.status){
					// Status changed — notify the source tenant owner so the
					// sender learns about confirmations/rejections/updates that
					// flow back through the reverse channel.
					QByteArray ownerUserId = FindTenantOwnerUserId(message.sourceTenantId);
					if (!ownerUserId.isEmpty()){
						pendingNotifications.append({
							ownerUserId,
							sdl::V1_0::imtauth::ECrossTenantMessageNotificationType::MessageStatusChanged,
							message,
							FindTenantName(message.targetTenantId)});
					}
				}
			}
		}

		m_cachedMessageStatus = currentState;
	}

	// Publish notifications outside the cache lock to avoid potential deadlocks
	// with the base class mutex used by PublishDataFiltered.
	for (const PendingNotification& notification : std::as_const(pendingNotifications)){
		PublishNotification(
			notification.targetUserId,
			notification.notificationType,
			notification.message,
			notification.tenantName);
	}
}


// private methods

void CCrossTenantMessageNotificationPublisherComp::PublishNotification(
			const QByteArray& targetUserId,
			sdl::V1_0::imtauth::ECrossTenantMessageNotificationType notificationType,
			const imtauth::CrossTenantMessageInfo& message,
			const QString& tenantName) const
{
	sdl::V1_0::imtauth::CCrossTenantMessageNotification notification;
	notification.notificationType = notificationType;
	notification.messageId = message.messageId;
	notification.sourceTenantId = message.sourceTenantId;
	notification.targetTenantId = message.targetTenantId;
	notification.relationshipId = message.relationshipId;
	notification.messageType = ToSdlMessageType(message.messageType);
	notification.status = ToSdlMessageStatus(message.status);
	notification.tenantName = tenantName;

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
			: QByteArray("OnCrossTenantMessageNotification");

	// Filter: only push to subscribers whose context userId matches targetUserId.
	PublishDataFiltered(commandId, data, [targetUserId](const imtgql::CGqlRequest& gqlRequest) -> bool {
		const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();
		if (contextPtr != nullptr){
			const QByteArray contextUserId = contextPtr->GetUserId();
			return contextUserId == targetUserId;
		}
		return false;
	});
}


QByteArray CCrossTenantMessageNotificationPublisherComp::FindTenantOwnerUserId(const QByteArray& tenantId) const
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


QString CCrossTenantMessageNotificationPublisherComp::FindTenantName(const QByteArray& tenantId) const
{
	if (!m_tenantManagerCompPtr.IsValid() || tenantId.isEmpty()){
		return QString();
	}

	imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);
	if (tenantPtr.IsValid()){
		return tenantPtr->GetTenantName();
	}

	return QString();
}


} // namespace imtauthgql
