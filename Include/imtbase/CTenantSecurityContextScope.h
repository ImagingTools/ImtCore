// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/CTenantSecurityContext.h>


namespace imtbase
{


/**
	RAII helper setting the tenant security context of the current thread.
	The previous context is restored on destruction, so scopes can be nested.

	\code{.cpp}
	{
		imtbase::CTenantSecurityContextScope scope(imtbase::CTenantSecurityContext(tenantId, userId));
		// all database queries in this block are executed in the tenant context
	}

	{
		imtbase::CTenantSecurityContextScope scope(imtbase::CTenantSecurityContext::CreateSystemContext());
		// trusted administrative operation
	}
	\endcode
*/
class CTenantSecurityContextScope
{
public:
	explicit CTenantSecurityContextScope(const CTenantSecurityContext& context);
	~CTenantSecurityContextScope();

	CTenantSecurityContextScope(const CTenantSecurityContextScope&) = delete;
	CTenantSecurityContextScope& operator=(const CTenantSecurityContextScope&) = delete;

private:
	CTenantSecurityContext m_previousContext;
};


} // namespace imtbase


