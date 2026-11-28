// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtassist/CAssistantRequestContext.h>


namespace imtassist
{


CAssistantRequestContext::CAssistantRequestContext(const QByteArray& userId, const QByteArray& tenantId)
	:m_userId(userId),
	m_tenantId(tenantId)
{
}


QByteArray CAssistantRequestContext::GetUserId() const
{
	return m_userId;
}


void CAssistantRequestContext::SetUserId(const QByteArray& userId)
{
	m_userId = userId;
}


QByteArray CAssistantRequestContext::GetTenantId() const
{
	return m_tenantId;
}


void CAssistantRequestContext::SetTenantId(const QByteArray& tenantId)
{
	m_tenantId = tenantId;
}


QByteArray CAssistantRequestContext::GetCorrelationId() const
{
	return m_correlationId;
}


void CAssistantRequestContext::SetCorrelationId(const QByteArray& correlationId)
{
	m_correlationId = correlationId;
}


CAssistantRequestContext::ContextItems CAssistantRequestContext::GetContextItems() const
{
	return m_contextItems;
}


void CAssistantRequestContext::SetContextItems(const ContextItems& items)
{
	m_contextItems = items;
}


bool CAssistantRequestContext::IsValid() const
{
	return !m_userId.isEmpty();
}


bool CAssistantRequestContext::IsSamePrincipal(const CAssistantRequestContext& other) const
{
	return IsValid() && (m_userId == other.m_userId) && (m_tenantId == other.m_tenantId);
}


const istd::IChangeable* CAssistantRequestContext::GetTransportContext() const
{
	return m_transportContextPtr.GetPtr();
}


void CAssistantRequestContext::SetTransportContext(const istd::IChangeableSharedPtr& transportContextPtr)
{
	m_transportContextPtr = transportContextPtr;
}


} // namespace imtassist


