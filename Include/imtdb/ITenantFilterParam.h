// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/ISerializable.h>

// Qt includes
#include <QtCore/QByteArray>


namespace imtdb
{


class ITenantFilterParam: virtual public iser::ISerializable
{
public:
    virtual QByteArray GetTenantId() const = 0;
    virtual void SetTenantId(const QByteArray& tenantId) = 0;

    virtual QByteArray GetOwnerId() const = 0;
    virtual void SetOwnerId(const QByteArray& ownerId) = 0;
};


} // namespace imtdb


