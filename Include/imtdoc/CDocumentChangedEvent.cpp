#include <imtdoc/CDocumentChangedEvent.h>


namespace imtdoc
{


// public methods

CDocumentChangedEvent::CDocumentChangedEvent(
	QByteArray userId,
	QByteArray documentId,
	QByteArray documentTypeId,
	QString documentName,
	QUrl documentUrl,
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


const istd::IChangeable::ChangeSet CDocumentChangedEvent::GetChangeSet() const
{
	return m_changeSet;
}


// reimplemented (CEventBase)

QByteArray CDocumentChangedEvent::Type() const
{
	return QByteArrayLiteral("DocumentChangedEvent");
}


} // namespace imtdoc


