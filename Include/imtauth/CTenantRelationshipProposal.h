// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/ITenantRelationshipProposalInfo.h>


namespace imtauth
{


class CTenantRelationshipProposal: virtual public ITenantRelationshipProposalInfo, virtual public iser::ISerializable
{
public:
CTenantRelationshipProposal();

virtual QByteArray GetProposalId() const override;
virtual void SetProposalId(const QByteArray& proposalId) override;

virtual QByteArray GetConnectionId() const override;
virtual void SetConnectionId(const QByteArray& connectionId) override;

virtual QByteArray GetExistingRelationshipId() const override;
virtual void SetExistingRelationshipId(const QByteArray& existingRelationshipId) override;

virtual RelationshipProposalType GetProposalType() const override;
virtual void SetProposalType(RelationshipProposalType proposalType) override;

virtual QByteArray GetInitiatorTenantId() const override;
virtual void SetInitiatorTenantId(const QByteArray& initiatorTenantId) override;

virtual QByteArray GetCounterpartyTenantId() const override;
virtual void SetCounterpartyTenantId(const QByteArray& counterpartyTenantId) override;

virtual ITenantRelationshipInfo::TenantRelationshipRole GetProposedSourceRole() const override;
virtual void SetProposedSourceRole(ITenantRelationshipInfo::TenantRelationshipRole role) override;

virtual ITenantRelationshipInfo::TenantRelationshipRole GetProposedTargetRole() const override;
virtual void SetProposedTargetRole(ITenantRelationshipInfo::TenantRelationshipRole role) override;

virtual QString GetProposedScope() const override;
virtual void SetProposedScope(const QString& scope) override;

virtual QString GetProposedDescription() const override;
virtual void SetProposedDescription(const QString& description) override;

virtual QString GetProposedValidFrom() const override;
virtual void SetProposedValidFrom(const QString& validFrom) override;

virtual QString GetProposedValidUntil() const override;
virtual void SetProposedValidUntil(const QString& validUntil) override;

virtual RelationshipProposalStatus GetStatus() const override;
virtual void SetStatus(RelationshipProposalStatus status) override;

virtual QString GetMessage() const override;
virtual void SetMessage(const QString& message) override;

virtual QString GetCreatedAt() const override;
virtual void SetCreatedAt(const QString& createdAt) override;

virtual QString GetUpdatedAt() const override;
virtual void SetUpdatedAt(const QString& updatedAt) override;

virtual bool Serialize(iser::IArchive& archive) override;
virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
QByteArray m_proposalId;
QByteArray m_connectionId;
QByteArray m_existingRelationshipId;
RelationshipProposalType m_proposalType;
QByteArray m_initiatorTenantId;
QByteArray m_counterpartyTenantId;
ITenantRelationshipInfo::TenantRelationshipRole m_proposedSourceRole;
ITenantRelationshipInfo::TenantRelationshipRole m_proposedTargetRole;
QString m_proposedScope;
QString m_proposedDescription;
QString m_proposedValidFrom;
QString m_proposedValidUntil;
RelationshipProposalStatus m_status;
QString m_message;
QString m_createdAt;
QString m_updatedAt;
};


typedef imtbase::TIdentifiableWrap<CTenantRelationshipProposal> CIdentifiableTenantRelationshipProposal;


} // namespace imtauth
