// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>
#include <istd/TSmartPtr.h>

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>

// ImtCore includes
#include <imtauth/ITenantRelationshipInfo.h>


namespace imtauth
{


/**
	Type of relationship proposal.
*/
enum RelationshipProposalType
{
	RPT_CREATE = 0,
	RPT_UPDATE,
	RPT_DELETE
};


/**
	Status of a relationship proposal.
*/
enum RelationshipProposalStatus
{
	RPS_PENDING = 0,
	RPS_APPROVED_BY_INITIATOR,
	RPS_APPROVED_BY_COUNTERPARTY,
	RPS_REJECTED,
	RPS_CANCELED,
	RPS_EXPIRED,
	RPS_APPLIED
};


/**
	Structure describing a relationship proposal.
*/
struct RelationshipProposalInfo
{
	QByteArray proposalId;
	QByteArray connectionId;
	QByteArray existingRelationshipId;
	RelationshipProposalType proposalType = RPT_CREATE;
	QByteArray initiatorTenantId;
	QByteArray counterpartyTenantId;
	ITenantRelationshipInfo::TenantRelationshipRole proposedSourceRole = ITenantRelationshipInfo::TRR_PARTNER;
	ITenantRelationshipInfo::TenantRelationshipRole proposedTargetRole = ITenantRelationshipInfo::TRR_PARTNER;
	QString proposedScope;
	QString proposedDescription;
	QString proposedValidFrom;
	QString proposedValidUntil;
	RelationshipProposalStatus status = RPS_PENDING;
	QString message;
	QString createdAt;
	QString updatedAt;

	bool operator==(const RelationshipProposalInfo& other) const
	{
		return proposalId == other.proposalId
				&& connectionId == other.connectionId
				&& proposalType == other.proposalType
				&& status == other.status;
	}

	bool operator!=(const RelationshipProposalInfo& other) const
	{
		return !(*this == other);
	}
};

typedef QList<RelationshipProposalInfo> RelationshipProposals;


/**
Serializable data holder for a relationship proposal.
Used as the stored representation inside an object collection
(e.g. an SQL-backed repository).
*/
class ITenantRelationshipProposalData: virtual public iser::IObject
{
public:
	I_DECLARE_ENUM(RelationshipProposalType, RPT_CREATE, RPT_UPDATE, RPT_DELETE);
	I_DECLARE_ENUM(RelationshipProposalStatus, RPS_PENDING, RPS_APPROVED_BY_INITIATOR, RPS_APPROVED_BY_COUNTERPARTY, RPS_REJECTED, RPS_CANCELED, RPS_EXPIRED, RPS_APPLIED);

	virtual RelationshipProposalInfo GetProposalInfo() const = 0;
	virtual void SetProposalInfo(const RelationshipProposalInfo& proposalInfo) = 0;
	virtual QByteArray GetProposalId() const = 0;
};


typedef istd::TUniqueInterfacePtr<ITenantRelationshipProposalData> ITenantRelationshipProposalDataUniquePtr;
typedef istd::TSharedInterfacePtr<ITenantRelationshipProposalData> ITenantRelationshipProposalDataSharedPtr;


} // namespace imtauth
