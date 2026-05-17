// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/CEventBase.h>


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief Event fired when a document has been saved to its existing collection object.

	Produced by \c TCollectionDocumentServiceWrap::DoSaveDocument when the
	document data is written back to the same collection element.  In
	single-copy mode the event is fired once per user session that holds the
	same underlying object.
*/
class CDocumentSavedEvent : public CEventBase
{
public:
	CDocumentSavedEvent() = delete;
	CDocumentSavedEvent(
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


