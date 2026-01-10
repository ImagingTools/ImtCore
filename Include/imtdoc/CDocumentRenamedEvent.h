#pragma once


// ImtCore includes
#include <imtdoc/CEventBase.h>


namespace imtdoc
{


class CDocumentRenamedEvent : public CEventBase
{
public:
	CDocumentRenamedEvent() = delete;
	CDocumentRenamedEvent(
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


