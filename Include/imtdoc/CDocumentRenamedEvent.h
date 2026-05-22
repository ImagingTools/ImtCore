// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/CEventBase.h>


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief Event fired when the display name of a document has been changed.

	Produced by \c TCollectionDocumentServiceWrap::SetDocumentName.  In
	single-copy mode the event is fired once per user session that shares the
	same underlying object, so multiple handlers may observe the same name
	change in sequence.
*/
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


