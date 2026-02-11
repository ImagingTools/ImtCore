// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtauth/IRole.h>


namespace imtauth
{


class IRoleManager: virtual public istd::IPolymorphic
{
public:
	virtual QByteArrayList GetRoleIds() const = 0;
	virtual IRoleUniquePtr GetRole(const QByteArray& roleId) const = 0;
	virtual QByteArray CreateRole(
				const QByteArray& productId,
				const QString& roleName,
				const QString& roleDescription = QString(),
				const QByteArrayList& permissions = QByteArrayList()) = 0;
	virtual bool RemoveRole(const QByteArray& roleId) = 0;
	virtual QByteArrayList GetRolePermissions(const QByteArray& roleId) const = 0;
	virtual bool AddPermissionsToRole(const QByteArray& roleId, const QByteArrayList& permissions) = 0;
	virtual bool RemovePermissionsFromRole(const QByteArray& roleId, const QByteArrayList& permissions) = 0;
};


} // namespace imtauth


