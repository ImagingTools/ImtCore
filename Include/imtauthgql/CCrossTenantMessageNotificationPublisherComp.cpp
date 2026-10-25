// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CCrossTenantMessageNotificationPublisherComp.h>

// Qt includes
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>
#include <imtauth/TenantEvents.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Tenants.h>


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

void CCrossTenantMessageNotificationPublisherComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& changeSet)
{
	if (!m_messageBrokerCompPtr.IsValid() || !m_tenantManagerCompPtr.IsValid()){
		return;
	}

	const QList<QVariantMap> events = imtauth::TenantEvent::GetEvents(changeSet);

	// A message announced as new carries its latest status, so its status changes in the same change are not told again
	QSet<QByteArray> createdMessageIds;
	for (const QVariantMap& event : events){
		if (event.value(imtauth::TenantEvent::s_type).toString() == imtauth::TenantEvent::s_messageCreated){
			createdMessageIds.insert(event.value(imtauth::TenantEvent::s_messageId).toByteArray());
		}
	}

	for (const QVariantMap& event : events){
		const QString type = event.value(imtauth::TenantEvent::s_type).toString();
		const QByteArray messageId = event.value(imtauth::TenantEvent::s_messageId).toByteArray();
		const bool isCreated = type == imtauth::TenantEvent::s_messageCreated;
		if (!isCreated && (type != imtauth::TenantEvent::s_messageStatusChanged || createdMessageIds.contains(messageId))){
			continue;
		}

		const imtauth::CrossTenantMessageInfo message = m_messageBrokerCompPtr->GetMessage(messageId);
		if (message.messageId.isEmpty()){
			continue;
		}

		if (isCreated){
			// A new message goes to the owner of the target organization
			PublishNotification(
						FindTenantOwnerUserId(message.targetTenantId),
						sdl::V1_0::imtauth::ECrossTenantMessageNotificationType::MessageReceived,
						message,
						FindTenantName(message.sourceTenantId));
		}
		else{
			// The answer flows back to the owner of the source organization
			PublishNotification(
						FindTenantOwnerUserId(message.sourceTenantId),
						sdl::V1_0::imtauth::ECrossTenantMessageNotificationType::MessageStatusChanged,
						message,
						FindTenantName(message.targetTenantId));
		}
	}
}


// private methods

void CCrossTenantMessageNotificationPublisherComp::PublishNotification(
			const QByteArray& targetUserId,
			sdl::V1_0::imtauth::ECrossTenantMessageNotificationType notificationType,
			const imtauth::CrossTenantMessageInfo& message,
			const QString& tenantName) const
{
	if (targetUserId.isEmpty()){
		return;
	}

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
			: QByteArrayLiteral("OnCrossTenantMessageNotification");

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
