// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtauth/ITenantInvitation.h>


namespace imtauth
{


class ITenantInvitationManager: virtual public istd::IChangeable
{
public:
	typedef QByteArrayList InvitationIds;
	typedef QList<ITenantInvitation::TenantInvitationStatus> Statuses;

	virtual InvitationIds GetInvitationsByTenant(const QByteArray& tenantId, const Statuses& statuses = Statuses()) const = 0;
	virtual InvitationIds GetInvitationsByUser(const QByteArray& userId, const Statuses& statuses = Statuses()) const = 0;
	virtual ITenantInvitationUniquePtr GetInvitation(const QByteArray& invitationId) const = 0;
	virtual ITenantInvitationUniquePtr FindPendingInvitation(const QByteArray& userId, const QByteArray& tenantId) const = 0;
	virtual QByteArray CreateInvitation(const QByteArray& invitedByUserId, const QByteArray& userId, const QByteArray& tenantId, ITenantMembership::TenantMemberRole role) = 0;
	virtual QByteArray AcceptInvitation(const QByteArray& invitationId, const QByteArray& userId) = 0;
	virtual bool RejectInvitation(const QByteArray& invitationId, const QByteArray& userId) = 0;
	virtual bool RevokeInvitation(const QByteArray& invitationId, const QByteArray& revokedByUserId) = 0;
	virtual bool ResendInvitation(const QByteArray& invitationId) = 0;
	virtual ITenantInvitation::TenantInvitationStatus GetEffectiveStatus(const ITenantInvitation& invitation) const = 0;
};


} // namespace imtauth

