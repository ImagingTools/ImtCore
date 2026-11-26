// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IContractInfo.h>
#include <imtauth/IContractPartyPrivateInfo.h>
#include <imtauth/ITenantRelationshipProvider.h>


namespace imtauth
{


/**
	State of a whole contract, computed from the facts of the contract and of its parties.
*/
enum ContractState
{
	CS_DRAFT = 0,
	CS_NEGOTIATION,
	CS_SIGNING,
	CS_REJECTED,
	CS_CANCELLED,
	/// Signed by all parties, comes into force later
	CS_SIGNED,
	CS_ACTIVE,
	CS_EXPIRED,
	CS_TERMINATED,
	CS_SUPERSEDED
};


enum ContractStateFlag
{
	CSF_NONE = 0,
	CSF_CHANGES_REQUESTED = 1,
	/// A party leaves, or the contract is terminated, at endsAt
	CSF_TERMINATING = 2,
	/// The current period ends soon
	CSF_EXPIRING = 4,
	/// A party stopped its allocations
	CSF_PARTIALLY_SUSPENDED = 8
};


/**
	The state computed at a time; a result, not stored.
*/
struct ContractStateInfo
{
	ContractState state = CS_DRAFT;
	int flags = CSF_NONE;
	/// Start of the effect, invalid before all parties signed
	QDateTime effectiveStart;
	/// End of the current period, invalid for a contract without an end
	QDateTime periodEnd;
	/// When a party leaves or the contract ends, for CSF_TERMINATING
	QDateTime endsAt;
	/// When a contract that is over ended
	QDateTime endedAt;
};


int FindContractParty(const IContractInfo::Parties& parties, const QByteArray& tenantId);
int FindContractLink(const IContractInfo::Links& links, const QByteArray& linkId);
int FindContractLinkOfPair(const IContractInfo::Links& links, const QByteArray& tenantId, const QByteArray& otherTenantId);
int FindContractAllocation(const IContractInfo::Allocations& allocations, const QByteArray& allocationId);
int FindContractAttachment(const IContractInfo::Attachments& attachments, const QByteArray& attachmentId);
QByteArrayList GetContractPartyTenantIds(const IContractInfo& contract);

/**
	A file attached before the contract was concluded: a part of the terms the parties sign.
*/
bool IsTermsAttachment(const IContractInfo& contract, const IContractInfo::Attachment& attachment);

/**
	The groups a party gives the permissions of an allocation to; none is nobody.
*/
QByteArrayList GetAllocationGroupIds(const IContractPartyPrivateInfo& privateInfo, const QByteArray& allocationId);
void SetAllocationGroupIds(IContractPartyPrivateInfo& privateInfo, const QByteArray& allocationId, const QByteArrayList& groupIds);

/**
	A contract comes into force at its effectiveFrom, otherwise when the last party signed.
*/
QDateTime GetContractEffectiveStart(const IContractInfo& contract);

/**
	End of the period running at \c now: the first end, extended by the renewals the notices of non-renewal allow.
	Invalid for a contract without an end.
*/
QDateTime GetContractPeriodEnd(const IContractInfo& contract, const QDateTime& now);

/**
	A party is bound by the contract at \c time: it signed and had not left yet.
*/
bool IsPartyBoundAt(const IContractInfo& contract, const IContractInfo::Party& party, const QDateTime& time);

ContractStateInfo ComputeContractState(const IContractInfo& contract, const QDateTime& now);

/**
	The contract is in force: only then its links, allocations and exchanges work.
*/
bool IsContractInForce(const IContractInfo& contract, const QDateTime& now);

bool IsLinkInForce(const IContractInfo& contract, const IContractInfo::Link& link, const QDateTime& now);

bool IsAllocationInForce(const IContractInfo& contract, const IContractInfo::Allocation& allocation, const QDateTime& now);

/**
	The time a link of a contract signed by all parties began and, when it is over, ended.
	\return False when the contract binds nobody: not signed by all parties.
*/
bool GetLinkPeriod(const IContractInfo& contract, const IContractInfo::Link& link, const QDateTime& now, RelationshipPeriod& period);

/**
	SHA-256 of what the parties sign: the terms, the parties, the links, the allocations, the references
	and the content of the files of the terms.
*/
QByteArray ComputeContractContentHash(const IContractInfo& contract);


} // namespace imtauth


