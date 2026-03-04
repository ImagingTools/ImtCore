// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcol/CDocumentIdFilter.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtcol
{


// public methods

CDocumentIdFilter::	CDocumentIdFilter()
	:m_conditionType(CT_IN)
{
}


// reimplemented (imtcol::IDocumentIdFilter)

QByteArrayList CDocumentIdFilter::GetDocumentIds() const
{
	return m_documentIds;
}


void CDocumentIdFilter::SetDocumentIds(const QByteArrayList& documentIds)
{
	if (m_documentIds != documentIds){
		istd::CChangeNotifier notifier(this);

		m_documentIds = documentIds;
	}
}


IDocumentIdFilter::ConditionType CDocumentIdFilter::GetConditionType() const
{
	return m_conditionType;
}


void CDocumentIdFilter::SetConditionType(ConditionType type)
{
	if (m_conditionType != type){
		istd::CChangeNotifier notifier(this);

		m_conditionType = type;
	}
}


// reimplemented (iser::ISerializable)

bool CDocumentIdFilter::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this, &istd::IChangeable::GetAllChanges());

	bool retVal = iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_documentIds, "DocumentIds", "DocumentId");

	iser::CArchiveTag conditionTypeTag("ConditionType", "Condition Type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(conditionTypeTag);
	retVal = retVal && I_SERIALIZE_ENUM(ConditionType, archive, m_conditionType);
	retVal = retVal && archive.EndTag(conditionTypeTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CDocumentIdFilter::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CDocumentIdFilter::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	auto sourcePtr = dynamic_cast<const CDocumentIdFilter*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier notifier(this, &istd::IChangeable::GetAllChanges());

		m_documentIds = sourcePtr->m_documentIds;
		m_conditionType = sourcePtr->m_conditionType;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CDocumentIdFilter::CloneMe(CompatibilityMode /*mode*/) const
{
	istd::IChangeableUniquePtr clonePtr(new CDocumentIdFilter());
	if (clonePtr->CopyFrom(*this)){
		return clonePtr;
	}

	return nullptr;
}


bool CDocumentIdFilter::ResetData(CompatibilityMode /*mode*/)
{
	m_documentIds.clear();
	m_conditionType = CT_IN;

	return true;
}


} // namespace imtcol


