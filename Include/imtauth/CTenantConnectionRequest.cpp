// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CTenantConnectionRequest.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


CTenantConnectionRequest::CTenantConnectionRequest():
	m_status(CRS_PENDING),
	m_isRelationshipProposed(false),
	m_proposedSourceRole(ITenantRelationshipInfo::TRR_PARTNER),
	m_proposedTargetRole(ITenantRelationshipInfo::TRR_PARTNER)
{
}


QByteArray CTenantConnectionRequest::GetRequestId() const
{
	return m_requestId;
}


void CTenantConnectionRequest::SetRequestId(const QByteArray& requestId)
{
	if (m_requestId != requestId){
		istd::CChangeNotifier notifier(this);

		m_requestId = requestId;
	}
}


QByteArray CTenantConnectionRequest::GetSourceTenantId() const
{
	return m_sourceTenantId;
}


void CTenantConnectionRequest::SetSourceTenantId(const QByteArray& sourceTenantId)
{
	if (m_sourceTenantId != sourceTenantId){
		istd::CChangeNotifier notifier(this);

		m_sourceTenantId = sourceTenantId;
	}
}


QByteArray CTenantConnectionRequest::GetTargetTenantId() const
{
	return m_targetTenantId;
}


void CTenantConnectionRequest::SetTargetTenantId(const QByteArray& targetTenantId)
{
	if (m_targetTenantId != targetTenantId){
		istd::CChangeNotifier notifier(this);

		m_targetTenantId = targetTenantId;
	}
}


QString CTenantConnectionRequest::GetConnectionCode() const
{
	return m_connectionCode;
}


void CTenantConnectionRequest::SetConnectionCode(const QString& connectionCode)
{
	if (m_connectionCode != connectionCode){
		istd::CChangeNotifier notifier(this);

		m_connectionCode = connectionCode;
	}
}


QString CTenantConnectionRequest::GetMessage() const
{
	return m_message;
}


void CTenantConnectionRequest::SetMessage(const QString& message)
{
	if (m_message != message){
		istd::CChangeNotifier notifier(this);

		m_message = message;
	}
}


ITenantConnectionRequestInfo::ConnectionRequestStatus CTenantConnectionRequest::GetStatus() const
{
	return m_status;
}


void CTenantConnectionRequest::SetStatus(ConnectionRequestStatus status)
{
	if (m_status != status){
		istd::CChangeNotifier notifier(this);

		m_status = status;
	}
}


QString CTenantConnectionRequest::GetCreatedAt() const
{
	return m_createdAt;
}


void CTenantConnectionRequest::SetCreatedAt(const QString& createdAt)
{
	if (m_createdAt != createdAt){
		istd::CChangeNotifier notifier(this);

		m_createdAt = createdAt;
	}
}


QString CTenantConnectionRequest::GetRespondedAt() const
{
	return m_respondedAt;
}


void CTenantConnectionRequest::SetRespondedAt(const QString& respondedAt)
{
	if (m_respondedAt != respondedAt){
		istd::CChangeNotifier notifier(this);

		m_respondedAt = respondedAt;
	}
}


QString CTenantConnectionRequest::GetSourceTenantName() const
{
	return m_sourceTenantName;
}


void CTenantConnectionRequest::SetSourceTenantName(const QString& sourceTenantName)
{
	if (m_sourceTenantName != sourceTenantName){
		istd::CChangeNotifier notifier(this);

		m_sourceTenantName = sourceTenantName;
	}
}


QString CTenantConnectionRequest::GetTargetTenantName() const
{
	return m_targetTenantName;
}


void CTenantConnectionRequest::SetTargetTenantName(const QString& targetTenantName)
{
	if (m_targetTenantName != targetTenantName){
		istd::CChangeNotifier notifier(this);

		m_targetTenantName = targetTenantName;
	}
}


bool CTenantConnectionRequest::IsRelationshipProposed() const
{
	return m_isRelationshipProposed;
}


void CTenantConnectionRequest::SetRelationshipProposed(bool isProposed)
{
	if (m_isRelationshipProposed != isProposed){
		istd::CChangeNotifier notifier(this);

		m_isRelationshipProposed = isProposed;
	}
}


ITenantRelationshipInfo::TenantRelationshipRole CTenantConnectionRequest::GetProposedSourceRole() const
{
	return m_proposedSourceRole;
}


void CTenantConnectionRequest::SetProposedSourceRole(ITenantRelationshipInfo::TenantRelationshipRole role)
{
	if (m_proposedSourceRole != role){
		istd::CChangeNotifier notifier(this);

		m_proposedSourceRole = role;
	}
}


ITenantRelationshipInfo::TenantRelationshipRole CTenantConnectionRequest::GetProposedTargetRole() const
{
	return m_proposedTargetRole;
}


void CTenantConnectionRequest::SetProposedTargetRole(ITenantRelationshipInfo::TenantRelationshipRole role)
{
	if (m_proposedTargetRole != role){
		istd::CChangeNotifier notifier(this);

		m_proposedTargetRole = role;
	}
}


QString CTenantConnectionRequest::GetProposedScope() const
{
	return m_proposedScope;
}


void CTenantConnectionRequest::SetProposedScope(const QString& scope)
{
	if (m_proposedScope != scope){
		istd::CChangeNotifier notifier(this);

		m_proposedScope = scope;
	}
}


bool CTenantConnectionRequest::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_requestId);
	retVal = retVal && archive.EndTag(idTag);

	iser::CArchiveTag sourceTag("SourceTenantId", "Source tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(sourceTag);
	retVal = retVal && archive.Process(m_sourceTenantId);
	retVal = retVal && archive.EndTag(sourceTag);

	iser::CArchiveTag targetTag("TargetTenantId", "Target tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(targetTag);
	retVal = retVal && archive.Process(m_targetTenantId);
	retVal = retVal && archive.EndTag(targetTag);

	iser::CArchiveTag codeTag("ConnectionCode", "Connection code", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(codeTag);
	retVal = retVal && archive.Process(m_connectionCode);
	retVal = retVal && archive.EndTag(codeTag);

	iser::CArchiveTag messageTag("Message", "Message", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(messageTag);
	retVal = retVal && archive.Process(m_message);
	retVal = retVal && archive.EndTag(messageTag);

	iser::CArchiveTag statusTag("Status", "Status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(statusTag);
	int status = static_cast<int>(m_status);
	retVal = retVal && archive.Process(status);
	m_status = static_cast<ConnectionRequestStatus>(status);
	retVal = retVal && archive.EndTag(statusTag);

	iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && archive.Process(m_createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	iser::CArchiveTag respondedAtTag("RespondedAt", "Responded at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(respondedAtTag);
	retVal = retVal && archive.Process(m_respondedAt);
	retVal = retVal && archive.EndTag(respondedAtTag);

	iser::CArchiveTag proposedTag("RelationshipProposed", "Relationship proposed", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(proposedTag);
	retVal = retVal && archive.Process(m_isRelationshipProposed);
	retVal = retVal && archive.EndTag(proposedTag);

	iser::CArchiveTag sourceRoleTag("ProposedSourceRole", "Proposed role of the source", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(sourceRoleTag);
	int sourceRole = static_cast<int>(m_proposedSourceRole);
	retVal = retVal && archive.Process(sourceRole);
	m_proposedSourceRole = static_cast<ITenantRelationshipInfo::TenantRelationshipRole>(sourceRole);
	retVal = retVal && archive.EndTag(sourceRoleTag);

	iser::CArchiveTag targetRoleTag("ProposedTargetRole", "Proposed role of the target", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(targetRoleTag);
	int targetRole = static_cast<int>(m_proposedTargetRole);
	retVal = retVal && archive.Process(targetRole);
	m_proposedTargetRole = static_cast<ITenantRelationshipInfo::TenantRelationshipRole>(targetRole);
	retVal = retVal && archive.EndTag(targetRoleTag);

	iser::CArchiveTag scopeTag("ProposedScope", "Proposed scope", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(scopeTag);
	retVal = retVal && archive.Process(m_proposedScope);
	retVal = retVal && archive.EndTag(scopeTag);

	return retVal;
}


bool CTenantConnectionRequest::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CTenantConnectionRequest* sourcePtr = dynamic_cast<const CTenantConnectionRequest*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_requestId = sourcePtr->m_requestId;
	m_sourceTenantId = sourcePtr->m_sourceTenantId;
	m_targetTenantId = sourcePtr->m_targetTenantId;
	m_connectionCode = sourcePtr->m_connectionCode;
	m_message = sourcePtr->m_message;
	m_status = sourcePtr->m_status;
	m_createdAt = sourcePtr->m_createdAt;
	m_respondedAt = sourcePtr->m_respondedAt;
	m_sourceTenantName = sourcePtr->m_sourceTenantName;
	m_targetTenantName = sourcePtr->m_targetTenantName;
	m_isRelationshipProposed = sourcePtr->m_isRelationshipProposed;
	m_proposedSourceRole = sourcePtr->m_proposedSourceRole;
	m_proposedTargetRole = sourcePtr->m_proposedTargetRole;
	m_proposedScope = sourcePtr->m_proposedScope;

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

	m_requestId.clear();
	m_sourceTenantId.clear();
	m_targetTenantId.clear();
	m_connectionCode.clear();
	m_message.clear();
	m_status = CRS_PENDING;
	m_createdAt.clear();
	m_respondedAt.clear();
	m_sourceTenantName.clear();
	m_targetTenantName.clear();
	m_isRelationshipProposed = false;
	m_proposedSourceRole = ITenantRelationshipInfo::TRR_PARTNER;
	m_proposedTargetRole = ITenantRelationshipInfo::TRR_PARTNER;
	m_proposedScope.clear();

	return true;
}


} // namespace imtauth
