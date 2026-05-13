// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcol/CDocumentCollectionFilter.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>


namespace imtcol
{


// public methods

// reimplemented (imtcol::IDocumentCollectionFilter)

QByteArray CDocumentCollectionFilter::GetDocumentId() const
{
	return m_documentId;
}


void CDocumentCollectionFilter::SetDocumentId(const QByteArray& documentId)
{
	if (m_documentId != documentId){
		istd::CChangeNotifier changeNotifier(this);

		m_documentId = documentId;
	}
}


IDocumentCollectionFilter::DocumentStates CDocumentCollectionFilter::GetDocumentStates() const
{
	return m_states;
}


bool CDocumentCollectionFilter::AddDocumentState(DocumentState state)
{
	if (m_states.contains(state)){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	m_states << state;

	return true;
}


bool CDocumentCollectionFilter::RemoveDocumentState(DocumentState state)
{
	if (!m_states.contains(state)){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	m_states.removeAll(state);

	return true;
}


// reimplemented (iser::ISerializable)

bool CDocumentCollectionFilter::Serialize(iser::IArchive& /*archive*/)
{
	return false;
}


// reimplemented (istd::IChangeable)

int CDocumentCollectionFilter::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CDocumentCollectionFilter::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	auto sourcePtr = dynamic_cast<const CDocumentCollectionFilter*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_documentId = sourcePtr->m_documentId;
		m_states = sourcePtr->m_states;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CDocumentCollectionFilter::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CDocumentCollectionFilter());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CDocumentCollectionFilter::ResetData(CompatibilityMode /*mode*/)
{
	m_documentId.clear();
	m_states.clear();

	return true;
}


} // namespace imtcol


