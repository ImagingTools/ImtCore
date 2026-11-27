// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/TNullableValue.h>

// ImtCore includes
#include <imtauth/IContractProvider.h>


namespace imtauth
{


/**
	Who acts on a contract: a user on behalf of an organization.
*/
struct ContractActor
{
	QByteArray tenantId;
	QByteArray userId;
	QString userName;
};


struct ContractSignatureRequest
{
	QString position;
};


struct ContractPrivateInfoUpdate
{
	istd::TNullableValue<QString> internalNumber;
	istd::TNullableValue<QByteArray> responsibleUserId;
	istd::TNullableValue<QString> notes;
	istd::TNullableValue<int> reminderDays;
};


/**
	The life of the contracts: the draft of the initiator, the review and the signatures of the parties,
	the leaving of a party and the end of a contract.
	A change of the terms takes the expected revision; a change made meanwhile refuses it.
	The access rights of the actor are checked by the caller, the rules of the contract here.
	\ingroup Tenant
*/
class IContractManager: virtual public IContractProvider
{
public:
	/**
		Create a draft of the organization of the actor, a party of it.
		The new parties must be known to the initiator: in its address book, parties of a common contract or listed in the directory.
		\return The id of the contract, empty when refused.
	*/
	virtual QByteArray CreateContract(const ContractActor& actor, const IContractInfo& draft, QString& errorMessage) = 0;

	/**
		Change the terms. After the contract was sent every approval and signature ends with the change.
	*/
	virtual bool UpdateContract(const ContractActor& actor, const IContractInfo& draft, int expectedRevision, QString& errorMessage) = 0;

	/**
		Add the organization of a connection code as a party.
	*/
	virtual bool AddPartyByCode(
				const ContractActor& actor,
				const QByteArray& contractId,
				int expectedRevision,
				const QString& connectionCode,
				const QByteArray& roleId,
				QString& errorMessage) = 0;

	/**
		Remove a draft that was never sent.
	*/
	virtual bool RemoveContract(const ContractActor& actor, const QByteArray& contractId, QString& errorMessage) = 0;

	virtual bool SubmitContract(const ContractActor& actor, const QByteArray& contractId, int expectedRevision, QString& errorMessage) = 0;
	virtual bool CancelContract(const ContractActor& actor, const QByteArray& contractId, const QString& reason, QString& errorMessage) = 0;
	virtual bool RequestChanges(const ContractActor& actor, const QByteArray& contractId, int expectedRevision, const QString& comment, QString& errorMessage) = 0;
	virtual bool ApproveContract(const ContractActor& actor, const QByteArray& contractId, int expectedRevision, QString& errorMessage) = 0;
	virtual bool SignContract(
				const ContractActor& actor,
				const QByteArray& contractId,
				int expectedRevision,
				const ContractSignatureRequest& signature,
				QString& errorMessage) = 0;
	virtual bool DeclineContract(const ContractActor& actor, const QByteArray& contractId, const QString& comment, QString& errorMessage) = 0;

	/**
		The party leaves the contract after the notice period, or at once when it gives the breach as the reason.
	*/
	virtual bool GiveNotice(const ContractActor& actor, const QByteArray& contractId, const QString& reason, bool isImmediate, QString& errorMessage) = 0;
	virtual bool RevokeNotice(const ContractActor& actor, const QByteArray& contractId, QString& errorMessage) = 0;
	virtual bool GiveNonRenewalNotice(const ContractActor& actor, const QByteArray& contractId, QString& errorMessage) = 0;

	/**
		The organization of the actor gives its own permissions to a party it is linked with; it decides it alone,
		so the terms and the signatures stay. The permissions work while the contract is in force.
		The grantor and the link of \c allocation are set here.
	*/
	virtual bool GiveAllocation(const ContractActor& actor, const QByteArray& contractId, const IContractInfo::Allocation& allocation, QString& errorMessage) = 0;

	/**
		The organization of the actor takes back permissions it gave.
	*/
	virtual bool WithdrawAllocation(const ContractActor& actor, const QByteArray& contractId, const QByteArray& allocationId, QString& errorMessage) = 0;

	/**
		Record an attachment the caller stored. An internal one is seen by the party of the actor only.
		A file attached before the contract is concluded is a part of the terms.
	*/
	virtual bool AddAttachment(
				const ContractActor& actor,
				const QByteArray& contractId,
				const IContractInfo::Attachment& attachment,
				bool isInternal,
				QString& errorMessage) = 0;

	/**
		Change the supplied fields of the records the party keeps for itself.
	*/
	virtual bool SetPrivateInfo(const ContractActor& actor, const QByteArray& contractId, const ContractPrivateInfoUpdate& update, QString& errorMessage) = 0;

	/**
		Give the permissions an allocation gives the party of the actor to its groups; no group is nobody.
	*/
	virtual bool SetAllocationGroups(
				const ContractActor& actor,
				const QByteArray& contractId,
				const QByteArray& allocationId,
				const QByteArrayList& groupIds,
				QString& errorMessage) = 0;

	/**
		Note a reminder sent to the party.
		\return False when it was noted before, or could not be noted.
	*/
	virtual bool RecordReminder(const QByteArray& contractId, const QByteArray& tenantId, const QByteArray& reminderKey) = 0;
};


} // namespace imtauth


