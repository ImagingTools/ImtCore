// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtauth/ITenantInvitation.h>


namespace imtauth
{


class CTenantInvitation: virtual public ITenantInvitation, virtual public iser::ISerializable
{
public:
	CTenantInvitation();

	virtual QByteArray GetInvitationId() const override;
	virtual void SetInvitationId(const QByteArray& invitationId) override;
	virtual QByteArray GetUserId() const override;
	virtual void SetUserId(const QByteArray& userId) override;
	virtual QByteArray GetTenantId() const override;
	virtual void SetTenantId(const QByteArray& tenantId) override;
	virtual TenantInvitationStatus GetStatus() const override;
	virtual void SetStatus(TenantInvitationStatus status) override;
	virtual QByteArray GetInvitedByUserId() const override;
	virtual void SetInvitedByUserId(const QByteArray& userId) override;
	virtual QString GetCreatedAt() const override;
	virtual void SetCreatedAt(const QString& createdAt) override;
	virtual QString GetUpdatedAt() const override;
	virtual void SetUpdatedAt(const QString& updatedAt) override;
	virtual QString GetExpiresAt() const override;
	virtual void SetExpiresAt(const QString& expiresAt) override;
	virtual QString GetAcceptedAt() const override;
	virtual void SetAcceptedAt(const QString& acceptedAt) override;
	virtual QString GetRejectedAt() const override;
	virtual void SetRejectedAt(const QString& rejectedAt) override;
	virtual QString GetRevokedAt() const override;
	virtual void SetRevokedAt(const QString& revokedAt) override;
	virtual QByteArray GetRevokedByUserId() const override;
	virtual void SetRevokedByUserId(const QByteArray& userId) override;
	virtual QString GetLastSentAt() const override;
	virtual void SetLastSentAt(const QString& lastSentAt) override;

	virtual bool Serialize(iser::IArchive& archive) override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QByteArray m_invitationId;
	QByteArray m_userId;
	QByteArray m_tenantId;
	TenantInvitationStatus m_status;
	QByteArray m_invitedByUserId;
	QString m_createdAt;
	QString m_updatedAt;
	QString m_expiresAt;
	QString m_acceptedAt;
	QString m_rejectedAt;
	QString m_revokedAt;
	QByteArray m_revokedByUserId;
	QString m_lastSentAt;
};


typedef imtbase::TIdentifiableWrap<CTenantInvitation> CIdentifiableTenantInvitation;


} // namespace imtauth

