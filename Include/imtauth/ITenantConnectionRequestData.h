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
Serializable data holder for a single connection request.
Used as the stored representation inside an object collection
(e.g. an SQL-backed repository).
*/
class ITenantConnectionRequestData: virtual public iser::IObject
{
public:
virtual ConnectionRequestInfo GetRequestInfo() const = 0;
virtual void SetRequestInfo(const ConnectionRequestInfo& requestInfo) = 0;
virtual QByteArray GetRequestId() const = 0;
};


typedef istd::TUniqueInterfacePtr<ITenantConnectionRequestData> ITenantConnectionRequestDataUniquePtr;
typedef istd::TSharedInterfacePtr<ITenantConnectionRequestData> ITenantConnectionRequestDataSharedPtr;


} // namespace imtauth
