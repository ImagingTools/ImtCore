#include <imtdoc/CDocumentClosedEvent.h>


namespace imtdoc
{


// public methods

CDocumentClosedEvent::CDocumentClosedEvent(
	QByteArray userId,
	QByteArray documentId,
	QByteArray documentTypeId,
	QString documentName,
	QUrl documentUrl,
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

QByteArray CDocumentClosedEvent::Type() const
{
	return QByteArrayLiteral("DocumentClosedEvent");
}


} // namespace imtdoc


