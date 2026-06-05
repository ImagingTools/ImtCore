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
Standalone data model for a confirmed connection between two tenants.

\ingroup Tenant
*/
class ITenantConnectionInfo: virtual public iser::IObject
{
public:
	/**
		Status of a confirmed connection between two tenants.
	*/
	enum ConnectionStatus
	{
		CS_ACTIVE = 0,
		CS_REMOVED,
		CS_SUSPENDED
	};

	I_DECLARE_ENUM(ConnectionStatus, CS_ACTIVE, CS_REMOVED, CS_SUSPENDED);

	virtual QByteArray GetConnectionId() const = 0;
	virtual void SetConnectionId(const QByteArray& connectionId) = 0;

	virtual QByteArray GetTenantAId() const = 0;
	virtual void SetTenantAId(const QByteArray& tenantAId) = 0;

	virtual QByteArray GetTenantBId() const = 0;
	virtual void SetTenantBId(const QByteArray& tenantBId) = 0;

	virtual ConnectionStatus GetStatus() const = 0;
	virtual void SetStatus(ConnectionStatus status) = 0;

	virtual QString GetCreatedAt() const = 0;
	virtual void SetCreatedAt(const QString& createdAt) = 0;

	virtual QString GetUpdatedAt() const = 0;
	virtual void SetUpdatedAt(const QString& updatedAt) = 0;
};


typedef istd::TUniqueInterfacePtr<ITenantConnectionInfo> ITenantConnectionInfoUniquePtr;
typedef istd::TSharedInterfacePtr<ITenantConnectionInfo> ITenantConnectionInfoSharedPtr;


} // namespace imtauth
