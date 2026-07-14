// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/ITenantRelationshipInfo.h>


namespace imtauth
{


class CTenantRelationshipInfo: virtual public ITenantRelationshipInfo, virtual public iser::ISerializable
{
public:
	CTenantRelationshipInfo();

	// reimplemented (imtauth::ITenantRelationshipInfo)
	virtual QByteArray GetRelationshipId() const override;
	virtual void SetRelationshipId(const QByteArray& relationshipId) override;
	virtual QByteArray GetConnectionId() const override;
	virtual void SetConnectionId(const QByteArray& connectionId) override;
	virtual QByteArray GetSourceTenantId() const override;
	virtual void SetSourceTenantId(const QByteArray& sourceTenantId) override;
	virtual QByteArray GetTargetTenantId() const override;
	virtual void SetTargetTenantId(const QByteArray& targetTenantId) override;
	virtual TenantRelationshipRole GetSourceRole() const override;
	virtual void SetSourceRole(TenantRelationshipRole role) override;
	virtual TenantRelationshipRole GetTargetRole() const override;
	virtual void SetTargetRole(TenantRelationshipRole role) override;
	virtual QString GetScope() const override;
	virtual void SetScope(const QString& scope) override;
	virtual QString GetValidFrom() const override;
	virtual void SetValidFrom(const QString& validFrom) override;
	virtual QString GetValidUntil() const override;
	virtual void SetValidUntil(const QString& validUntil) override;
	virtual TenantRelationshipStatus GetStatus() const override;
	virtual void SetStatus(TenantRelationshipStatus status) override;
	virtual QString GetDescription() const override;
	virtual void SetDescription(const QString& description) override;
	virtual QString GetCreatedAt() const override;
	virtual void SetCreatedAt(const QString& createdAt) override;
	virtual QString GetUpdatedAt() const override;
	virtual void SetUpdatedAt(const QString& updatedAt) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QByteArray m_relationshipId;
	QByteArray m_connectionId;
	QByteArray m_sourceTenantId;
	QByteArray m_targetTenantId;
	TenantRelationshipRole m_sourceRole;
	TenantRelationshipRole m_targetRole;
	QString m_scope;
	QString m_validFrom;
	QString m_validUntil;
	TenantRelationshipStatus m_status;
	QString m_description;
	QString m_createdAt;
	QString m_updatedAt;
};


typedef imtbase::TIdentifiableWrap<CTenantRelationshipInfo> CIdentifiableTenantRelationshipInfo;


} // namespace imtauth
