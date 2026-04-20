// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// std includes
#include <functional>

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
class IDocumentDataBridge
{
public:
	struct FOpenedDocumentInfo
	{
		QString documentId;
		QString typeId;
		QString name;
		QString objectId;
		bool hasNameProvider = false;
	};

	enum EOperationStatus
	{
		OS_OK = 0,
		OS_INVALID_USER_ID,
		OS_INVALID_DOCUMENT_ID,
		OS_INVALID_DOCUMENT_DATA,
		OS_FAILED
	};

	typedef std::function<void(QList<FOpenedDocumentInfo>, QString /*errorMessage*/)> TDocumentListCallback;
	typedef std::function<void(FOpenedDocumentInfo, QString /*errorMessage*/)> TDocumentCallback;
	typedef std::function<void(QString /*errorMessage*/)> TVoidCallback;
	typedef std::function<void(EOperationStatus, QString /*errorMessage*/)> TStatusCallback;
	typedef std::function<void(int /*availableUndoSteps*/, int /*availableRedoSteps*/, bool /*isDirty*/, QString /*errorMessage*/)> TUndoInfoCallback;

	virtual ~IDocumentDataBridge() = default;

	virtual void GetOpenedDocumentList(
			const QString& collectionId,
			TDocumentListCallback callback) = 0;

	virtual void OpenDocument(
			const QString& collectionId,
			const QString& typeId,
			const QString& documentId,
			TDocumentCallback callback) = 0;

	virtual void CreateDocument(
			const QString& collectionId,
			const QString& typeId,
			TDocumentCallback callback) = 0;

	virtual void SaveDocument(
			const QString& collectionId,
			const QString& documentId,
			const QString& documentName,
			TStatusCallback callback) = 0;

	virtual void CloseDocument(
			const QString& collectionId,
			const QString& documentId,
			TVoidCallback callback) = 0;

	virtual void DoUndo(
			const QString& collectionId,
			const QString& documentId,
			int steps,
			TVoidCallback callback) = 0;

	virtual void DoRedo(
			const QString& collectionId,
			const QString& documentId,
			int steps,
			TVoidCallback callback) = 0;

	virtual void ResetUndo(
			const QString& collectionId,
			const QString& documentId,
			TVoidCallback callback) = 0;

	virtual void GetUndoInfo(
			const QString& collectionId,
			const QString& documentId,
			TUndoInfoCallback callback) = 0;
};


} // namespace imtqml
