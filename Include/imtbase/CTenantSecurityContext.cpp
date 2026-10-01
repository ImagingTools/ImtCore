// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CTenantSecurityContext.h>


namespace imtbase
{


namespace
{

thread_local CTenantSecurityContext s_currentContext;

} // namespace


// public methods

CTenantSecurityContext::CTenantSecurityContext()
	:m_isSystemContext(false)
{
}


CTenantSecurityContext::CTenantSecurityContext(const QByteArray& tenantId, const QByteArray& userId)
	:m_tenantId(tenantId),
	m_userId(userId),
	m_isSystemContext(false)
{
}


CTenantSecurityContext CTenantSecurityContext::CreateSystemContext()
{
	CTenantSecurityContext retVal;

	retVal.m_isSystemContext = true;

	return retVal;
}


CTenantSecurityContext CTenantSecurityContext::GetCurrentContext()
{
	return s_currentContext;
}


QByteArray CTenantSecurityContext::GetTenantId() const
{
	return m_tenantId;
}


QByteArray CTenantSecurityContext::GetUserId() const
{
	return m_userId;
}


bool CTenantSecurityContext::IsSystemContext() const
{
	return m_isSystemContext;
}


bool CTenantSecurityContext::IsDefined() const
{
	return m_isSystemContext || !m_tenantId.isEmpty() || !m_userId.isEmpty();
}


bool CTenantSecurityContext::operator==(const CTenantSecurityContext& other) const
{
	return (m_tenantId == other.m_tenantId) && (m_userId == other.m_userId) && (m_isSystemContext == other.m_isSystemContext);
}


bool CTenantSecurityContext::operator!=(const CTenantSecurityContext& other) const
{
	return !operator==(other);
}


// protected methods

void CTenantSecurityContext::SetCurrentContext(const CTenantSecurityContext& context)
{
	s_currentContext = context;
}


} // namespace imtbase


