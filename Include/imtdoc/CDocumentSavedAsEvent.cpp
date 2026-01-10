#include <imtdoc/CDocumentSavedAsEvent.h>


namespace imtdoc
{


// public methods

CDocumentSavedAsEvent::CDocumentSavedAsEvent(
	const QByteArray& userId,
	const QByteArray& documentId,
	const QByteArray& documentTypeId,
	const QString& documentName,
	const QUrl& documentUrl,
	bool isDirty)
{
	m_userId = userId;
	m_documentId = documentId;
	m_documentTypeId = documentTypeId;
	m_documentName = documentName;
	m_documentUrl = documentUrl;
	m_isDirty = isDirty;
}


// reimplemented (CEventBase)

QByteArray CDocumentSavedAsEvent::Type() const
{
	return QByteArrayLiteral("DocumentSavedAsEvent");
}


} // namespace imtdoc


