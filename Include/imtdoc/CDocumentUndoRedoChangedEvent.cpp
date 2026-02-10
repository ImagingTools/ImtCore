// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/CDocumentUndoRedoChangedEvent.h>


namespace imtdoc
{


// public methods

CDocumentUndoRedoChangedEvent::CDocumentUndoRedoChangedEvent(
	const QByteArray& userId,
	const QByteArray& documentId,
	const QByteArray& documentTypeId,
	const QString& documentName,
	const QUrl& documentUrl,
	bool isDirty,
	const idoc::IUndoManager& undoManager)
	:m_undoManager(undoManager)
{
	m_userId = userId;
	m_documentId = documentId;
	m_documentTypeId = documentTypeId;
	m_documentName = documentName;
	m_documentUrl = documentUrl;
	m_isDirty = isDirty;
}


const idoc::IUndoManager& CDocumentUndoRedoChangedEvent::GetUndoManager()
{
	return m_undoManager;
}


// reimplemented (CEventBase)

QByteArray CDocumentUndoRedoChangedEvent::Type() const
{
	return QByteArrayLiteral("DocumentUndoRedoChangedEvent");
}


} // namespace imtdoc


