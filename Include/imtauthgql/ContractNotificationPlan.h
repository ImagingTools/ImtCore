// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArrayList>
#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QVariantMap>

// ACF includes
#include <istd/IInformationProvider.h>

// ImtCore includes
#include <imtauth/IContractInfo.h>


namespace imtauthgql
{


/**
	What an event of a contract changes in the inboxes of its parties: the notifications it makes untrue and the ones
	it sends. Decided from the event and the contract alone, so CContractNotificationPublisherComp only carries it out.

	Groups of a contract, one row per inbox:
	- pending: what a party is to answer (invitation, changes asked of the initiator, changed terms);
	- signed: the signatures the initiator collects;
	- notice: the notice of one party to leave.
*/
namespace ContractNotificationPlan
{


struct Withdrawal
{
	QByteArray tenantId;
	QByteArray groupKey;
};


struct Delivery
{
	QByteArray tenantId;
	QByteArray typeId;
	istd::IInformationProvider::InformationCategory severity = istd::IInformationProvider::IC_INFO;
	QByteArray groupKey;
	/// The party that acted; empty for what the contract does by itself
	QByteArray partnerTenantId;
	/// False when the organization cannot open the contract any more
	bool isSubjectLinked = true;
	/// The date a reminder is about
	QDateTime date;
};


struct Plan
{
	QList<Withdrawal> withdrawals;
	QList<Delivery> deliveries;
};


QByteArray GetPendingGroupKey(const QByteArray& contractId);
QByteArray GetSignedGroupKey(const QByteArray& contractId);
QByteArray GetNoticeGroupKey(const QByteArray& contractId, const QByteArray& partyTenantId);

/**
	The plan of an event announced by imtauth::TenantEvent::Publish; contract is the contract after the event.
	The withdrawals come first: a delivery may open the group they close.
*/
Plan CreatePlan(const QVariantMap& event, const imtauth::IContractInfo& contract);

/**
	The users of a party told about its contract: the responsible user while a member, otherwise the owner;
	never the user who acted.
*/
QByteArrayList SelectRecipients(const QByteArray& ownerId, const QByteArray& responsibleUserId, bool isResponsibleMember, const QByteArray& actorUserId);


} // namespace ContractNotificationPlan


} // namespace imtauthgql
