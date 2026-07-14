// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CContract.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


CContract::CContract()
{
	m_info.status = CTS_DRAFT;
}


ContractInfo CContract::GetContractInfo() const
{
	return m_info;
}


void CContract::SetContractInfo(const ContractInfo& contractInfo)
{
	if (m_info != contractInfo){
		istd::CChangeNotifier notifier(this);
		m_info = contractInfo;
	}
}


QByteArray CContract::GetContractId() const
{
	return m_info.contractId;
}


bool CContract::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag idTag("Id", "Id", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(idTag);
	retVal = retVal && archive.Process(m_info.contractId);
	retVal = retVal && archive.EndTag(idTag);

	iser::CArchiveTag relationshipTag("RelationshipId", "Relationship ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(relationshipTag);
	retVal = retVal && archive.Process(m_info.relationshipId);
	retVal = retVal && archive.EndTag(relationshipTag);

	iser::CArchiveTag sourceTag("SourceTenantId", "Source tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(sourceTag);
	retVal = retVal && archive.Process(m_info.sourceTenantId);
	retVal = retVal && archive.EndTag(sourceTag);

	iser::CArchiveTag targetTag("TargetTenantId", "Target tenant ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(targetTag);
	retVal = retVal && archive.Process(m_info.targetTenantId);
	retVal = retVal && archive.EndTag(targetTag);

	iser::CArchiveTag statusTag("Status", "Status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(statusTag);
	int status = static_cast<int>(m_info.status);
	retVal = retVal && archive.Process(status);
	m_info.status = static_cast<ContractStatus>(status);
	retVal = retVal && archive.EndTag(statusTag);

	iser::CArchiveTag scopeTag("Scope", "Scope", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(scopeTag);
	retVal = retVal && archive.Process(m_info.scope);
	retVal = retVal && archive.EndTag(scopeTag);

	iser::CArchiveTag validFromTag("ValidFrom", "Valid from", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(validFromTag);
	retVal = retVal && archive.Process(m_info.validFrom);
	retVal = retVal && archive.EndTag(validFromTag);

	iser::CArchiveTag validUntilTag("ValidUntil", "Valid until", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(validUntilTag);
	retVal = retVal && archive.Process(m_info.validUntil);
	retVal = retVal && archive.EndTag(validUntilTag);

	iser::CArchiveTag descriptionTag("Description", "Description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_info.description);
	retVal = retVal && archive.EndTag(descriptionTag);

	iser::CArchiveTag termsTag("Terms", "Terms", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(termsTag);
	retVal = retVal && archive.Process(m_info.terms);
	retVal = retVal && archive.EndTag(termsTag);

	iser::CArchiveTag createdAtTag("CreatedAt", "Created at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(createdAtTag);
	retVal = retVal && archive.Process(m_info.createdAt);
	retVal = retVal && archive.EndTag(createdAtTag);

	iser::CArchiveTag updatedAtTag("UpdatedAt", "Updated at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(updatedAtTag);
	retVal = retVal && archive.Process(m_info.updatedAt);
	retVal = retVal && archive.EndTag(updatedAtTag);

	return retVal;
}


bool CContract::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CContract* sourcePtr = dynamic_cast<const CContract*>(&object);
	if (sourcePtr == nullptr){
		return false;
	}

	istd::CChangeNotifier notifier(this);

	m_info = sourcePtr->m_info;

	return true;
}


istd::IChangeableUniquePtr CContract::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CContract);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CContract::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier notifier(this);

	m_info = ContractInfo();
	m_info.status = CTS_DRAFT;

	return true;
}


} // namespace imtauth
