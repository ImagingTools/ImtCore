// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdb/IDatabaseAccessContext.h>


namespace imtdb
{


/**
	Controller of the tenant access context of the calling thread.
	Used by the request entry points to bind the database operations of a request to its tenant and user.
	System access cannot be set by this interface, it is configured by the component wiring only.
*/
class IDatabaseAccessContextController: virtual public IDatabaseAccessContext
{
public:
	/**
		Set the tenant access context of the calling thread.
	*/
	virtual void SetTenantAccessContext(const QByteArray& tenantId, const QByteArray& userId) = 0;

	/**
		Remove the access context of the calling thread.
	*/
	virtual void ResetAccessContext() = 0;
};


} // namespace imtdb


