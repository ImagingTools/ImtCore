// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/ContractNotificationPlan.h>


// ImtCore includes
#include <imtauth/ContractState.h>
#include <imtauth/TenantEvents.h>
#include <imtauthgql/TenantNotifications.h>


namespace imtauthgql
{


namespace ContractNotificationPlan
{


namespace
{


enum Group
{
	G_NONE,
	G_PENDING,
	G_SIGNED,
	G_NOTICE
};


struct Kind
{
	QByteArray typeId;
	istd::IInformationProvider::InformationCategory severity = istd::IInformationProvider::IC_INFO;
	Group group = G_NONE;
	bool isSubjectLinked = true;
};


// A reminder names its date: "end:<time>" or "renewal:<time>"
Kind GetReminderKind(const QByteArray& reminderKey, QDateTime& date)
{
	const int separatorIndex = reminderKey.indexOf(':');
	if (separatorIndex < 0){
		return Kind();
	}

	date = QDateTime::fromString(QString::fromUtf8(reminderKey.mid(separatorIndex + 1)), Qt::ISODate);
	if (!date.isValid()){
		return Kind();
	}

	const QByteArray dateKind = reminderKey.left(separatorIndex);
	if (dateKind == "end"){
		return {TenantNotification::s_contractEndsSoon, istd::IInformationProvider::IC_WARNING};
	}

	if (dateKind == "renewal"){
		return {TenantNotification::s_contractRenewalDeadline, istd::IInformationProvider::IC_WARNING};
	}

	return Kind();
}


Kind GetKind(const QString& eventType)
{
	namespace te = imtauth::TenantEvent;
	namespace tn = TenantNotification;

	const istd::IInformationProvider::InformationCategory info = istd::IInformationProvider::IC_INFO;
	const istd::IInformationProvider::InformationCategory warning = istd::IInformationProvider::IC_WARNING;

	if (eventType == te::s_contractInvitationReceived){
		return {tn::s_contractInvitationReceived, info, G_PENDING};
	}

	if (eventType == te::s_contractChangesRequested){
		return {tn::s_contractChangesRequested, warning, G_PENDING};
	}

	if (eventType == te::s_contractTermsChanged){
		return {tn::s_contractTermsChanged, warning, G_PENDING};
	}

	if (eventType == te::s_contractPartyRemoved){
		return {tn::s_contractPartyRemoved, warning, G_NONE, false};
	}

	if (eventType == te::s_contractSigned){
		return {tn::s_contractSigned, info, G_SIGNED};
	}

	if (eventType == te::s_contractDeclined){
		return {tn::s_contractDeclined, warning};
	}

	if (eventType == te::s_contractConcluded){
		return {tn::s_contractConcluded, info};
	}

	if (eventType == te::s_contractCancelled){
		return {tn::s_contractCancelled, warning};
	}

	if (eventType == te::s_contractNoticeGiven){
		return {tn::s_contractNoticeGiven, warning, G_NOTICE};
	}

	if (eventType == te::s_contractNoticeRevoked){
		return {tn::s_contractNoticeRevoked, info};
	}

	if (eventType == te::s_contractRenewalStopped){
		return {tn::s_contractRenewalStopped, warning};
	}

	if (eventType == te::s_contractWithdrawn){
		return {tn::s_contractWithdrawn, warning};
	}

	if (eventType == te::s_contractPermissionsWithdrawn){
		return {tn::s_contractPermissionsWithdrawn, warning};
	}

	if (eventType == te::s_contractPermissionsReceived){
		return {tn::s_contractPermissionsReceived, info};
	}

	if (eventType == te::s_contractSuperseded){
		return {tn::s_contractSuperseded, info};
	}

	return Kind();
}


void AddWithdrawals(Plan& plan, const QByteArrayList& tenantIds, const QByteArray& groupKey)
{
	for (const QByteArray& tenantId : tenantIds){
		if (!tenantId.isEmpty()){
			plan.withdrawals.append({tenantId, groupKey});
		}
	}
}


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


} // anonymous namespace


QByteArray GetPendingGroupKey(const QByteArray& contractId)
{
	return QByteArrayLiteral("contract-pending:") + contractId;
}


QByteArray GetSignedGroupKey(const QByteArray& contractId)
{
	return QByteArrayLiteral("contract-signed:") + contractId;
}


QByteArray GetNoticeGroupKey(const QByteArray& contractId, const QByteArray& partyTenantId)
{
	return QByteArrayLiteral("contract-notice:") + contractId + QByteArrayLiteral(":") + partyTenantId;
}


Plan CreatePlan(const QVariantMap& event, const imtauth::IContractInfo& contract)
{
	namespace te = imtauth::TenantEvent;

	Plan retVal;

	const QString type = event.value(te::s_type).toString();
	const QByteArray contractId = contract.GetContractId();
	const QByteArray actorTenantId = event.value(te::s_sourceTenantId).toByteArray();
	const QByteArrayList recipientTenantIds = ToTenantIds(event.value(te::s_recipientTenantIds));
	const QByteArray initiatorTenantId = contract.GetInitiatorTenantId();

	// What the event makes untrue
	if (type == te::s_contractSigned || type == te::s_contractChangesRequested){
		// The changes asked of the initiator stay until it changes the terms, even if it signs meanwhile
		if (actorTenantId != initiatorTenantId){
			AddWithdrawals(retVal, {actorTenantId}, GetPendingGroupKey(contractId));
		}
	}
	else if (type == te::s_contractTermsChanged){
		// The initiator answered what it was asked; the signatures it collected are gone; the others review anew
		AddWithdrawals(retVal, {actorTenantId}, GetPendingGroupKey(contractId));
		AddWithdrawals(retVal, {actorTenantId}, GetSignedGroupKey(contractId));
		AddWithdrawals(retVal, recipientTenantIds, GetPendingGroupKey(contractId));
	}
	else if (type == te::s_contractPartyRemoved){
		AddWithdrawals(retVal, recipientTenantIds, GetPendingGroupKey(contractId));
	}
	else if (type == te::s_contractNoticeRevoked){
		AddWithdrawals(retVal, imtauth::GetContractPartyTenantIds(contract), GetNoticeGroupKey(contractId, actorTenantId));
	}
	else if (type == te::s_contractDeclined || type == te::s_contractConcluded || type == te::s_contractCancelled){
		AddWithdrawals(retVal, imtauth::GetContractPartyTenantIds(contract), GetPendingGroupKey(contractId));
		AddWithdrawals(retVal, {initiatorTenantId}, GetSignedGroupKey(contractId));
	}

	// What it tells
	QDateTime date;
	const Kind kind = (type == te::s_contractReminder)
				? GetReminderKind(event.value(te::s_reminderKey).toByteArray(), date)
				: GetKind(type);
	if (kind.typeId.isEmpty()){
		return retVal;
	}

	QByteArray groupKey;
	if (kind.group == G_PENDING){
		groupKey = GetPendingGroupKey(contractId);
	}
	else if (kind.group == G_SIGNED){
		groupKey = GetSignedGroupKey(contractId);
	}
	else if (kind.group == G_NOTICE){
		groupKey = GetNoticeGroupKey(contractId, actorTenantId);
	}

	for (const QByteArray& tenantId : recipientTenantIds){
		Delivery delivery;
		delivery.tenantId = tenantId;
		delivery.typeId = kind.typeId;
		delivery.severity = kind.severity;
		delivery.groupKey = groupKey;
		delivery.partnerTenantId = actorTenantId;
		delivery.isSubjectLinked = kind.isSubjectLinked;
		delivery.date = date;
		retVal.deliveries.append(delivery);
	}

	return retVal;
}


QByteArrayList SelectRecipients(const QByteArray& ownerId, const QByteArray& responsibleUserId, bool isResponsibleMember, const QByteArray& actorUserId)
{
	const QByteArray recipientId = (!responsibleUserId.isEmpty() && isResponsibleMember) ? responsibleUserId : ownerId;
	if (recipientId.isEmpty() || recipientId == actorUserId){
		return QByteArrayList();
	}

	return {recipientId};
}


} // namespace ContractNotificationPlan


} // namespace imtauthgql
