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
	Serializable data holder for a single tenant connection request.
	Used as the stored representation of a TenantConnectionRequestInfo inside an
	object collection (e.g. an SQL-backed repository).
*/
class ITenantConnectionRequestData: virtual public iser::IObject
{
public:
	/**
		Get the full connection request description.
	*/
	virtual TenantConnectionRequestInfo GetRequestInfo() const = 0;

	/**
		Set the full connection request description.
	*/
	virtual void SetRequestInfo(const TenantConnectionRequestInfo& requestInfo) = 0;

	/**
		Convenience accessor for the request ID, which is used as the
		collection element key.
	*/
	virtual QByteArray GetRequestId() const = 0;
};


typedef istd::TUniqueInterfacePtr<ITenantConnectionRequestData> ITenantConnectionRequestDataUniquePtr;
typedef istd::TSharedInterfacePtr<ITenantConnectionRequestData> ITenantConnectionRequestDataSharedPtr;


} // namespace imtauth
