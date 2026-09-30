// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CTenantContextScope.h>


namespace imtbase
{


namespace
{


thread_local QByteArray s_currentTenantId;
thread_local bool s_isActive = false;


} // namespace


CTenantContextScope::CTenantContextScope(const QByteArray& tenantId)
	:m_previousTenantId(s_currentTenantId),
	m_wasActive(s_isActive)
{
	s_currentTenantId = tenantId;
	s_isActive = true;
}


CTenantContextScope::~CTenantContextScope()
{
	s_currentTenantId = m_previousTenantId;
	s_isActive = m_wasActive;
}


// static methods

QByteArray CTenantContextScope::GetCurrentTenantId()
{
	return s_currentTenantId;
}


bool CTenantContextScope::IsActive()
{
	return s_isActive;
}


} // namespace imtbase
