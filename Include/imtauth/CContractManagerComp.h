// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QHash>
#include <QtCore/QMutex>
#include <QtCore/QVariantMap>

// STL includes
#include <functional>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/IContractManager.h>
#include <imtauth/IContractTypeCatalog.h>
#include <imtauth/IPartyRoleCatalog.h>
#include <imtauth/ITenantConnectionRequestManager.h>
#include <imtauth/ITenantDirectory.h>
#include <imtauth/ITenantEntityBindingManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtauth/ITenantManager.h>
#include <imtbase/IObjectCollection.h>


namespace imtauth
{


/**
	The contracts of the organizations in three collections: the contracts, the records of the parties and the journal.
	It is the only writer of them, so it keeps what it read until it writes it again.
*/
class CContractManagerComp:
			public ilog::CLoggerComponentBase,
			virtual public IContractManager
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CContractManagerComp);
		I_REGISTER_INTERFACE(IContractManager);
		I_REGISTER_INTERFACE(IContractProvider);
		I_ASSIGN(m_contractCollectionCompPtr, "ContractCollection", "Contracts", true, "ContractCollection");
		I_ASSIGN(m_contractFactoryCompPtr, "ContractFactory", "Factory of a contract", true, "ContractInfo");
		I_ASSIGN(m_privateCollectionCompPtr, "PrivateInfoCollection", "Records the parties keep for themselves", true, "ContractPartyPrivateCollection");
		I_ASSIGN(m_privateFactoryCompPtr, "PrivateInfoFactory", "Factory of a record of a party", true, "ContractPartyPrivateInfo");
		I_ASSIGN(m_eventCollectionCompPtr, "EventCollection", "Journals of the contracts", true, "ContractEventCollection");
		I_ASSIGN(m_eventFactoryCompPtr, "EventFactory", "Factory of an entry of a journal", true, "ContractEventInfo");
		I_ASSIGN(m_tenantManagerCompPtr, "TenantManager", "Organizations: names and the permissions they may give", true, "TenantManager");
		I_ASSIGN(m_roleCatalogCompPtr, "PartyRoleCatalog", "Roles of the parties", true, "PartyRoleCatalog");
		I_ASSIGN(m_typeCatalogCompPtr, "ContractTypeCatalog", "Types of the contracts", true, "ContractTypeCatalog");
		I_ASSIGN(m_connectionManagerCompPtr, "ConnectionManager", "Address books and connection codes of the organizations", true, "TenantConnectionRequestManager");
		I_ASSIGN(m_directoryCompPtr, "TenantDirectory", "Organizations that accept contracts of unknown ones", true, "TenantDirectory");
		I_ASSIGN(m_bindingManagerCompPtr, "TenantEntityBindingManager", "The organizations of the groups", true, "TenantEntityBindingManager");
		I_ASSIGN(m_membershipManagerCompPtr, "MembershipManager", "Members of the organizations: who may be responsible for a contract", true, "TenantMembershipManager");
		I_ASSIGN(m_invitationLimitAttrPtr, "UnknownInvitationsPerHour", "Organizations outside of its address book an organization invites into contracts per hour", true, 30);
	I_END_COMPONENT;

	// reimplemented (imtauth::IContractProvider)
	virtual IContractInfoUniquePtr GetContract(const QByteArray& contractId) const override;
	virtual ContractInfoList GetContractsOfParty(const QByteArray& tenantId, bool* isReadPtr = nullptr) const override;
	virtual ContractInfoList GetContractsOfPair(const QByteArray& tenantId, const QByteArray& otherTenantId, bool* isReadPtr = nullptr) const override;
	virtual ContractInfoList GetOpenContracts(bool* isReadPtr = nullptr) const override;
	virtual IContractPartyPrivateInfoUniquePtr GetPrivateInfo(const QByteArray& contractId, const QByteArray& tenantId) const override;
	virtual ContractEventInfoList GetEvents(const QByteArray& contractId, const QByteArray& tenantId, bool* isReadPtr = nullptr) const override;

	// reimplemented (imtauth::IContractManager)
	virtual QByteArray CreateContract(const ContractActor& actor, const IContractInfo& draft, QString& errorMessage) override;
	virtual bool UpdateContract(const ContractActor& actor, const IContractInfo& draft, int expectedRevision, QString& errorMessage) override;
	virtual bool AddPartyByCode(
				const ContractActor& actor,
				const QByteArray& contractId,
				int expectedRevision,
				const QString& connectionCode,
				const QByteArray& roleId,
				QString& errorMessage) override;
	virtual bool RemoveContract(const ContractActor& actor, const QByteArray& contractId, QString& errorMessage) override;
	virtual bool SubmitContract(const ContractActor& actor, const QByteArray& contractId, int expectedRevision, QString& errorMessage) override;
	virtual bool CancelContract(const ContractActor& actor, const QByteArray& contractId, const QString& reason, QString& errorMessage) override;
	virtual bool RequestChanges(const ContractActor& actor, const QByteArray& contractId, int expectedRevision, const QString& comment, QString& errorMessage) override;
	virtual bool ApproveContract(const ContractActor& actor, const QByteArray& contractId, int expectedRevision, QString& errorMessage) override;
	virtual bool SignContract(
				const ContractActor& actor,
				const QByteArray& contractId,
				int expectedRevision,
				const ContractSignatureRequest& signature,
				QString& errorMessage) override;
	virtual bool DeclineContract(const ContractActor& actor, const QByteArray& contractId, const QString& comment, QString& errorMessage) override;
	virtual bool GiveNotice(const ContractActor& actor, const QByteArray& contractId, const QString& reason, bool isImmediate, QString& errorMessage) override;
	virtual bool RevokeNotice(const ContractActor& actor, const QByteArray& contractId, QString& errorMessage) override;
	virtual bool GiveNonRenewalNotice(const ContractActor& actor, const QByteArray& contractId, QString& errorMessage) override;
	virtual bool GiveAllocation(const ContractActor& actor, const QByteArray& contractId, const IContractInfo::Allocation& allocation, QString& errorMessage) override;
	virtual bool WithdrawAllocation(const ContractActor& actor, const QByteArray& contractId, const QByteArray& allocationId, QString& errorMessage) override;
	virtual bool AddAttachment(
				const ContractActor& actor,
				const QByteArray& contractId,
				const IContractInfo::Attachment& attachment,
				bool isInternal,
				QString& errorMessage) override;
	virtual bool SetPrivateInfo(const ContractActor& actor, const QByteArray& contractId, const ContractPrivateInfoUpdate& update, QString& errorMessage) override;
	virtual bool SetAllocationGroups(
				const ContractActor& actor,
				const QByteArray& contractId,
				const QByteArray& allocationId,
				const QByteArrayList& groupIds,
				QString& errorMessage) override;
	virtual bool RecordReminder(const QByteArray& contractId, const QByteArray& tenantId, const QByteArray& reminderKey) override;

private:
	/**
		An entry of the journal to store with a change.
	*/
	struct JournalEntry
	{
		IContractEventInfo::EventType type = IContractEventInfo::ET_UPDATED;
		QByteArray tenantId;
		QByteArray userId;
		QString userName;
		QString comment;
		QByteArray subjectId;
		bool isPrivate = false;
	};

	/**
		What a change of a contract announces: the entries of the journal and the events for the other organizations.
	*/
	struct ChangeRecord
	{
		QList<JournalEntry> journal;
		QList<QVariantMap> events;
		bool isConcluded = false;
		// Unknown organizations the change invites: counted once it is stored
		int unknownInvitations = 0;
	};

	typedef std::function<bool(IContractInfo& contract, ChangeRecord& record, QString& errorMessage)> ContractChange;

	/**
		Read the contract, check that the organization of the actor is a party, change it, store it and announce the change.
	*/
	bool ChangeContract(const ContractActor& actor, const QByteArray& contractId, const ContractChange& change, QString& errorMessage);

	IContractInfoUniquePtr CopyContract(const IContractInfo& contract) const;
	IContractInfoUniquePtr ReadContract(const QByteArray& contractId) const;
	bool StoreContract(const IContractInfo& contract, bool isNew);
	IContractPartyPrivateInfoUniquePtr ReadPrivateInfo(const QByteArray& contractId, const QByteArray& tenantId) const;
	bool StorePrivateInfo(const IContractPartyPrivateInfo& privateInfo);
	void Announce(const IContractInfo& contract, const ChangeRecord& record);

	/**
		Take the terms, the parties and the links of the draft into the contract.
		A party new to the contract must be known to the initiator.
	*/
	bool ApplyDraft(const IContractInfo& draft, IContractInfo& contract, QString& errorMessage) const;

	/**
		Links a party added by the server as the type links its parties: with the center of a star, or with every other party.
	*/
	void AddSchemeLinks(IContractInfo& contract, const QByteArray& tenantId) const;
	bool CheckStructure(const IContractInfo& contract, QString& errorMessage) const;
	bool CheckSendable(const IContractInfo& contract, QString& errorMessage) const;
	bool CheckReferences(const IContractInfo& contract, QString& errorMessage) const;
	/// The owner, the creator or an active member of the organization
	bool IsTenantUser(const QByteArray& tenantId, const QByteArray& userId) const;
	/**
		The grantor holds the permissions it gives, and a framework contract it is concluded under gives them too.
	*/
	bool CheckAllocationScope(const IContractInfo& contract, const IContractInfo::Allocation& allocation, QString& errorMessage) const;

	/**
		A party gives and takes back its permissions until the contract ends, unless it declined or left it.
	*/
	bool CanPartyChangeAllocations(const IContractInfo& contract, const QByteArray& tenantId, QString& errorMessage) const;
	/// The terms changed: a sent contract is reviewed again, every approval and signature ends
	void RestartReview(IContractInfo& contract) const;
	/// The last signature was given: the contract is concluded
	bool Conclude(IContractInfo& contract, ChangeRecord& record, QString& errorMessage);
	/// After the concluded contract was stored: it replaces the contracts it supersedes, its parties get connected
	void CompleteConclusion(const IContractInfo& contract);

	/**
		How the initiator knows an organization: false when it does not.
	*/
	bool FindChannel(const QByteArray& initiatorTenantId, const QByteArray& tenantId, IContractInfo::PartyChannel& channel) const;
	bool HasCommonContract(const QByteArray& tenantId, const QByteArray& otherTenantId) const;
	bool CheckInvitationLimit(const QByteArray& initiatorTenantId, int unknownCount, QString& errorMessage);
	void RecordInvitations(const QByteArray& initiatorTenantId, int unknownCount);
	/// The name of a role usable by the initiator, empty when it is not
	QString FindRoleName(const QByteArray& roleId, const QByteArray& initiatorTenantId, IPartyRoleInfo::Applicability use) const;
	QString FindTenantName(const QByteArray& tenantId) const;
	QString CreateNumber(const IContractInfo& contract) const;

	static bool CheckRevision(const IContractInfo& contract, int expectedRevision, QString& errorMessage);
	static JournalEntry CreateEntry(const ContractActor& actor, IContractEventInfo::EventType type, const QString& comment = QString());
	static QVariantMap CreateTenantEvent(const QString& type, const IContractInfo& contract, const QByteArray& actorTenantId, const QByteArrayList& recipientTenantIds);

private:
	I_REF(imtbase::IObjectCollection, m_contractCollectionCompPtr);
	I_FACT(IContractInfo, m_contractFactoryCompPtr);
	I_REF(imtbase::IObjectCollection, m_privateCollectionCompPtr);
	I_FACT(IContractPartyPrivateInfo, m_privateFactoryCompPtr);
	I_REF(imtbase::IObjectCollection, m_eventCollectionCompPtr);
	I_FACT(IContractEventInfo, m_eventFactoryCompPtr);
	I_REF(ITenantManager, m_tenantManagerCompPtr);
	I_REF(IPartyRoleCatalog, m_roleCatalogCompPtr);
	I_REF(IContractTypeCatalog, m_typeCatalogCompPtr);
	I_REF(ITenantConnectionRequestManager, m_connectionManagerCompPtr);
	I_REF(ITenantDirectory, m_directoryCompPtr);
	I_REF(ITenantEntityBindingManager, m_bindingManagerCompPtr);
	I_REF(ITenantMembershipManager, m_membershipManagerCompPtr);
	I_ATTR(int, m_invitationLimitAttrPtr);

	// One change of the contracts at a time: a check and its change must see the same contract
	QMutex m_changeMutex;

	// What was read or written, given out as copies
	mutable QMutex m_cacheMutex;
	mutable QHash<QByteArray, IContractInfoSharedPtr> m_contractCache;
	mutable QHash<QByteArray, QByteArrayList> m_partyContractIdsCache;
	mutable QHash<QByteArray, IContractPartyPrivateInfoSharedPtr> m_privateInfoCache;
	// Advanced by every change: a read that started before one does not put its older copy into the caches
	mutable quint64 m_cacheGeneration = 0;

	// The invitations of unknown organizations of an initiator in the last hour
	QHash<QByteArray, QList<QDateTime>> m_unknownInvitations;
};


} // namespace imtauth


