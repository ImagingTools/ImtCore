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
	m_info.proposedSourceRole = ITenantInfo::Partner;
	m_info.proposedTargetRole = ITenantInfo::Partner;
	m_info.status = TCS_PENDING;
}


TenantConnectionRequestInfo CTenantConnectionRequest::GetRequestInfo() const
{
	return m_info;
}


void CTenantConnectionRequest::SetRequestInfo(const TenantConnectionRequestInfo& requestInfo)
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

	iser::CArchiveTag identifierTag("TargetIdentifier", "Target identifier", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(identifierTag);
	retVal = retVal && archive.Process(m_info.targetIdentifier);
	retVal = retVal && archive.EndTag(identifierTag);

	iser::CArchiveTag codeTag("ConnectCode", "Connect code", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(codeTag);
	retVal = retVal && archive.Process(m_info.connectCode);
	retVal = retVal && archive.EndTag(codeTag);

	iser::CArchiveTag sourceRoleTag("ProposedSourceRole", "Proposed source role", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(sourceRoleTag);
	int sourceRole = static_cast<int>(m_info.proposedSourceRole);
	retVal = retVal && archive.Process(sourceRole);
	m_info.proposedSourceRole = static_cast<ITenantInfo::TenantRelationshipRole>(sourceRole);
	retVal = retVal && archive.EndTag(sourceRoleTag);

	iser::CArchiveTag targetRoleTag("ProposedTargetRole", "Proposed target role", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(targetRoleTag);
	int targetRole = static_cast<int>(m_info.proposedTargetRole);
	retVal = retVal && archive.Process(targetRole);
	m_info.proposedTargetRole = static_cast<ITenantInfo::TenantRelationshipRole>(targetRole);
	retVal = retVal && archive.EndTag(targetRoleTag);

	iser::CArchiveTag messageTag("Message", "Message", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(messageTag);
	retVal = retVal && archive.Process(m_info.message);
	retVal = retVal && archive.EndTag(messageTag);

	iser::CArchiveTag statusTag("Status", "Status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(statusTag);
	int status = static_cast<int>(m_info.status);
	retVal = retVal && archive.Process(status);
	m_info.status = static_cast<TenantConnectionStatus>(status);
	retVal = retVal && archive.EndTag(statusTag);

	iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && archive.Process(m_info.createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	iser::CArchiveTag expiresAtTag("ExpiresAt", "Expires at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(expiresAtTag);
	retVal = retVal && archive.Process(m_info.expiresAt);
	retVal = retVal && archive.EndTag(expiresAtTag);

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

	m_info = TenantConnectionRequestInfo();
	m_info.proposedSourceRole = ITenantInfo::Partner;
	m_info.proposedTargetRole = ITenantInfo::Partner;
	m_info.status = TCS_PENDING;

	return true;
}


} // namespace imtauth
