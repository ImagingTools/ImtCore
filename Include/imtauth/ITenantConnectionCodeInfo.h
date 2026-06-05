// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>
#include <istd/TSmartPtr.h>

// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>


namespace imtauth
{


/**
Standalone data model for a tenant connection code.
Each tenant has a unique connection code used for discovery and
initiating connection requests.

\ingroup Tenant
*/
class ITenantConnectionCodeInfo: virtual public iser::IObject
{
public:
virtual QByteArray GetTenantId() const = 0;
virtual void SetTenantId(const QByteArray& tenantId) = 0;

virtual QString GetConnectionCode() const = 0;
virtual void SetConnectionCode(const QString& connectionCode) = 0;

virtual bool GetAllowConnectionsByCode() const = 0;
virtual void SetAllowConnectionsByCode(bool allow) = 0;

virtual QString GetCreatedAt() const = 0;
virtual void SetCreatedAt(const QString& createdAt) = 0;
};


typedef istd::TUniqueInterfacePtr<ITenantConnectionCodeInfo> ITenantConnectionCodeInfoUniquePtr;
typedef istd::TSharedInterfacePtr<ITenantConnectionCodeInfo> ITenantConnectionCodeInfoSharedPtr;


} // namespace imtauth
