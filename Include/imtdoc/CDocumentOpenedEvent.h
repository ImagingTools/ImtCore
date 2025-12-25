#pragma once


// ImtCore includes
#include <imtdoc/CEventBase.h>


namespace imtdoc
{


class CDocumentOpenedEvent : public CEventBase
{
public:
	CDocumentOpenedEvent() = delete;
	CDocumentOpenedEvent(
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


