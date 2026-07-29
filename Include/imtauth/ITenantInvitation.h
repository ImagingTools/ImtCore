// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>
#include <idoc/IDocumentMetaInfo.h>
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imtauth/ITenantMembership.h>


namespace imtauth
{


class ITenantInvitation: virtual public iser::IObject
{
public:
	enum TenantInvitationStatus
	{
		TIS_PENDING,
		TIS_ACCEPTED,
		TIS_REJECTED,
		TIS_REVOKED,
		TIS_EXPIRED
	};

	I_DECLARE_ENUM(TenantInvitationStatus, TIS_PENDING, TIS_ACCEPTED, TIS_REJECTED, TIS_REVOKED, TIS_EXPIRED);

	enum MetaInfoTypes
	{
		MIT_INVITATION_ID = idoc::IDocumentMetaInfo::MIT_USER + 350,
		MIT_INVITATION_USER_ID,
		MIT_INVITATION_TENANT_ID,
		MIT_INVITATION_STATUS
	};

	virtual QByteArray GetInvitationId() const = 0;
	virtual void SetInvitationId(const QByteArray& invitationId) = 0;
	virtual QByteArray GetUserId() const = 0;
	virtual void SetUserId(const QByteArray& userId) = 0;
	virtual QByteArray GetTenantId() const = 0;
	virtual void SetTenantId(const QByteArray& tenantId) = 0;
	virtual TenantInvitationStatus GetStatus() const = 0;
	virtual void SetStatus(TenantInvitationStatus status) = 0;
	virtual QByteArray GetInvitedByUserId() const = 0;
	virtual void SetInvitedByUserId(const QByteArray& userId) = 0;
	virtual QString GetCreatedAt() const = 0;
	virtual void SetCreatedAt(const QString& createdAt) = 0;
	virtual QString GetUpdatedAt() const = 0;
	virtual void SetUpdatedAt(const QString& updatedAt) = 0;
	virtual QString GetExpiresAt() const = 0;
	virtual void SetExpiresAt(const QString& expiresAt) = 0;
	virtual QString GetAcceptedAt() const = 0;
	virtual void SetAcceptedAt(const QString& acceptedAt) = 0;
	virtual QString GetRejectedAt() const = 0;
	virtual void SetRejectedAt(const QString& rejectedAt) = 0;
	virtual QString GetRevokedAt() const = 0;
	virtual void SetRevokedAt(const QString& revokedAt) = 0;
	virtual QByteArray GetRevokedByUserId() const = 0;
	virtual void SetRevokedByUserId(const QByteArray& userId) = 0;
	virtual QString GetLastSentAt() const = 0;
	virtual void SetLastSentAt(const QString& lastSentAt) = 0;
};


typedef istd::TUniqueInterfacePtr<ITenantInvitation> ITenantInvitationUniquePtr;
typedef istd::TSharedInterfacePtr<ITenantInvitation> ITenantInvitationSharedPtr;


} // namespace imtauth

