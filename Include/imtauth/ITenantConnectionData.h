// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imtauth/ITenantConnectionRequest.h>


namespace imtauth
{


/**
Serializable data holder for a tenant connection.
Used as the stored representation inside an object collection
(e.g. an SQL-backed repository).
*/
class ITenantConnectionData: virtual public iser::IObject
{
public:
	virtual TenantConnectionInfo GetConnectionInfo() const = 0;
	virtual void SetConnectionInfo(const TenantConnectionInfo& connectionInfo) = 0;
	virtual QByteArray GetConnectionId() const = 0;
};


typedef istd::TUniqueInterfacePtr<ITenantConnectionData> ITenantConnectionDataUniquePtr;
typedef istd::TSharedInterfacePtr<ITenantConnectionData> ITenantConnectionDataSharedPtr;


} // namespace imtauth
