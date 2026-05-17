// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtdoc/CEventBase.h>


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief Event fired when the in-memory content of an open document has changed.

	Produced by \c CDocumentServiceBase::OnUpdate when the document's model
	notifies the service of a data change.  In addition to the standard
	\c CEventBase fields the event carries a reference to the current document
	object and the \c ChangeSet that describes what changed.

	\note The referenced \a document and \a changeSet are only valid for the
	      duration of the \c ProcessEvent call.  Handlers must not store
	      references to them.
*/
class CDocumentChangedEvent : public CEventBase
{
public:
	CDocumentChangedEvent() = delete;
	CDocumentChangedEvent(
				const QByteArray& userId,
				const QByteArray& documentId,
				const QByteArray& documentTypeId,
				const QString& documentName,
				const QUrl& documentUrl,
				bool isDirty,
				const istd::IChangeable& document,
				const istd::IChangeable::ChangeSet& changeSet);
	const istd::IChangeable& GetDocument() const;
	istd::IChangeable::ChangeSet GetChangeSet() const;

	// reimplemented (CEventBase)
	virtual QByteArray Type() const override;

private:
	const istd::IChangeable& m_document;
	const istd::IChangeable::ChangeSet& m_changeSet;
};


} // namespace imtdoc


