#pragma once


// ImtCore includes
#include <imtdoc/CEventBase.h>


namespace imtdoc
{


class CDocumentCreatedEvent : public CEventBase
{
public:
	CDocumentCreatedEvent() = delete;
	CDocumentCreatedEvent(
		const QByteArray& userId,
		const QByteArray& documentId,
		const QByteArray& documentTypeId,
		const QString& documentName,
		const QUrl& documentUrl,
		bool isDirty);

	// reimplemented (CEventBase)
	virtual QByteArray Type() const override;
};


} // namespace imtdoc


