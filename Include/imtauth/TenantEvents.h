// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QAtomicInteger>
#include <QtCore/QList>
#include <QtCore/QVariantMap>

// ACF includes
#include <istd/IChangeable.h>
#include <istd/CChangeNotifier.h>


namespace imtauth
{


/**
	Events the organization managers attach to their change notifications.
	A notification publisher reads what changed from them instead of comparing the whole state before and after.
*/
namespace TenantEvent
{


static const inline QByteArray s_keyPrefix = QByteArrayLiteral("TenantEvent:");

static const inline QString s_type = QStringLiteral("type");
static const inline QString s_tenantId = QStringLiteral("tenantId");
static const inline QString s_userId = QStringLiteral("userId");
static const inline QString s_userName = QStringLiteral("userName");
static const inline QString s_membershipId = QStringLiteral("membershipId");
static const inline QString s_invitationId = QStringLiteral("invitationId");
static const inline QString s_invitedByUserId = QStringLiteral("invitedByUserId");
static const inline QString s_previousOwnerId = QStringLiteral("previousOwnerId");
static const inline QString s_ownerId = QStringLiteral("ownerId");
static const inline QString s_requestId = QStringLiteral("requestId");
static const inline QString s_connectionId = QStringLiteral("connectionId");
static const inline QString s_messageId = QStringLiteral("messageId");
static const inline QString s_sourceTenantId = QStringLiteral("sourceTenantId");
static const inline QString s_targetTenantId = QStringLiteral("targetTenantId");
static const inline QString s_status = QStringLiteral("status");
static const inline QString s_previousStatus = QStringLiteral("previousStatus");

static const inline QString s_membershipAdded = QStringLiteral("MembershipAdded");
static const inline QString s_membershipRemoved = QStringLiteral("MembershipRemoved");
static const inline QString s_membershipPermissionsChanged = QStringLiteral("MembershipPermissionsChanged");
static const inline QString s_invitationSent = QStringLiteral("InvitationSent");
static const inline QString s_invitationStatusChanged = QStringLiteral("InvitationStatusChanged");
static const inline QString s_ownershipTransferred = QStringLiteral("OwnershipTransferred");
static const inline QString s_connectionRequestCreated = QStringLiteral("ConnectionRequestCreated");
static const inline QString s_connectionRequestStatusChanged = QStringLiteral("ConnectionRequestStatusChanged");
static const inline QString s_connectionRemoved = QStringLiteral("ConnectionRemoved");
static const inline QString s_messageCreated = QStringLiteral("MessageCreated");
static const inline QString s_messageStatusChanged = QStringLiteral("MessageStatusChanged");
static const inline QString s_contractId = QStringLiteral("contractId");
// The organizations a contract event is for, a list of ids
static const inline QString s_recipientTenantIds = QStringLiteral("recipientTenantIds");
// Every change of a contract, for all its parties
static const inline QString s_contractChanged = QStringLiteral("ContractChanged");
static const inline QString s_contractInvitationReceived = QStringLiteral("ContractInvitationReceived");
static const inline QString s_contractUpdated = QStringLiteral("ContractUpdated");
static const inline QString s_contractChangesRequested = QStringLiteral("ContractChangesRequested");
static const inline QString s_contractSigned = QStringLiteral("ContractSigned");
static const inline QString s_contractDeclined = QStringLiteral("ContractDeclined");
static const inline QString s_contractConcluded = QStringLiteral("ContractConcluded");
static const inline QString s_contractCancelled = QStringLiteral("ContractCancelled");
static const inline QString s_contractNoticeGiven = QStringLiteral("ContractNoticeGiven");
static const inline QString s_contractWithdrawn = QStringLiteral("ContractWithdrawn");
static const inline QString s_contractAllocationsSuspended = QStringLiteral("ContractAllocationsSuspended");
static const inline QString s_contractAllocationsResumed = QStringLiteral("ContractAllocationsResumed");
static const inline QString s_contractPermissionsReceived = QStringLiteral("ContractPermissionsReceived");
static const inline QString s_contractSuperseded = QStringLiteral("ContractSuperseded");
static const inline QString s_contractReminder = QStringLiteral("ContractReminder");
static const inline QString s_reminderKey = QStringLiteral("reminderKey");


/**
	Announce an event of the model; inside a change block of the model it joins the notification of that block.
*/
inline void Publish(istd::IChangeable* modelPtr, const QVariantMap& event)
{
	static QAtomicInteger<quint64> s_sequence;

	// The keys sort in the order the events happened
	const QByteArray key = s_keyPrefix + QByteArray::number(s_sequence.fetchAndAddOrdered(1)).rightJustified(20, '0');

	istd::IChangeable::ChangeSet changeSet(istd::IChangeable::CF_ANY);
	changeSet.SetChangeInfo(key, event);

	istd::CChangeNotifier notifier(modelPtr, &changeSet);
}


/**
	The events announced by a change notification, in the order they happened.
*/
inline QList<QVariantMap> GetEvents(const istd::IChangeable::ChangeSet& changeSet)
{
	QList<QVariantMap> retVal;

	const istd::IChangeable::ChangeInfoMap& infoMap = changeSet.GetChangeInfoMap();
	for (auto it = infoMap.constBegin(); it != infoMap.constEnd(); ++it){
		if (it.key().startsWith(s_keyPrefix)){
			retVal.append(it.value().toMap());
		}
	}

	return retVal;
}


} // namespace TenantEvent


} // namespace imtauth


