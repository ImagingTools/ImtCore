// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/CEventBase.h>


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief Event fired when an existing document has been opened.

	Produced by \c TCollectionDocumentServiceWrap::DoOpenDocument when the
	document entry is added to the per-user registry.  At this point the
	document is in the loading state; \c CDocumentDataLoadedEvent follows
	once the background loading thread has finished.

	In single-copy mode a second open of the same underlying object fires
	this event immediately with \c isLoading reflecting the shared entry's
	current loading state.
*/
class CDocumentOpenedEvent : public CEventBase
{
public:
	CDocumentOpenedEvent() = delete;
	CDocumentOpenedEvent(
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


