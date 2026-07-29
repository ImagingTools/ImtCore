// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CCrossTenantMessage.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


CCrossTenantMessage::CCrossTenantMessage()
{
	m_info.messageType = CTMT_CUSTOM;
	m_info.status = CTMS_CREATED;
}


CrossTenantMessageInfo CCrossTenantMessage::GetMessageInfo() const
{
	return m_info;
}


void CCrossTenantMessage::SetMessageInfo(const CrossTenantMessageInfo& messageInfo)
{
	if (m_info != messageInfo){
		istd::CChangeNotifier notifier(this);
		m_info = messageInfo;
	}
}


QByteArray CCrossTenantMessage::GetMessageId() const
{
	return m_info.messageId;
}


bool CCrossTenantMessage::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_info.messageId);
	retVal = retVal && archive.EndTag(idTag);

	iser::CArchiveTag sourceTag("SourceTenantId", "Source tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(sourceTag);
	retVal = retVal && archive.Process(m_info.sourceTenantId);
	retVal = retVal && archive.EndTag(sourceTag);

	iser::CArchiveTag targetTag("TargetTenantId", "Target tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(targetTag);
	retVal = retVal && archive.Process(m_info.targetTenantId);
	retVal = retVal && archive.EndTag(targetTag);

	iser::CArchiveTag relationshipTag("RelationshipId", "Relationship ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(relationshipTag);
	retVal = retVal && archive.Process(m_info.relationshipId);
	retVal = retVal && archive.EndTag(relationshipTag);

	iser::CArchiveTag contractTag("ContractId", "Contract ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(contractTag);
	retVal = retVal && archive.Process(m_info.contractId);
	retVal = retVal && archive.EndTag(contractTag);

	iser::CArchiveTag sourceObjectTag("SourceObjectId", "Source object ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(sourceObjectTag);
	retVal = retVal && archive.Process(m_info.sourceObjectId);
	retVal = retVal && archive.EndTag(sourceObjectTag);

	iser::CArchiveTag targetObjectTag("TargetObjectId", "Target object ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(targetObjectTag);
	retVal = retVal && archive.Process(m_info.targetObjectId);
	retVal = retVal && archive.EndTag(targetObjectTag);

	iser::CArchiveTag messageTypeTag("MessageType", "Message type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(messageTypeTag);
	int messageType = static_cast<int>(m_info.messageType);
	retVal = retVal && archive.Process(messageType);
	m_info.messageType = static_cast<CrossTenantMessageType>(messageType);
	retVal = retVal && archive.EndTag(messageTypeTag);

	iser::CArchiveTag customTypeTag("CustomType", "Custom type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(customTypeTag);
	retVal = retVal && archive.Process(m_info.customType);
	retVal = retVal && archive.EndTag(customTypeTag);

	iser::CArchiveTag payloadTag("Payload", "Payload", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(payloadTag);
	retVal = retVal && archive.Process(m_info.payload);
	retVal = retVal && archive.EndTag(payloadTag);

	iser::CArchiveTag statusTag("Status", "Status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(statusTag);
	int status = static_cast<int>(m_info.status);
	retVal = retVal && archive.Process(status);
	m_info.status = static_cast<CrossTenantMessageStatus>(status);
	retVal = retVal && archive.EndTag(statusTag);

	iser::CArchiveTag errorTag("ErrorMessage", "Error message", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(errorTag);
	retVal = retVal && archive.Process(m_info.errorMessage);
	retVal = retVal && archive.EndTag(errorTag);

	iser::CArchiveTag accessModeTag("AccessMode", "Access mode", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(accessModeTag);
	int accessMode = static_cast<int>(m_info.accessMode);
	retVal = retVal && archive.Process(accessMode);
	m_info.accessMode = static_cast<DocumentShareAccessMode>(accessMode);
	retVal = retVal && archive.EndTag(accessModeTag);

	iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && archive.Process(m_info.createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	iser::CArchiveTag updatedAtTag("UpdatedAt", "Updated at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(updatedAtTag);
	retVal = retVal && archive.Process(m_info.updatedAt);
	retVal = retVal && archive.EndTag(updatedAtTag);

	iser::CArchiveTag expiresAtTag("ExpiresAt", "Expires at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(expiresAtTag);
	retVal = retVal && archive.Process(m_info.expiresAt);
	retVal = retVal && archive.EndTag(expiresAtTag);

	return retVal;
}


bool CCrossTenantMessage::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CCrossTenantMessage* sourcePtr = dynamic_cast<const CCrossTenantMessage*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_info = sourcePtr->m_info;

	return true;
}


istd::IChangeableUniquePtr CCrossTenantMessage::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CCrossTenantMessage);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CCrossTenantMessage::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_info = CrossTenantMessageInfo();
	m_info.messageType = CTMT_CUSTOM;
	m_info.status = CTMS_CREATED;

	return true;
}


} // namespace imtauth
