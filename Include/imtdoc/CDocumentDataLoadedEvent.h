// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdoc/CEventBase.h>


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief Event fired when a document's data object has finished loading.

	Produced by \c CDocumentServiceBase::OnDocumentDataLoaded once the
	background loading thread has stored the document's data object and
	initialised the model observers.  The \c isLoading flag transitions from
	\c true to \c false at this point.

	Handlers can use this event to perform work that requires the document
	data to be present (e.g.\ populate UI components).
*/
class CDocumentDataLoadedEvent : public CEventBase
{
public:
	CDocumentDataLoadedEvent() = delete;
	CDocumentDataLoadedEvent(
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


