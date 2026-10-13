// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>

// ImtCore includes
#include <imtauth/ITenantManager.h>
#include <imtauth/ITenantMembershipManager.h>
#include <imtgql/CGqlRequest.h>


namespace imtauthgql
{


/**
	Access to the data of the organization: system administrators, and otherwise
	the organization of the request context, or (without a context) its owner and members.
*/
bool HasTenantIsolationAccess(
			const imtauth::ITenantManager& tenantManager,
			const imtauth::ITenantMembershipManager* membershipManagerPtr,
			const imtgql::CGqlRequest& gqlRequest,
			const QByteArray& tenantId,
			QString& errorMessage);

/**
	Management of the organization: system administrators, its owner and creator,
	and members holding \c requiredOrgPermission; never from the context of another organization.
*/
bool HasTenantManageAccess(
			const imtauth::ITenantManager& tenantManager,
			const imtauth::ITenantMembershipManager* membershipManagerPtr,
			const imtgql::CGqlRequest& gqlRequest,
			const QByteArray& tenantId,
			const QByteArray& requiredOrgPermission,
			QString& errorMessage);


} // namespace imtauthgql


