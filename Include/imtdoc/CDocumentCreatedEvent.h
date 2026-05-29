// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/CEventBase.h>


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief Event fired when a new document has been created.

	Produced by \c CDocumentServiceBase when a TT_NEW task has enqueued a new
	document entry (before the asynchronous object-creation thread completes).
	At this point the document is still in the loading state;
	\c CDocumentDataLoadedEvent follows once the data object is available.
*/
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


