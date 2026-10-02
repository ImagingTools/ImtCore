// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IAccessContext.h>


namespace imtbase
{


/**
	Controller of the tenant access context of the calling thread.
	Used by the request entry points to bind the data operations of a request to its tenant and user,
	and by components that continue the work of a request in another thread.
	System access cannot be set by this interface, it is configured by the component wiring only.
*/
class IAccessContextController: virtual public IAccessContext
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


} // namespace imtbase


