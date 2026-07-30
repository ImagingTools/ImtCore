// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CDocumentShare.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


CDocumentShare::CDocumentShare()
{
	m_info.accessMode = DSAM_READ_ONLY;
	m_info.status = DSS_ACTIVE;
}


DocumentShareInfo CDocumentShare::GetShareInfo() const
{
	return m_info;
}


void CDocumentShare::SetShareInfo(const DocumentShareInfo& shareInfo)
{
	if (m_info != shareInfo){
		istd::CChangeNotifier notifier(this);
		m_info = shareInfo;
	}
}


QByteArray CDocumentShare::GetShareId() const
{
	return m_info.shareId;
}


bool CDocumentShare::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_info.shareId);
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

	iser::CArchiveTag documentTag("DocumentId", "Document ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(documentTag);
	retVal = retVal && archive.Process(m_info.documentId);
	retVal = retVal && archive.EndTag(documentTag);

	iser::CArchiveTag documentTypeTag("DocumentType", "Document type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(documentTypeTag);
	retVal = retVal && archive.Process(m_info.documentType);
	retVal = retVal && archive.EndTag(documentTypeTag);

	iser::CArchiveTag documentNameTag("DocumentName", "Document name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(documentNameTag);
	retVal = retVal && archive.Process(m_info.documentName);
	retVal = retVal && archive.EndTag(documentNameTag);

	iser::CArchiveTag accessModeTag("AccessMode", "Access mode", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(accessModeTag);
	int accessMode = static_cast<int>(m_info.accessMode);
	retVal = retVal && archive.Process(accessMode);
	m_info.accessMode = static_cast<DocumentShareAccessMode>(accessMode);
	retVal = retVal && archive.EndTag(accessModeTag);

	iser::CArchiveTag statusTag("Status", "Status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(statusTag);
	int status = static_cast<int>(m_info.status);
	retVal = retVal && archive.Process(status);
	m_info.status = static_cast<DocumentShareStatus>(status);
	retVal = retVal && archive.EndTag(statusTag);

	iser::CArchiveTag sharedByTag("SharedByUserId", "Shared by user ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(sharedByTag);
	retVal = retVal && archive.Process(m_info.sharedByUserId);
	retVal = retVal && archive.EndTag(sharedByTag);

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

	iser::CArchiveTag revokedAtTag("RevokedAt", "Revoked at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(revokedAtTag);
	retVal = retVal && archive.Process(m_info.revokedAt);
	retVal = retVal && archive.EndTag(revokedAtTag);

	return retVal;
}


bool CDocumentShare::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CDocumentShare* sourcePtr = dynamic_cast<const CDocumentShare*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_info = sourcePtr->m_info;

	return true;
}


istd::IChangeableUniquePtr CDocumentShare::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CDocumentShare);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CDocumentShare::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_info = DocumentShareInfo();

	return true;
}


} // namespace imtauth
