// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>
#include <istd/TSmartPtr.h>

// Qt includes
#include <QString>
#include <QByteArray>


namespace imtauth
{


/**
	Interface for a tenant-scoped role with custom permissions.
	\ingroup TenantRole
*/
class ITenantRole: virtual public iser::IObject
{
public:
	/**
		Get role ID.
	*/
	virtual QByteArray GetRoleId() const = 0;

	/**
		Get the tenant ID this role belongs to.
	*/
	virtual QByteArray GetTenantId() const = 0;

	/**
		Get role name.
	*/
	virtual QString GetRoleName() const = 0;

	/**
		Get permissions as semicolon-separated string of permission IDs.
	*/
	virtual QString GetPermissions() const = 0;
};


typedef istd::TUniqueInterfacePtr<ITenantRole> ITenantRoleUniquePtr;


} // namespace imtauth
