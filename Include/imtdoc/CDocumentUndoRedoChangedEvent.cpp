#include <imtdoc/CDocumentUndoRedoChangedEvent.h>


namespace imtdoc
{


// public methods

CDocumentUndoRedoChangedEvent::CDocumentUndoRedoChangedEvent(
	QByteArray userId,
	QByteArray documentId,
	QByteArray documentTypeId,
	QString documentName,
	QUrl documentUrl,
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


