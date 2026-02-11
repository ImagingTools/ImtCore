// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/CDocumentChangedEvent.h>


namespace imtdoc
{


// public methods

CDocumentChangedEvent::CDocumentChangedEvent(
			const QByteArray& userId,
			const QByteArray& documentId,
			const QByteArray& documentTypeId,
			const QString& documentName,
			const QUrl& documentUrl,
			bool isDirty,
			const istd::IChangeable& document,
			const istd::IChangeable::ChangeSet& changeSet)
	:m_document(document),
	m_changeSet(changeSet)
{
	m_userId = userId;
	m_documentId = documentId;
	m_documentTypeId = documentTypeId;
	m_documentName = documentName;
	m_documentUrl = documentUrl;
	m_isDirty = isDirty;
}


const istd::IChangeable& CDocumentChangedEvent::GetDocument() const
{
	return m_document;
}


istd::IChangeable::ChangeSet CDocumentChangedEvent::GetChangeSet() const
{
	return m_changeSet;
}


// reimplemented (CEventBase)

QByteArray CDocumentChangedEvent::Type() const
{
	return QByteArrayLiteral("DocumentChangedEvent");
}


} // namespace imtdoc


