// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtauth/IPartyRoleInfo.h>


namespace imtauth
{


/**
	Catalog of the roles of the parties: the system roles and the roles of the organizations.
	\ingroup Tenant
*/
class IPartyRoleCatalog: virtual public istd::IChangeable
{
public:
	/**
		The system roles and the roles of the organization.
		\param isReadPtr [optional] False when the roles of the organization could not be read.
	*/
	virtual PartyRoleInfoList GetRoles(const QByteArray& tenantId, bool includeArchived, bool* isReadPtr = nullptr) const = 0;

	/**
		\return Null when there is no such role or it could not be read.
	*/
	virtual IPartyRoleInfoUniquePtr GetRole(const QByteArray& roleId) const = 0;

	/**
		Create a role of the organization that owns \c role.
		\return The id of the role, empty when refused.
	*/
	virtual QByteArray CreateRole(const IPartyRoleInfo& role, QString& errorMessage) = 0;

	/**
		Change a role of an organization; the owner and the id stay.
	*/
	virtual bool UpdateRole(const IPartyRoleInfo& role, QString& errorMessage) = 0;

	/**
		An archived role is not offered for new contracts; the contracts using it keep it.
	*/
	virtual bool SetRoleArchived(const QByteArray& tenantId, const QByteArray& roleId, bool isArchived, QString& errorMessage) = 0;
};


} // namespace imtauth


