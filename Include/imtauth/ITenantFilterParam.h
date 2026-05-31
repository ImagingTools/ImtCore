// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>

// Qt includes
#include <QtCore/QByteArray>


namespace imtauth
{


/**
	Tenant filter mode: Include returns items belonging to the tenant,
	Exclude returns items NOT belonging to the tenant.
*/
enum TenantFilterMode
{
	TFM_INCLUDE = 0,
	TFM_EXCLUDE = 1
};


class ITenantFilterParam: virtual public iser::ISerializable
{
public:
	virtual QByteArray GetTenantId() const = 0;
	virtual void SetTenantId(const QByteArray& tenantId) = 0;

	virtual QByteArray GetOwnerId() const = 0;
	virtual void SetOwnerId(const QByteArray& ownerId) = 0;

	virtual TenantFilterMode GetFilterMode() const = 0;
	virtual void SetFilterMode(TenantFilterMode mode) = 0;
};


} // namespace imtauth
