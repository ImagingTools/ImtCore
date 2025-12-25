#pragma once


// ImtCore includes
#include <imtdoc/CEventBase.h>


namespace imtdoc
{


class CDocumentSavedEvent : public CEventBase
{
public:
	CDocumentSavedEvent() = delete;
	CDocumentSavedEvent(
		QByteArray userId,
		QByteArray documentId,
		QByteArray documentTypeId,
		QString documentName,
		QUrl documentUrl,
		bool isDirty);

	// reimplemented (CEventBase)
	virtual QByteArray Type() const override;
};


} // namespace imtdoc


