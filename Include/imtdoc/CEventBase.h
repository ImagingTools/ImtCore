// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QUrl>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtdoc
{


/**
	\ingroup imtdoc
	\brief Base class for all document lifecycle event objects.

	Every event produced by the document service carries a common set of
	document-identifying fields (user-ID, document-ID, type-ID, name, URL,
	dirty flag) together with an accept/reject flag that handlers can use to
	signal whether the event was acted upon.

	Concrete subclasses override \c Type() to return a unique byte-string
	identifier and may add further payload fields (e.g.\ the changed document
	data in \c CDocumentChangedEvent or the undo-manager state in
	\c CDocumentUndoRedoChangedEvent).

	Events are created on the stack or heap by the service, passed to each
	\c IDocumentServiceEventHandler::ProcessEvent implementation, and must
	\e not be deleted by any handler.
*/
class CEventBase: virtual public istd::IPolymorphic
{
public:
	/**
		\brief Return the type tag that identifies the concrete event class.

		The base implementation returns an empty byte array; all concrete
		subclasses return a non-empty, unique string (e.g.\
		\c "DocumentOpened").
	*/
	virtual QByteArray Type() const;

	/** \return \c true if no handler has called \c Reject(). */
	bool IsAccepted() const;
	/** \brief Mark the event as accepted (default state). */
	void Accept();
	/** \brief Mark the event as rejected. */
	void Reject();

	/** \return The user-ID of the session that owns the affected document. */
	QByteArray GetUserId() const;
	/** \return The instance ID of the affected document. */
	QByteArray GetDocumentId() const;
	/** \return The registered type-ID of the document's data object. */
	QByteArray GetDocumentTypeId() const;
	/** \return The human-readable name of the document at the time of the event. */
	QString GetDocumentName() const;
	/** \return The source URL of the document (e.g.\ \c collection://\<objectId\>). */
	QUrl GetDocumentUrl() const;
	/** \return \c true when the document had unsaved changes at the time of the event. */
	bool IsDocumentDirty() const;

protected:
	bool m_isAccepted = true;  ///< Accept/reject state; initialised to \c true.

	QByteArray m_userId;          ///< Owning user session ID.
	QByteArray m_documentId;      ///< Document instance ID.
	QByteArray m_documentTypeId;  ///< Registered object type-ID.
	QString m_documentName;       ///< Human-readable document name.
	QUrl m_documentUrl;           ///< Source URL.
	bool m_isDirty = false;       ///< Unsaved-changes flag.
};


} // namespace imtdoc


