// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// std includes
#include <functional>

// ACF includes
#include <istd/IPolymorphic.h>

// Qt includes
#include <QtCore/QList>
#include <QtCore/QString>


namespace imtqml
{


/**
	\brief Transport-agnostic abstract bridge for the document-management
	operations exposed by \c CDocumentDataController.

	\details
	The controller (a QML-facing \c QObject) does not know how its
	requests are actually serviced — that is delegated to an
	implementation of this interface, resolved through icomp at runtime.
	Two implementations are provided out of the box:

	- \c CGqlDocumentManagerBridge: serialises every call to an
	  SDL-generated GraphQL request and forwards it through
	  \c imtclientgql::IGqlClient (the GUI-client scenario).
	- \c CDocumentManagerBridge: forwards every call directly to an
	  \c imtdoc::IDocumentManager instance (the in-process / server
	  scenario).

	All operations are asynchronous — the implementation is expected to
	dispatch the work on a worker thread and invoke \c callback on the
	GUI thread (typically through \c QMetaObject::invokeMethod with
	\c Qt::QueuedConnection). Implementations MUST guarantee that the
	callback fires exactly once.

	An empty \c errorMessage in the callback signals success.
*/
class IDocumentDataBridge: virtual public istd::IPolymorphic
{
public:
	struct OpenedDocumentInfo
	{
		QString documentId;
		QString typeId;
		QString name;
		QString objectId;
		bool hasNameProvider = false;
	};

	enum OperationStatus
	{
		OS_OK = 0,
		OS_INVALID_USER_ID,
		OS_INVALID_DOCUMENT_ID,
		OS_INVALID_DOCUMENT_DATA,
		OS_FAILED
	};

	typedef std::function<void(QList<OpenedDocumentInfo>, QString /*errorMessage*/)> DocumentListCallback;
	typedef std::function<void(OpenedDocumentInfo, QString /*errorMessage*/)> DocumentCallback;
	typedef std::function<void(QString /*errorMessage*/)> VoidCallback;
	typedef std::function<void(OperationStatus, QString /*errorMessage*/)> StatusCallback;
	typedef std::function<void(int /*availableUndoSteps*/, int /*availableRedoSteps*/, bool /*isDirty*/, QString /*errorMessage*/)> UndoInfoCallback;

	virtual void GetOpenedDocumentList(
			const QString& collectionId,
			DocumentListCallback callback) = 0;

	virtual void OpenDocument(
			const QString& collectionId,
			const QString& typeId,
			const QString& documentId,
			DocumentCallback callback) = 0;

	virtual void CreateDocument(
			const QString& collectionId,
			const QString& typeId,
			DocumentCallback callback) = 0;

	virtual void SaveDocument(
			const QString& collectionId,
			const QString& documentId,
			const QString& documentName,
			StatusCallback callback) = 0;

	virtual void CloseDocument(
			const QString& collectionId,
			const QString& documentId,
			VoidCallback callback) = 0;

	virtual void DoUndo(
			const QString& collectionId,
			const QString& documentId,
			int steps,
			VoidCallback callback) = 0;

	virtual void DoRedo(
			const QString& collectionId,
			const QString& documentId,
			int steps,
			VoidCallback callback) = 0;

	virtual void ResetUndo(
			const QString& collectionId,
			const QString& documentId,
			VoidCallback callback) = 0;

	virtual void GetUndoInfo(
			const QString& collectionId,
			const QString& documentId,
			UndoInfoCallback callback) = 0;
};


} // namespace imtqml
