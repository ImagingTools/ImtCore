// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcol/CTextFilter.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtcol
{


// public methods

// reimplemented (imtcol::ITextFilter)

QByteArrayList CTextFilter::GetFilteringFieldIds() const
{
	return m_fieldIds;
}


void CTextFilter::SetFilteringFieldIds(const QByteArrayList& fieldIds)
{
	if (m_fieldIds != fieldIds){
		istd::CChangeNotifier notifier(this, &istd::IChangeable::GetAnyChange());

		m_fieldIds = fieldIds;
	}
}


QString CTextFilter::GetFilterText() const
{
	return m_text;
}


void CTextFilter::SetFilterText(const QString& text)
{
	if (m_text != text){
		istd::CChangeNotifier notifier(this, &istd::IChangeable::GetAnyChange());

		m_text = text;
	}
}


// reimplemented (iser::ISerializable)

bool CTextFilter::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this, &istd::IChangeable::GetAllChanges());

	bool retVal = true;

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_fieldIds, "FieldIds", "FieldId");

	iser::CArchiveTag textTag("Text", "Text", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(textTag);
	retVal = retVal && archive.Process(m_text);
	retVal = retVal && archive.EndTag(textTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CTextFilter::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CTextFilter::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	auto sourcePtr = dynamic_cast<const CTextFilter*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier notifier(this, &istd::IChangeable::GetAllChanges());

		m_fieldIds = sourcePtr->m_fieldIds;
		m_text = sourcePtr->m_text;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CTextFilter::CloneMe(CompatibilityMode /*mode*/) const
{
	istd::IChangeableUniquePtr clonePtr(new CTextFilter());
	if (clonePtr->CopyFrom(*this)){
		return clonePtr;
	}

	return nullptr;
}


bool CTextFilter::ResetData(CompatibilityMode /*mode*/)
{
	m_fieldIds.clear();
	m_text.clear();

	return true;
}


} // namespace imtcol


