// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CConnectionCodesNotificationPublisherComp.h>

// Qt includes
#include <QJsonDocument>
#include <QJsonObject>

// ImtCore includes
#include <imtauthgql/TenantNotifications.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>
#include <imtauth/TenantEvents.h>
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

	if (m_connectionRequestManagerModelCompPtr.IsValid()){
		BaseClass2::RegisterModel(m_connectionRequestManagerModelCompPtr.GetPtr(), 0);
	}
}


void CConnectionCodesNotificationPublisherComp::OnComponentDestroyed()
{
	BaseClass2::UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CConnectionCodesNotificationPublisherComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& changeSet)
{
	using NotificationType = sdl::V1_0::imtauth::EConnectionNotificationType;
	using RequestStatus = imtauth::ITenantConnectionRequestInfo::ConnectionRequestStatus;
	using ProposalStatus = imtauth::ITenantRelationshipProposalInfo::RelationshipProposalStatus;

	// The manager names each change, so only the organizations it touched are read
	for (const QVariantMap& event : imtauth::TenantEvent::GetEvents(changeSet)){
		const QString type = event.value(imtauth::TenantEvent::s_type).toString();
		const QByteArray sourceTenantId = event.value(imtauth::TenantEvent::s_sourceTenantId).toByteArray();
		const QByteArray targetTenantId = event.value(imtauth::TenantEvent::s_targetTenantId).toByteArray();
		const int status = event.value(imtauth::TenantEvent::s_status).toInt();

		if (type == imtauth::TenantEvent::s_connectionRequestCreated){
			if (status == RequestStatus::CRS_PENDING){
				PublishNotification(FindTenantOwnerUserId(targetTenantId), NotificationType::ConnectionRequestReceived, targetTenantId,
							event.value(imtauth::TenantEvent::s_requestId).toByteArray(), sourceTenantId);
			}
		}
		else if (type == imtauth::TenantEvent::s_connectionRequestStatusChanged){
			// The answer goes to the organization that asked
			const QByteArray requestId = event.value(imtauth::TenantEvent::s_requestId).toByteArray();
			WithdrawFromNotificationCenter(targetTenantId, GetGroupKey(NotificationType::ConnectionRequestReceived, requestId));
			if (status == RequestStatus::CRS_APPROVED){
				PublishNotification(FindTenantOwnerUserId(sourceTenantId), NotificationType::ConnectionRequestApproved, sourceTenantId, requestId, targetTenantId);
			}
			else if (status == RequestStatus::CRS_REJECTED){
				PublishNotification(FindTenantOwnerUserId(sourceTenantId), NotificationType::ConnectionRequestRejected, sourceTenantId, requestId, targetTenantId);
			}
		}
		else if (type == imtauth::TenantEvent::s_connectionRemoved || type == imtauth::TenantEvent::s_relationshipRemoved){
			const bool isConnection = type == imtauth::TenantEvent::s_connectionRemoved;
			const NotificationType notificationType = isConnection ? NotificationType::ConnectionRemoved : NotificationType::RelationshipRemoved;
			const QByteArray relatedId = event.value(isConnection ? imtauth::TenantEvent::s_connectionId : imtauth::TenantEvent::s_relationshipId).toByteArray();

			const QByteArray sourceOwnerUserId = FindTenantOwnerUserId(sourceTenantId);
			PublishNotification(sourceOwnerUserId, notificationType, sourceTenantId, relatedId, targetTenantId);

			const QByteArray targetOwnerUserId = FindTenantOwnerUserId(targetTenantId);
			if (targetOwnerUserId != sourceOwnerUserId){
				PublishNotification(targetOwnerUserId, notificationType, targetTenantId, relatedId, sourceTenantId);
			}
		}
		else if (type == imtauth::TenantEvent::s_proposalCreated){
			PublishNotification(FindTenantOwnerUserId(targetTenantId), NotificationType::RelationshipProposalReceived, targetTenantId,
						event.value(imtauth::TenantEvent::s_proposalId).toByteArray(), sourceTenantId);
		}
		else if (type == imtauth::TenantEvent::s_proposalStatusChanged){
			// The answer goes to the party that is waiting for it
			const QByteArray proposalId = event.value(imtauth::TenantEvent::s_proposalId).toByteArray();
			WithdrawFromNotificationCenter(targetTenantId, GetGroupKey(NotificationType::RelationshipProposalReceived, proposalId));
			const int previousStatus = event.value(imtauth::TenantEvent::s_previousStatus).toInt();
			if (status == ProposalStatus::RPS_REJECTED){
				PublishNotification(FindTenantOwnerUserId(sourceTenantId), NotificationType::RelationshipProposalRejected, sourceTenantId, proposalId, targetTenantId);
			}
			else if (status == ProposalStatus::RPS_APPROVED_BY_COUNTERPARTY
						|| (status == ProposalStatus::RPS_APPLIED && previousStatus != ProposalStatus::RPS_APPROVED_BY_COUNTERPARTY)){
				PublishNotification(FindTenantOwnerUserId(sourceTenantId), NotificationType::RelationshipProposalApproved, sourceTenantId, proposalId, targetTenantId);
			}
			else if (status == ProposalStatus::RPS_APPLIED){
				PublishNotification(FindTenantOwnerUserId(targetTenantId), NotificationType::RelationshipProposalApproved, targetTenantId, proposalId, sourceTenantId);
			}
		}
	}
}


// private methods

void CConnectionCodesNotificationPublisherComp::PublishNotification(
	const QByteArray& targetUserId,
	sdl::V1_0::imtauth::EConnectionNotificationType notificationType,
	const QByteArray& tenantId,
	const QByteArray& relatedId,
	const QByteArray& partnerTenantId) const
{
	if (targetUserId.isEmpty()){
		return;
	}

	SendToNotificationCenter(targetUserId, notificationType, tenantId, relatedId, partnerTenantId);

	sdl::V1_0::imtauth::CConnectionNotification notification;
	notification.notificationType = notificationType;
	notification.tenantId = tenantId;

	// Set the appropriate related ID based on notification type.
	switch (notificationType){
		case sdl::V1_0::imtauth::EConnectionNotificationType::ConnectionRequestReceived:
		case sdl::V1_0::imtauth::EConnectionNotificationType::ConnectionRequestApproved:
		case sdl::V1_0::imtauth::EConnectionNotificationType::ConnectionRequestRejected:
			notification.requestId = relatedId;
			break;
		case sdl::V1_0::imtauth::EConnectionNotificationType::ConnectionRemoved:
			notification.connectionId = relatedId;
			break;
		case sdl::V1_0::imtauth::EConnectionNotificationType::RelationshipProposalReceived:
		case sdl::V1_0::imtauth::EConnectionNotificationType::RelationshipProposalApproved:
		case sdl::V1_0::imtauth::EConnectionNotificationType::RelationshipProposalRejected:
			notification.proposalId = relatedId;
			break;
		case sdl::V1_0::imtauth::EConnectionNotificationType::RelationshipRemoved:
			notification.relationshipId = relatedId;
			break;
	}

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
		: QByteArrayLiteral("OnConnectionCodesNotification");

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


void CConnectionCodesNotificationPublisherComp::SendToNotificationCenter(
			const QByteArray& targetUserId,
			sdl::V1_0::imtauth::EConnectionNotificationType notificationType,
			const QByteArray& tenantId,
			const QByteArray& relatedId,
			const QByteArray& partnerTenantId) const
{
	namespace tn = TenantNotification;
	using NotificationType = sdl::V1_0::imtauth::EConnectionNotificationType;

	if (!m_notificationSenderCompPtr.IsValid()){
		return;
	}

	const QString tenantName = FindTenantName(tenantId);

	QByteArray typeId;
	QByteArray subjectTypeId;
	istd::IInformationProvider::InformationCategory severity = istd::IInformationProvider::IC_INFO;
	switch (notificationType){
	case NotificationType::ConnectionRequestReceived:
		typeId = tn::s_connectionRequestReceived;
		subjectTypeId = QByteArrayLiteral("TenantConnectionRequest");
		severity = istd::IInformationProvider::IC_INFO;
		break;
	case NotificationType::ConnectionRequestApproved:
		typeId = tn::s_connectionRequestApproved;
		subjectTypeId = QByteArrayLiteral("TenantConnectionRequest");
		severity = istd::IInformationProvider::IC_INFO;
		break;
	case NotificationType::ConnectionRequestRejected:
		typeId = tn::s_connectionRequestRejected;
		subjectTypeId = QByteArrayLiteral("TenantConnectionRequest");
		severity = istd::IInformationProvider::IC_INFO;
		break;
	case NotificationType::ConnectionRemoved:
		typeId = tn::s_connectionRemoved;
		subjectTypeId = QByteArrayLiteral("TenantConnection");
		severity = istd::IInformationProvider::IC_WARNING;
		break;
	case NotificationType::RelationshipProposalReceived:
		typeId = tn::s_relationshipProposalReceived;
		subjectTypeId = QByteArrayLiteral("TenantRelationshipProposal");
		severity = istd::IInformationProvider::IC_INFO;
		break;
	case NotificationType::RelationshipProposalApproved:
		typeId = tn::s_relationshipProposalApproved;
		subjectTypeId = QByteArrayLiteral("TenantRelationshipProposal");
		severity = istd::IInformationProvider::IC_INFO;
		break;
	case NotificationType::RelationshipProposalRejected:
		typeId = tn::s_relationshipProposalRejected;
		subjectTypeId = QByteArrayLiteral("TenantRelationshipProposal");
		severity = istd::IInformationProvider::IC_INFO;
		break;
	case NotificationType::RelationshipRemoved:
		typeId = tn::s_relationshipRemoved;
		subjectTypeId = QByteArrayLiteral("TenantRelationship");
		severity = istd::IInformationProvider::IC_WARNING;
		break;
	}

	if (typeId.isEmpty()){
		return;
	}

	imtnotify::ObjectLinkInfo subject;
	subject.id = relatedId;
	subject.typeId = subjectTypeId;
	subject.name = tenantName;

	// The other organization of the connection or the relationship
	imtnotify::NotificationArguments arguments;
	if (!partnerTenantId.isEmpty()){
		arguments.append(imtnotify::NotificationArgument::Object(
					QByteArrayLiteral("partner"),
					tn::TenantLink(partnerTenantId, FindTenantName(partnerTenantId))));
	}

	// The owner hears about it inside the organization concerned
	tn::Send(m_notificationSenderCompPtr.GetPtr(), targetUserId, tenantId, typeId, subject, GetGroupKey(notificationType, relatedId), severity,
				imtnotify::ObjectLinkInfo(), arguments);
}


QString CConnectionCodesNotificationPublisherComp::FindTenantName(const QByteArray& tenantId) const
{
	if (!m_tenantManagerCompPtr.IsValid() || tenantId.isEmpty()){
		return QString();
	}

	imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);

	return tenantPtr.IsValid() ? tenantPtr->GetTenantName() : QString();
}


void CConnectionCodesNotificationPublisherComp::WithdrawFromNotificationCenter(const QByteArray& tenantId, const QByteArray& groupKey) const
{
	if (m_notificationSenderCompPtr.IsValid() && !tenantId.isEmpty() && !groupKey.isEmpty()){
		m_notificationSenderCompPtr->Withdraw(QByteArray(), tenantId, groupKey);
	}
}


QByteArray CConnectionCodesNotificationPublisherComp::GetGroupKey(
			sdl::V1_0::imtauth::EConnectionNotificationType notificationType,
			const QByteArray& relatedId)
{
	// Only a pending request waits for an answer, and is withdrawn when it gets one
	if (notificationType == sdl::V1_0::imtauth::EConnectionNotificationType::ConnectionRequestReceived){
		return QByteArrayLiteral("connection-request:") + relatedId;
	}

	if (notificationType == sdl::V1_0::imtauth::EConnectionNotificationType::RelationshipProposalReceived){
		return QByteArrayLiteral("relationship-proposal:") + relatedId;
	}

	return QByteArray();
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
