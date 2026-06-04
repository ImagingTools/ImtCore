// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantConnectionRequest.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


CTenantConnectionRequest::CTenantConnectionRequest()
{
m_info.status = CRS_PENDING;
}


ConnectionRequestInfo CTenantConnectionRequest::GetRequestInfo() const
{
return m_info;
}


void CTenantConnectionRequest::SetRequestInfo(const ConnectionRequestInfo& requestInfo)
{
if (m_info != requestInfo){
istd::CChangeNotifier notifier(this);
m_info = requestInfo;
}
}


QByteArray CTenantConnectionRequest::GetRequestId() const
{
return m_info.requestId;
}


bool CTenantConnectionRequest::Serialize(iser::IArchive& archive)
{
istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

bool retVal = true;

iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
retVal = retVal && archive.BeginTag(idTag);
retVal = retVal && archive.Process(m_info.requestId);
retVal = retVal && archive.EndTag(idTag);

iser::CArchiveTag sourceTag("SourceTenantId", "Source tenant ID", iser::CArchiveTag::TT_LEAF);
retVal = retVal && archive.BeginTag(sourceTag);
retVal = retVal && archive.Process(m_info.sourceTenantId);
retVal = retVal && archive.EndTag(sourceTag);

iser::CArchiveTag targetTag("TargetTenantId", "Target tenant ID", iser::CArchiveTag::TT_LEAF);
retVal = retVal && archive.BeginTag(targetTag);
retVal = retVal && archive.Process(m_info.targetTenantId);
retVal = retVal && archive.EndTag(targetTag);

iser::CArchiveTag codeTag("ConnectionCode", "Connection code", iser::CArchiveTag::TT_LEAF);
retVal = retVal && archive.BeginTag(codeTag);
retVal = retVal && archive.Process(m_info.connectionCode);
retVal = retVal && archive.EndTag(codeTag);

iser::CArchiveTag messageTag("Message", "Message", iser::CArchiveTag::TT_LEAF);
retVal = retVal && archive.BeginTag(messageTag);
retVal = retVal && archive.Process(m_info.message);
retVal = retVal && archive.EndTag(messageTag);

iser::CArchiveTag statusTag("Status", "Status", iser::CArchiveTag::TT_LEAF);
retVal = retVal && archive.BeginTag(statusTag);
int status = static_cast<int>(m_info.status);
retVal = retVal && archive.Process(status);
m_info.status = static_cast<ConnectionRequestStatus>(status);
retVal = retVal && archive.EndTag(statusTag);

iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
retVal = retVal && archive.BeginTag(createdAtTag);
retVal = retVal && archive.Process(m_info.createdAt);
retVal = retVal && archive.EndTag(createdAtTag);

iser::CArchiveTag respondedAtTag("RespondedAt", "Responded at", iser::CArchiveTag::TT_LEAF);
retVal = retVal && archive.BeginTag(respondedAtTag);
retVal = retVal && archive.Process(m_info.respondedAt);
retVal = retVal && archive.EndTag(respondedAtTag);

return retVal;
}


bool CTenantConnectionRequest::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
const CTenantConnectionRequest* sourcePtr = dynamic_cast<const CTenantConnectionRequest*>(&object);
if (sourcePtr == nullptr){
return false;
}

istd::CChangeNotifier notifier(this);

m_info = sourcePtr->m_info;

return true;
}


istd::IChangeableUniquePtr CTenantConnectionRequest::CloneMe(CompatibilityMode mode) const
{
istd::IChangeableUniquePtr clonePtr(new CTenantConnectionRequest);
if (clonePtr->CopyFrom(*this, mode)){
return clonePtr;
}

return nullptr;
}


bool CTenantConnectionRequest::ResetData(CompatibilityMode /*mode*/)
{
istd::CChangeNotifier notifier(this);

m_info = ConnectionRequestInfo();

return true;
}


} // namespace imtauth
