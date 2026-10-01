// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CTenantSecurityContextScope.h>


namespace imtbase
{


// public methods

CTenantSecurityContextScope::CTenantSecurityContextScope(const CTenantSecurityContext& context)
	:m_previousContext(CTenantSecurityContext::GetCurrentContext())
{
	CTenantSecurityContext::SetCurrentContext(context);
}


CTenantSecurityContextScope::~CTenantSecurityContextScope()
{
	CTenantSecurityContext::SetCurrentContext(m_previousContext);
}


} // namespace imtbase


