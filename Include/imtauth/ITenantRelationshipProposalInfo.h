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
Standalone data model for a relationship proposal.
Proposals implement a two-sided approval workflow for creating,
updating, or deleting Relationships.

\ingroup Tenant
*/
class ITenantRelationshipProposalInfo: virtual public iser::IObject
{
public:
enum RelationshipProposalType
{
RPT_CREATE = 0,
RPT_UPDATE,
RPT_DELETE
};

I_DECLARE_ENUM(RelationshipProposalType, RPT_CREATE, RPT_UPDATE, RPT_DELETE);

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

I_DECLARE_ENUM(RelationshipProposalStatus, RPS_PENDING, RPS_APPROVED_BY_INITIATOR, RPS_APPROVED_BY_COUNTERPARTY, RPS_REJECTED, RPS_CANCELED, RPS_EXPIRED, RPS_APPLIED);

virtual QByteArray GetProposalId() const = 0;
virtual void SetProposalId(const QByteArray& proposalId) = 0;

virtual QByteArray GetConnectionId() const = 0;
virtual void SetConnectionId(const QByteArray& connectionId) = 0;

virtual QByteArray GetExistingRelationshipId() const = 0;
virtual void SetExistingRelationshipId(const QByteArray& existingRelationshipId) = 0;

virtual RelationshipProposalType GetProposalType() const = 0;
virtual void SetProposalType(RelationshipProposalType proposalType) = 0;

virtual QByteArray GetInitiatorTenantId() const = 0;
virtual void SetInitiatorTenantId(const QByteArray& initiatorTenantId) = 0;

virtual QByteArray GetCounterpartyTenantId() const = 0;
virtual void SetCounterpartyTenantId(const QByteArray& counterpartyTenantId) = 0;

virtual ITenantRelationshipInfo::TenantRelationshipRole GetProposedSourceRole() const = 0;
virtual void SetProposedSourceRole(ITenantRelationshipInfo::TenantRelationshipRole role) = 0;

virtual ITenantRelationshipInfo::TenantRelationshipRole GetProposedTargetRole() const = 0;
virtual void SetProposedTargetRole(ITenantRelationshipInfo::TenantRelationshipRole role) = 0;

virtual QString GetProposedScope() const = 0;
virtual void SetProposedScope(const QString& scope) = 0;

virtual QString GetProposedDescription() const = 0;
virtual void SetProposedDescription(const QString& description) = 0;

virtual QString GetProposedValidFrom() const = 0;
virtual void SetProposedValidFrom(const QString& validFrom) = 0;

virtual QString GetProposedValidUntil() const = 0;
virtual void SetProposedValidUntil(const QString& validUntil) = 0;

virtual RelationshipProposalStatus GetStatus() const = 0;
virtual void SetStatus(RelationshipProposalStatus status) = 0;

virtual QString GetMessage() const = 0;
virtual void SetMessage(const QString& message) = 0;

virtual QString GetCreatedAt() const = 0;
virtual void SetCreatedAt(const QString& createdAt) = 0;

virtual QString GetUpdatedAt() const = 0;
virtual void SetUpdatedAt(const QString& updatedAt) = 0;
};


typedef istd::TUniqueInterfacePtr<ITenantRelationshipProposalInfo> ITenantRelationshipProposalInfoUniquePtr;
typedef istd::TSharedInterfacePtr<ITenantRelationshipProposalInfo> ITenantRelationshipProposalInfoSharedPtr;


} // namespace imtauth
