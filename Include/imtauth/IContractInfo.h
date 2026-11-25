// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArrayList>
#include <QtCore/QDateTime>
#include <QtCore/QList>

// ACF includes
#include <iser/IObject.h>
#include <istd/TSmartPtr.h>


namespace imtauth
{


/**
	A contract of organizations: its terms, signed by all parties, and the facts of its life.
	The state is not stored but computed from the facts (\see ComputeContractState).
	\ingroup Tenant
*/
class IContractInfo: virtual public iser::IObject
{
public:
	/**
		State of a party; the state of the whole contract is computed from the states of its parties (\see ComputeContractState).
	*/
	enum PartyState
	{
		/// In the draft, not sent yet
		PS_INVITED = 0,
		/// Received, under review
		PS_REVIEWING,
		/// Asks to change the terms
		PS_CHANGES_REQUESTED,
		/// Agreed to the terms inside the party, not signed yet
		PS_APPROVED,
		/// The required signatures of this revision are given
		PS_SIGNED,
		/// Refuses to sign
		PS_DECLINED,
		/// Leaves the contract at the end of the notice period
		PS_NOTICE_GIVEN,
		/// Left the contract
		PS_WITHDRAWN
	};

	I_DECLARE_ENUM(PartyState, PS_INVITED, PS_REVIEWING, PS_CHANGES_REQUESTED, PS_APPROVED, PS_SIGNED, PS_DECLINED, PS_NOTICE_GIVEN, PS_WITHDRAWN);

	enum SignatureMethod
	{
		SM_IN_SYSTEM = 0,
		/// A scan of the signed paper is attached
		SM_SCAN,
		/// An electronic signature given outside the system
		SM_EXTERNAL
	};

	I_DECLARE_ENUM(SignatureMethod, SM_IN_SYSTEM, SM_SCAN, SM_EXTERNAL);

	enum ReferenceType
	{
		/// Replaces the referenced contract when it comes into force: renewal, amendment, accession of a party
		RT_SUPERSEDES = 0,
		/// Concluded under the referenced framework contract
		RT_FRAMEWORK,
		/// For information only
		RT_RELATED
	};

	I_DECLARE_ENUM(ReferenceType, RT_SUPERSEDES, RT_FRAMEWORK, RT_RELATED);

	enum AttachmentKind
	{
		AK_MAIN_TEXT = 0,
		AK_APPENDIX,
		AK_SIGNED_COPY,
		AK_AMENDMENT_DRAFT,
		AK_OTHER
	};

	I_DECLARE_ENUM(AttachmentKind, AK_MAIN_TEXT, AK_APPENDIX, AK_SIGNED_COPY, AK_AMENDMENT_DRAFT, AK_OTHER);

	enum SignOrder
	{
		SO_PARALLEL = 0,
		/// A party signs after all parties with a lower order
		SO_SEQUENTIAL
	};

	I_DECLARE_ENUM(SignOrder, SO_PARALLEL, SO_SEQUENTIAL);

	/**
		How the initiator reached a party: an organization never sees a contract of an organization it does not know.
	*/
	enum PartyChannel
	{
		PC_INITIATOR = 0,
		PC_ADDRESS_BOOK,
		PC_CONNECTION_CODE,
		PC_DIRECTORY,
		PC_CO_PARTY
	};

	I_DECLARE_ENUM(PartyChannel, PC_INITIATOR, PC_ADDRESS_BOOK, PC_CONNECTION_CODE, PC_DIRECTORY, PC_CO_PARTY);

	struct Signature
	{
		QByteArray userId;
		QString userName;
		QString position;
		QDateTime signedAt;
		int revision = 0;
		QByteArray contentHash;
		SignatureMethod method = SM_IN_SYSTEM;
		QByteArray externalSignatureId;
		QByteArray attachmentId;

		bool operator==(const Signature& other) const;
		bool operator!=(const Signature& other) const;
	};

	typedef QList<Signature> Signatures;

	struct Party
	{
		QByteArray tenantId;
		QString tenantName;
		QByteArray roleId;
		QString roleName;
		int signOrder = 0;
		int requiredSignatures = 1;
		PartyChannel channel = PC_ADDRESS_BOOK;
		PartyState state = PS_INVITED;
		QString stateComment;
		QDateTime stateChangedAt;
		Signatures signatures;
		/// When a party that gave notice leaves, or left
		QDateTime withdrawAt;
		QString withdrawReason;
		/// The party does not want the contract to be renewed again
		QDateTime nonRenewalNoticeAt;
		/// The party stopped its own allocations
		bool allocationsSuspended = false;
		/// The party agrees to the proposed termination
		bool terminationConsent = false;

		bool operator==(const Party& other) const;
		bool operator!=(const Party& other) const;
	};

	typedef QList<Party> Parties;

	/**
		The relationship of two parties: their roles towards each other, and the base of their exchange.
	*/
	struct Link
	{
		QByteArray linkId;
		QByteArray tenantAId;
		QByteArray tenantBId;
		QByteArray roleAId;
		QString roleAName;
		QByteArray roleBId;
		QString roleBName;
		QDateTime validFrom;
		QDateTime validUntil;
		bool isMessagingAllowed = true;
		bool isSharingAllowed = true;

		bool Connects(const QByteArray& tenantId, const QByteArray& otherTenantId) const;
		bool operator==(const Link& other) const;
		bool operator!=(const Link& other) const;
	};

	typedef QList<Link> Links;

	/**
		Permissions one party of a link gives the other one in its space, for one product.
		The grantee gives them to its own groups (\see IContractPartyPrivateInfo).
	*/
	struct Allocation
	{
		QByteArray allocationId;
		QByteArray linkId;
		QByteArray grantorTenantId;
		QByteArray granteeTenantId;
		QByteArray productId;
		QByteArrayList permissionIds;
		QDateTime validFrom;
		QDateTime validUntil;
		QString comment;

		bool operator==(const Allocation& other) const;
		bool operator!=(const Allocation& other) const;
	};

	typedef QList<Allocation> Allocations;

	struct Reference
	{
		QByteArray contractId;
		ReferenceType type = RT_RELATED;
		QString contractNumber;
		QString contractName;

		bool operator==(const Reference& other) const;
		bool operator!=(const Reference& other) const;
	};

	typedef QList<Reference> References;

	struct Attachment
	{
		QByteArray attachmentId;
		AttachmentKind kind = AK_OTHER;
		QString fileName;
		QString mimeType;
		qint64 size = 0;
		/// SHA-256 of the content, hex
		QByteArray contentHash;
		QByteArray addedByTenantId;
		QByteArray addedByUserId;
		QDateTime addedAt;
		/// The earlier version this one replaces
		QByteArray replacesAttachmentId;
		/// A later version replaces this one
		bool isReplaced = false;

		bool operator==(const Attachment& other) const;
		bool operator!=(const Attachment& other) const;
	};

	typedef QList<Attachment> Attachments;

	/**
		How many parties of a role a contract of a type has; a maximum of 0 is no limit.
	*/
	struct RoleSlot
	{
		QByteArray roleId;
		int minCount = 0;
		int maxCount = 0;

		bool operator==(const RoleSlot& other) const;
		bool operator!=(const RoleSlot& other) const;
	};

	typedef QList<RoleSlot> RoleSlots;

	struct TerminationProposal
	{
		QByteArray proposedByTenantId;
		QDateTime proposedAt;
		QDateTime terminateAt;
		QString reason;

		bool IsValid() const;
		bool operator==(const TerminationProposal& other) const;
		bool operator!=(const TerminationProposal& other) const;
	};

	virtual QByteArray GetContractId() const = 0;
	virtual void SetContractId(const QByteArray& value) = 0;

	virtual QString GetNumber() const = 0;
	virtual void SetNumber(const QString& value) = 0;

	virtual QString GetName() const = 0;
	virtual void SetName(const QString& value) = 0;

	virtual QString GetDescription() const = 0;
	virtual void SetDescription(const QString& value) = 0;

	virtual QString GetTerms() const = 0;
	virtual void SetTerms(const QString& value) = 0;

	virtual QByteArray GetTypeId() const = 0;
	virtual void SetTypeId(const QByteArray& value) = 0;

	virtual QString GetTypeName() const = 0;
	virtual void SetTypeName(const QString& value) = 0;

	virtual QByteArray GetInitiatorTenantId() const = 0;
	virtual void SetInitiatorTenantId(const QByteArray& value) = 0;

	virtual QByteArray GetCreatedByUserId() const = 0;
	virtual void SetCreatedByUserId(const QByteArray& value) = 0;

	/**
		Start of the effect; invalid: the time of the last signature
	*/
	virtual QDateTime GetEffectiveFrom() const = 0;
	virtual void SetEffectiveFrom(const QDateTime& value) = 0;

	/**
		Length of the first period in days, 0 when the contract has a fixed end or none
	*/
	virtual int GetTermDays() const = 0;
	virtual void SetTermDays(int value) = 0;

	/**
		Fixed end of the first period, before the term in days
	*/
	virtual QDateTime GetValidUntil() const = 0;
	virtual void SetValidUntil(const QDateTime& value) = 0;

	virtual bool IsAutoRenew() const = 0;
	virtual void SetAutoRenew(bool value) = 0;

	virtual int GetRenewalPeriodDays() const = 0;
	virtual void SetRenewalPeriodDays(int value) = 0;

	/**
		A notice of non-renewal is given at least so many days before the end of a period
	*/
	virtual int GetRenewalNoticeDays() const = 0;
	virtual void SetRenewalNoticeDays(int value) = 0;

	/**
		A party leaves so many days after its notice
	*/
	virtual int GetTerminationNoticeDays() const = 0;
	virtual void SetTerminationNoticeDays(int value) = 0;

	/**
		The offer lapses unless all parties signed before
	*/
	virtual QDateTime GetSignDeadline() const = 0;
	virtual void SetSignDeadline(const QDateTime& value) = 0;

	virtual SignOrder GetSignOrder() const = 0;
	virtual void SetSignOrder(SignOrder value) = 0;

	virtual Parties GetParties() const = 0;
	virtual void SetParties(const Parties& value) = 0;

	virtual Links GetLinks() const = 0;
	virtual void SetLinks(const Links& value) = 0;

	virtual Allocations GetAllocations() const = 0;
	virtual void SetAllocations(const Allocations& value) = 0;

	virtual References GetReferences() const = 0;
	virtual void SetReferences(const References& value) = 0;

	virtual Attachments GetAttachments() const = 0;
	virtual void SetAttachments(const Attachments& value) = 0;

	/**
		The slots of the type the contract was made of
	*/
	virtual RoleSlots GetRoleSlots() const = 0;
	virtual void SetRoleSlots(const RoleSlots& value) = 0;

	/**
		The kinds of attachments the type needs
	*/
	virtual QList<int> GetRequiredAttachmentKinds() const = 0;
	virtual void SetRequiredAttachmentKinds(const QList<int>& value) = 0;

	virtual int GetRevision() const = 0;
	virtual void SetRevision(int value) = 0;

	virtual QByteArray GetContentHash() const = 0;
	virtual void SetContentHash(const QByteArray& value) = 0;

	virtual QDateTime GetSubmittedAt() const = 0;
	virtual void SetSubmittedAt(const QDateTime& value) = 0;

	virtual QDateTime GetCancelledAt() const = 0;
	virtual void SetCancelledAt(const QDateTime& value) = 0;

	/**
		All parties signed
	*/
	virtual QDateTime GetSignedAt() const = 0;
	virtual void SetSignedAt(const QDateTime& value) = 0;

	virtual QDateTime GetTerminatedAt() const = 0;
	virtual void SetTerminatedAt(const QDateTime& value) = 0;

	virtual QString GetTerminationReason() const = 0;
	virtual void SetTerminationReason(const QString& value) = 0;

	virtual TerminationProposal GetTerminationProposal() const = 0;
	virtual void SetTerminationProposal(const TerminationProposal& value) = 0;

	virtual QByteArray GetSupersededBy() const = 0;
	virtual void SetSupersededBy(const QByteArray& value) = 0;

	virtual QDateTime GetSupersededAt() const = 0;
	virtual void SetSupersededAt(const QDateTime& value) = 0;

	virtual QDateTime GetCreatedAt() const = 0;
	virtual void SetCreatedAt(const QDateTime& value) = 0;

	virtual QDateTime GetUpdatedAt() const = 0;
	virtual void SetUpdatedAt(const QDateTime& value) = 0;
};


typedef istd::TUniqueInterfacePtr<IContractInfo> IContractInfoUniquePtr;
typedef istd::TSharedInterfacePtr<IContractInfo> IContractInfoSharedPtr;
typedef QList<IContractInfoSharedPtr> ContractInfoList;


} // namespace imtauth


