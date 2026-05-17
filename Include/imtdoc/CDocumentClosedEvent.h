// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/CEventBase.h>


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief Event fired when a document instance has been closed.

	Produced by \c CDocumentServiceBase when a TT_CLOSE task completes
	successfully, or when an open/create task fails after the document entry
	was already registered (cleanup path).

	After this event is dispatched the \a documentId is no longer valid for
	the affected user session.
*/
class CDocumentClosedEvent : public CEventBase
{
public:
	CDocumentClosedEvent() = delete;
	CDocumentClosedEvent(
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


