// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtauth/ITenantInfo.h>


namespace imtauth
{


class ITenantManager: virtual public istd::IPolymorphic
{
public:
	virtual QByteArrayList GetTenantIds() const = 0;
	virtual ITenantInfoUniquePtr GetTenant(const QByteArray& tenantId) const = 0;
	virtual QByteArray CreateTenant(const QString& tenantName, const QString& description = QString()) = 0;
	virtual bool RemoveTenant(const QByteArray& tenantId) = 0;
	virtual bool UpdateTenant(const QByteArray& tenantId, const QString& tenantName, const QString& description) = 0;
	virtual bool SetTenantActive(const QByteArray& tenantId, bool isActive) = 0;
};


} // namespace imtauth


