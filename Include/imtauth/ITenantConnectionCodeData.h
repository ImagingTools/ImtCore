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
Serializable data holder for a tenant connection code.
Used as the stored representation inside an object collection
(e.g. an SQL-backed repository).
*/
class ITenantConnectionCodeData: virtual public iser::IObject
{
public:
	virtual ITenantConnectionRequest::TenantConnectionCodeInfo GetCodeInfo() const = 0;
	virtual void SetCodeInfo(const ITenantConnectionRequest::TenantConnectionCodeInfo& codeInfo) = 0;
	virtual QByteArray GetTenantId() const = 0;
};


typedef istd::TUniqueInterfacePtr<ITenantConnectionCodeData> ITenantConnectionCodeDataUniquePtr;
typedef istd::TSharedInterfacePtr<ITenantConnectionCodeData> ITenantConnectionCodeDataSharedPtr;


} // namespace imtauth
