// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CContractNotificationPublisherComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imtauth/ContractState.h>
#include <imtauth/TenantEvents.h>
#include <imtauthgql/TenantNotifications.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Contracts.h>


namespace imtauthgql
{


namespace
{


QByteArrayList ToTenantIds(const QVariant& value)
{
	QByteArrayList retVal;
	for (const QString& tenantId : value.toStringList()){
		if (!tenantId.isEmpty()){
			retVal.append(tenantId.toUtf8());
		}
	}

	return retVal;
}


QByteArray GetPendingGroupKey(const QByteArray& contractId)
{
	return QByteArrayLiteral("contract-pending:") + contractId;
}


QByteArray GetSignedGroupKey(const QByteArray& contractId)
{
	return QByteArrayLiteral("contract-signed:") + contractId;
}


} // anonymous namespace


// protected methods

// reimplemented (imtgql::IGqlSubscriberController)

bool CContractNotificationPublisherComp::IsOpenFor(SubscriberKind subscriberKind, const imtgql::CGqlRequest& gqlRequest) const
{
	// A session hears of the contracts of its own organization only, whatever its permissions
	if (subscriberKind == SK_RECIPIENT){
		return m_readPermissionsAttrPtr.GetCount() == 0 && gqlRequest.GetCommandId() == GetPushCommandId().toUtf8();
	}

	return BaseClass::IsOpenFor(subscriberKind, gqlRequest);
}


// reimplemented (icomp::CComponentBase)

void CContractNotificationPublisherComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_contractModelCompPtr.IsValid()){
		BaseClass2::RegisterModel(m_contractModelCompPtr.GetPtr(), 0);
	}
}


void CContractNotificationPublisherComp::OnComponentDestroyed()
{
	BaseClass2::UnregisterAllModels();

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CContractNotificationPublisherComp::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& changeSet)
{
	for (const QVariantMap& event : imtauth::TenantEvent::GetEvents(changeSet)){
		const QString type = event.value(imtauth::TenantEvent::s_type).toString();
		const QByteArray contractId = event.value(imtauth::TenantEvent::s_contractId).toByteArray();
		const QByteArrayList tenantIds = ToTenantIds(event.value(imtauth::TenantEvent::s_recipientTenantIds));
		if (contractId.isEmpty()){
			continue;
		}

		// A step ends even when nobody is told, e.g. the terms changed after the only other party was replaced
		EndStep(event, GetStepEnd(type), tenantIds);
		if (tenantIds.isEmpty()){
			continue;
		}

		if (type == imtauth::TenantEvent::s_contractChanged){
			PushChange(
						contractId,
						tenantIds,
						event.value(imtauth::TenantEvent::s_userId).toByteArray(),
						event.value(imtauth::TenantEvent::s_userName).toString());

			continue;
		}

		const NotificationKind kind = GetNotificationKind(type);
		if (!kind.typeId.isEmpty()){
			SendToNotificationCenter(event, kind, tenantIds);
		}
	}
}


// private methods

CContractNotificationPublisherComp::NotificationKind CContractNotificationPublisherComp::GetNotificationKind(const QString& eventType)
{
	namespace te = imtauth::TenantEvent;
	namespace tn = TenantNotification;

	const istd::IInformationProvider::InformationCategory info = istd::IInformationProvider::IC_INFO;
	const istd::IInformationProvider::InformationCategory warning = istd::IInformationProvider::IC_WARNING;

	NotificationKind retVal;
	if (eventType == te::s_contractInvitationReceived){
		retVal = {tn::s_contractInvitationReceived, info, NG_PENDING};
	}
	else if (eventType == te::s_contractChangesRequested){
		retVal = {tn::s_contractChangesRequested, warning, NG_PENDING};
	}
	else if (eventType == te::s_contractTermsChanged){
		retVal = {tn::s_contractTermsChanged, warning, NG_PENDING};
	}
	else if (eventType == te::s_contractPartyRemoved){
		retVal = {tn::s_contractPartyRemoved, warning, NG_NONE};
	}
	else if (eventType == te::s_contractSigned){
		retVal = {tn::s_contractSigned, info, NG_SIGNED};
	}
	else if (eventType == te::s_contractDeclined){
		retVal = {tn::s_contractDeclined, warning, NG_NONE};
	}
	else if (eventType == te::s_contractConcluded){
		retVal = {tn::s_contractConcluded, info, NG_NONE};
	}
	else if (eventType == te::s_contractCancelled){
		retVal = {tn::s_contractCancelled, warning, NG_NONE};
	}
	else if (eventType == te::s_contractNoticeGiven){
		retVal = {tn::s_contractNoticeGiven, warning, NG_NONE};
	}
	else if (eventType == te::s_contractNoticeRevoked){
		retVal = {tn::s_contractNoticeRevoked, info, NG_NONE};
	}
	else if (eventType == te::s_contractRenewalStopped){
		retVal = {tn::s_contractRenewalStopped, warning, NG_NONE};
	}
	else if (eventType == te::s_contractWithdrawn){
		retVal = {tn::s_contractWithdrawn, warning, NG_NONE};
	}
	else if (eventType == te::s_contractPermissionsWithdrawn){
		retVal = {tn::s_contractPermissionsWithdrawn, warning, NG_NONE};
	}
	else if (eventType == te::s_contractPermissionsReceived){
		retVal = {tn::s_contractPermissionsReceived, info, NG_NONE};
	}
	else if (eventType == te::s_contractSuperseded){
		retVal = {tn::s_contractSuperseded, info, NG_NONE};
	}
	else if (eventType == te::s_contractReminder){
		retVal = {tn::s_contractReminder, warning, NG_NONE};
	}

	return retVal;
}


CContractNotificationPublisherComp::StepEnd CContractNotificationPublisherComp::GetStepEnd(const QString& eventType)
{
	namespace te = imtauth::TenantEvent;

	if (eventType == te::s_contractSigned || eventType == te::s_contractChangesRequested){
		return SE_ACTOR_ANSWERED;
	}

	if (eventType == te::s_contractTermsChanged){
		return SE_TERMS_CHANGED;
	}

	if (eventType == te::s_contractPartyRemoved){
		return SE_RECIPIENTS_REMOVED;
	}

	if (eventType == te::s_contractDeclined
				|| eventType == te::s_contractConcluded
				|| eventType == te::s_contractCancelled){
		return SE_ALL;
	}

	return SE_NONE;
}


void CContractNotificationPublisherComp::PushChange(const QByteArray& contractId, const QByteArrayList& tenantIds, const QByteArray& userId, const QString& userName) const
{
	const QByteArray commandId = GetPushCommandId().toUtf8();

	for (const QByteArray& tenantId : tenantIds){
		sdl::V1_0::imtauth::CContractNotification notification;
		notification.notificationType = sdl::V1_0::imtauth::EContractNotificationType::ContractChanged;
		notification.contractId = contractId;
		notification.tenantId = tenantId;
		notification.sourceUserId = userId;
		notification.sourceUserName = userName;

		QJsonObject jsonObject;
		if (!notification.WriteToJsonObject(jsonObject)){
			Q_ASSERT(false);

			continue;
		}

		const QByteArray data = QJsonDocument(jsonObject).toJson(QJsonDocument::Compact);
		PublishDataFiltered(commandId, data, [tenantId](const imtgql::CGqlRequest& gqlRequest){
			const imtgql::IGqlContext* contextPtr = gqlRequest.GetRequestContext();

			return contextPtr != nullptr && !contextPtr->GetUserId().isEmpty() && contextPtr->GetTenantId() == tenantId;
		});
	}
}


void CContractNotificationPublisherComp::SendToNotificationCenter(const QVariantMap& event, const NotificationKind& kind, const QByteArrayList& tenantIds) const
{
	namespace tn = TenantNotification;

	if (!m_notificationSenderCompPtr.IsValid()){
		return;
	}

	const QByteArray contractId = event.value(imtauth::TenantEvent::s_contractId).toByteArray();
	imtauth::IContractInfoUniquePtr contractPtr = m_contractProviderCompPtr->GetContract(contractId);
	if (!contractPtr.IsValid()){
		return;
	}

	imtnotify::ObjectLinkInfo subject;
	subject.id = contractId;
	subject.typeId = QByteArrayLiteral("TenantContract");
	subject.name = contractPtr->GetNumber().isEmpty() ? contractPtr->GetName() : contractPtr->GetNumber() + QStringLiteral(" ") + contractPtr->GetName();

	// The party that acted; none for what the contract does by itself
	imtnotify::NotificationArguments arguments;
	const QByteArray partnerTenantId = event.value(imtauth::TenantEvent::s_sourceTenantId).toByteArray();
	if (!partnerTenantId.isEmpty()){
		arguments.append(imtnotify::NotificationArgument::Object(QByteArrayLiteral("partner"), tn::TenantLink(partnerTenantId, FindTenantName(partnerTenantId))));
	}

	QByteArray groupKey;
	if (kind.group == NG_PENDING){
		groupKey = GetPendingGroupKey(contractId);
	}
	else if (kind.group == NG_SIGNED){
		groupKey = GetSignedGroupKey(contractId);
	}
	const imtnotify::ObjectLinkInfo actor = tn::RequestUserLink(m_userCollectionCompPtr.GetPtr());

	for (const QByteArray& tenantId : tenantIds){
		imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);
		if (!tenantPtr.IsValid()){
			continue;
		}

		// The owner, and the user the organization made responsible for the contract
		QByteArrayList userIds = {tenantPtr->GetOwnerId()};
		imtauth::IContractPartyPrivateInfoUniquePtr privateInfoPtr = m_contractProviderCompPtr->GetPrivateInfo(contractId, tenantId);
		const QByteArray responsibleUserId = privateInfoPtr.IsValid() ? privateInfoPtr->GetResponsibleUserId() : QByteArray();
		if (!responsibleUserId.isEmpty() && !userIds.contains(responsibleUserId)){
			imtauth::ITenantMembershipUniquePtr membershipPtr = m_membershipManagerCompPtr->FindMembership(responsibleUserId, tenantId);
			if (membershipPtr.IsValid() && membershipPtr->IsActive()){
				userIds.append(responsibleUserId);
			}
		}

		for (const QByteArray& userId : std::as_const(userIds)){
			if (userId != actor.id){
				tn::Send(m_notificationSenderCompPtr.GetPtr(), userId, tenantId, kind.typeId, subject, groupKey, kind.severity, actor, arguments);
			}
		}
	}
}


void CContractNotificationPublisherComp::EndStep(const QVariantMap& event, StepEnd stepEnd, const QByteArrayList& recipientTenantIds) const
{
	const QByteArray contractId = event.value(imtauth::TenantEvent::s_contractId).toByteArray();
	const QByteArrayList actorTenantIds = {event.value(imtauth::TenantEvent::s_sourceTenantId).toByteArray()};

	switch (stepEnd){
	case SE_ACTOR_ANSWERED:
		{
			// The changes asked of the initiator stay until it changes the terms, even if it signs meanwhile
			imtauth::IContractInfoUniquePtr contractPtr = m_contractProviderCompPtr->GetContract(contractId);
			if (contractPtr.IsValid() && contractPtr->GetInitiatorTenantId() != actorTenantIds.first()){
				WithdrawGroup(GetPendingGroupKey(contractId), actorTenantIds);
			}
		}
		break;

	case SE_TERMS_CHANGED:
		WithdrawGroup(GetPendingGroupKey(contractId), actorTenantIds);
		WithdrawGroup(GetSignedGroupKey(contractId), actorTenantIds);
		break;

	case SE_RECIPIENTS_REMOVED:
		WithdrawGroup(GetPendingGroupKey(contractId), recipientTenantIds);
		break;

	case SE_ALL:
		{
			imtauth::IContractInfoUniquePtr contractPtr = m_contractProviderCompPtr->GetContract(contractId);
			if (contractPtr.IsValid()){
				const QByteArrayList partyTenantIds = imtauth::GetContractPartyTenantIds(*contractPtr);
				WithdrawGroup(GetPendingGroupKey(contractId), partyTenantIds);
				WithdrawGroup(GetSignedGroupKey(contractId), {contractPtr->GetInitiatorTenantId()});
			}
		}
		break;

	case SE_NONE:
		break;
	}
}


void CContractNotificationPublisherComp::WithdrawGroup(const QByteArray& groupKey, const QByteArrayList& tenantIds) const
{
	if (!m_notificationSenderCompPtr.IsValid()){
		return;
	}

	for (const QByteArray& tenantId : tenantIds){
		if (!tenantId.isEmpty()){
			TenantNotification::Withdraw(m_notificationSenderCompPtr.GetPtr(), tenantId, groupKey);
		}
	}
}


QString CContractNotificationPublisherComp::FindTenantName(const QByteArray& tenantId) const
{
	imtauth::ITenantInfoUniquePtr tenantPtr = m_tenantManagerCompPtr->GetTenant(tenantId);

	return tenantPtr.IsValid() ? tenantPtr->GetTenantName() : QString();
}


QString CContractNotificationPublisherComp::GetPushCommandId() const
{
	return (m_commandIdsAttrPtr.IsValid() && m_commandIdsAttrPtr.GetCount() > 0)
				? QString::fromUtf8(m_commandIdsAttrPtr[0])
				: QStringLiteral("OnContractNotification");
}


} // namespace imtauthgql


