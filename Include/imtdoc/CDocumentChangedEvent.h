#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtdoc/CEventBase.h>


namespace imtdoc
{


class CDocumentChangedEvent : public CEventBase
{
public:
	CDocumentChangedEvent() = delete;
	CDocumentChangedEvent(
		QByteArray userId,
		QByteArray documentId,
		QByteArray documentTypeId,
		QString documentName,
		QUrl documentUrl,
		bool isDirty,
		const istd::IChangeable& document,
		const istd::IChangeable::ChangeSet& changeSet);
	const istd::IChangeable& GetDocument() const;
	const istd::IChangeable::ChangeSet GetChangeSet() const;

	// reimplemented (CEventBase)
	virtual QByteArray Type() const override;

private:
	const istd::IChangeable& m_document;
	const istd::IChangeable::ChangeSet& m_changeSet;
};


} // namespace imtdoc


