// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/CEventBase.h>


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief Event fired when a document has been saved to a \e new collection object.

	Produced by \c TCollectionDocumentServiceWrap::DoSaveDocument when the
	save operation results in a new collection element being created (i.e.\
	the caller supplied a \c documentName that differs from the current name,
	causing a copy-on-save).  The \c documentId refers to the same in-memory
	instance; only the backing collection object changes.
*/
class CDocumentSavedAsEvent : public CEventBase
{
public:
	CDocumentSavedAsEvent() = delete;
	CDocumentSavedAsEvent(
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


