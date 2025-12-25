#pragma once


// ImtCore includes
#include <imtdoc/CEventBase.h>


namespace imtdoc
{


class CDocumentClosedEvent : public CEventBase
{
public:
	CDocumentClosedEvent() = delete;
	CDocumentClosedEvent(
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


