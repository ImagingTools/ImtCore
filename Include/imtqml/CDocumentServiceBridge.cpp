// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CDocumentServiceBridge.h>


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QUrl>

// ACF includes
#include <idoc/IUndoManager.h>


namespace imtqml
{


namespace
{


/**
	\brief Schedule \a fn on the GUI thread via the global
	\c QCoreApplication, so that callbacks from this bridge always
	fire on the GUI thread regardless of where the caller invoked
	the method from.
*/
template<class Fn>
void PostToMainThread(Fn&& fn)
{
	QCoreApplication* appPtr = QCoreApplication::instance();
	if (appPtr == nullptr){
		// In headless / unit-test scenarios fall back to a direct call
		// so that the contract "callback fires exactly once" still
		// holds.
		fn();
		return;
	}
	QMetaObject::invokeMethod(appPtr, std::forward<Fn>(fn), Qt::QueuedConnection);
}


IDocumentServiceBridge::OperationStatus MapStatus(imtdoc::IDocumentService::OperationStatus status)
{
	switch (status){
		case imtdoc::IDocumentService::OS_OK:
			return IDocumentServiceBridge::OS_OK;
		case imtdoc::IDocumentService::OS_INVALID_USER_ID:
			return IDocumentServiceBridge::OS_INVALID_USER_ID;
		case imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID:
			return IDocumentServiceBridge::OS_INVALID_DOCUMENT_ID;
		case imtdoc::IDocumentService::OS_INVALID_DOCUMENT_DATA:
			return IDocumentServiceBridge::OS_INVALID_DOCUMENT_DATA;
		case imtdoc::IDocumentService::OS_FAILED:
		default:
			return IDocumentServiceBridge::OS_FAILED;
	}
}


QString StatusToErrorMessage(imtdoc::IDocumentService::OperationStatus status)
{
	switch (status){
		case imtdoc::IDocumentService::OS_OK:
			return QString();
		case imtdoc::IDocumentService::OS_INVALID_USER_ID:
			return QStringLiteral("Invalid user id");
		case imtdoc::IDocumentService::OS_INVALID_DOCUMENT_ID:
			return QStringLiteral("Invalid document id");
		case imtdoc::IDocumentService::OS_INVALID_DOCUMENT_DATA:
			return QStringLiteral("Invalid document data");
		case imtdoc::IDocumentService::OS_FAILED:
		default:
			return QStringLiteral("Operation failed");
	}
}


const QString c_noManagerError = QStringLiteral(
		"CDocumentServiceBridge: no IDocumentService wired (DocumentService attribute is not set)");


} // anonymous namespace


CDocumentServiceBridge* CDocumentServiceBridge::s_instancePtr = nullptr;


CDocumentServiceBridge::~CDocumentServiceBridge()
{
	if (s_instancePtr == this){
		s_instancePtr = nullptr;
	}
}


CDocumentServiceBridge* CDocumentServiceBridge::Instance()
{
	return s_instancePtr;
}


// reimplemented (icomp::CComponentBase)

void CDocumentServiceBridge::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
	s_instancePtr = this;
}


// reimplemented (IDocumentServiceBridge)

void CDocumentServiceBridge::GetOpenedDocumentList(
		const QString& /*collectionId*/,
		DocumentListCallback callback)
{
	if (!callback){
		return;
	}
	imtdoc::IDocumentService* mgrPtr = m_documentManagerCompPtr.GetPtr();
	if (mgrPtr == nullptr){
		PostToMainThread([cb = std::move(callback)]() mutable {
				cb({}, c_noManagerError);
			});
		return;
	}
	imtdoc::IDocumentService::DocumentList list = mgrPtr->GetOpenedDocumentList(QByteArray());
	QList<OpenedDocumentInfo> result;
	result.reserve(list.size());
	for (const auto& item: list){
		OpenedDocumentInfo info;
		info.documentId = QString::fromUtf8(item.documentId);
		info.typeId = QString::fromUtf8(item.typeId);
		info.name = item.name;
		info.objectId = item.url.path().mid(1); // strip leading '/'
		info.hasNameProvider = item.hasNameProvider;
		result.append(info);
	}
	PostToMainThread([cb = std::move(callback), result = std::move(result)]() mutable {
			cb(result, QString());
		});
}


void CDocumentServiceBridge::OpenDocument(
		const QString& /*collectionId*/,
		const QString& typeId,
		const QString& documentId,
		DocumentCallback callback)
{
	if (!callback){
		return;
	}
	imtdoc::IDocumentService* mgrPtr = m_documentManagerCompPtr.GetPtr();
	if (mgrPtr == nullptr){
		PostToMainThread([cb = std::move(callback)]() mutable {
				cb(OpenedDocumentInfo{}, c_noManagerError);
			});
		return;
	}
	const QUrl url(QStringLiteral("collection:///") + documentId);
	imtdoc::IDocumentService::TaskParams params;
	params.userId = QByteArray();
	params.url = url;
	const QByteArray taskId = mgrPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_OPEN, params);
	const imtdoc::IDocumentService::TaskResult result = mgrPtr->WaitForTaskFinished(taskId);
	if (result.status != imtdoc::IDocumentService::OS_OK || result.documentId.isEmpty()){
		QString msg = result.errorMessage;
		if (msg.isEmpty()){
			msg = QStringLiteral("Failed to open document");
		}
		PostToMainThread([cb = std::move(callback), msg = std::move(msg)]() mutable {
				cb(OpenedDocumentInfo{}, msg);
			});
		return;
	}
	OpenedDocumentInfo info;
	info.documentId = QString::fromUtf8(result.documentId);
	info.typeId = typeId;
	info.objectId = documentId;
	QString name;
	if (mgrPtr->GetDocumentName(QByteArray(), result.documentId, name) == imtdoc::IDocumentService::OS_OK){
		info.name = name;
	}
	PostToMainThread([cb = std::move(callback), info = std::move(info)]() mutable {
			cb(info, QString());
		});
}


void CDocumentServiceBridge::CreateDocument(
		const QString& /*collectionId*/,
		const QString& typeId,
		DocumentCallback callback)
{
	if (!callback){
		return;
	}
	imtdoc::IDocumentService* mgrPtr = m_documentManagerCompPtr.GetPtr();
	if (mgrPtr == nullptr){
		PostToMainThread([cb = std::move(callback)]() mutable {
				cb(OpenedDocumentInfo{}, c_noManagerError);
			});
		return;
	}
	imtdoc::IDocumentService::TaskParams params;
	params.userId = QByteArray();
	params.documentTypeId = typeId.toUtf8();
	const QByteArray taskId = mgrPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_NEW, params);
	const imtdoc::IDocumentService::TaskResult result = mgrPtr->WaitForTaskFinished(taskId);
	if (result.status != imtdoc::IDocumentService::OS_OK || result.documentId.isEmpty()){
		QString msg = result.errorMessage;
		if (msg.isEmpty()){
			msg = QStringLiteral("Failed to create document");
		}
		PostToMainThread([cb = std::move(callback), msg = std::move(msg)]() mutable {
				cb(OpenedDocumentInfo{}, msg);
			});
		return;
	}
	OpenedDocumentInfo info;
	info.documentId = QString::fromUtf8(result.documentId);
	info.typeId = typeId;
	QString name;
	if (mgrPtr->GetDocumentName(QByteArray(), result.documentId, name) == imtdoc::IDocumentService::OS_OK){
		info.name = name;
	}
	PostToMainThread([cb = std::move(callback), info = std::move(info)]() mutable {
			cb(info, QString());
		});
}


void CDocumentServiceBridge::SaveDocument(
		const QString& /*collectionId*/,
		const QString& documentId,
		const QString& documentName,
		StatusCallback callback)
{
	if (!callback){
		return;
	}
	imtdoc::IDocumentService* mgrPtr = m_documentManagerCompPtr.GetPtr();
	if (mgrPtr == nullptr){
		PostToMainThread([cb = std::move(callback)]() mutable {
				cb(OS_FAILED, c_noManagerError);
			});
		return;
	}
	imtdoc::IDocumentService::TaskParams params;
	params.userId = QByteArray();
	params.documentId = documentId.toUtf8();
	params.documentName = documentName;
	const QByteArray taskId = mgrPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_SAVE, params);
	const imtdoc::IDocumentService::TaskResult result = mgrPtr->WaitForTaskFinished(taskId);
	const auto mapped = MapStatus(result.status);
	QString msg = result.errorMessage;
	if (msg.isEmpty() && result.status != imtdoc::IDocumentService::OS_OK){
		msg = StatusToErrorMessage(result.status);
	}
	PostToMainThread([cb = std::move(callback), mapped, msg = std::move(msg)]() mutable {
			cb(mapped, msg);
		});
}


void CDocumentServiceBridge::CloseDocument(
		const QString& /*collectionId*/,
		const QString& documentId,
		VoidCallback callback)
{
	if (!callback){
		return;
	}
	imtdoc::IDocumentService* mgrPtr = m_documentManagerCompPtr.GetPtr();
	if (mgrPtr == nullptr){
		PostToMainThread([cb = std::move(callback)]() mutable {
				cb(c_noManagerError);
			});
		return;
	}
	imtdoc::IDocumentService::TaskParams params;
	params.userId = QByteArray();
	params.documentId = documentId.toUtf8();
	const QByteArray taskId = mgrPtr->BeginDocumentTask(imtdoc::IDocumentService::TT_CLOSE, params);
	const imtdoc::IDocumentService::TaskResult result = mgrPtr->WaitForTaskFinished(taskId);
	QString msg;
	if (result.status != imtdoc::IDocumentService::OS_OK){
		msg = result.errorMessage;
		if (msg.isEmpty()){
			msg = StatusToErrorMessage(result.status);
		}
	}
	PostToMainThread([cb = std::move(callback), msg = std::move(msg)]() mutable {
			cb(msg);
		});
}


namespace
{


/**
	\brief Resolve the IUndoManager attached to a given document.
	Returns \c nullptr and an error message in \a errorMessage on
	failure.
*/
idoc::IUndoManager* ResolveUndoManager(
		imtdoc::IDocumentService* mgrPtr,
		const QString& documentId,
		QString& errorMessage)
{
	if (mgrPtr == nullptr){
		errorMessage = c_noManagerError;
		return nullptr;
	}
	idoc::IUndoManager* undoManagerPtr = nullptr;
	const auto status = mgrPtr->GetDocumentUndoManager(
			QByteArray(), documentId.toUtf8(), undoManagerPtr);
	if (status != imtdoc::IDocumentService::OS_OK || undoManagerPtr == nullptr){
		errorMessage = StatusToErrorMessage(status);
		if (errorMessage.isEmpty()){
			errorMessage = QStringLiteral("Undo manager is not available");
		}
		return nullptr;
	}
	return undoManagerPtr;
}


} // anonymous namespace


void CDocumentServiceBridge::DoUndo(
		const QString& /*collectionId*/,
		const QString& documentId,
		int steps,
		VoidCallback callback)
{
	if (!callback){
		return;
	}
	QString errorMessage;
	idoc::IUndoManager* undoManagerPtr = ResolveUndoManager(
			m_documentManagerCompPtr.GetPtr(), documentId, errorMessage);
	if (undoManagerPtr == nullptr){
		PostToMainThread([cb = std::move(callback), errorMessage]() mutable {
				cb(errorMessage);
			});
		return;
	}
	const bool ok = undoManagerPtr->DoUndo(steps);
	QString msg;
	if (!ok){
		msg = QStringLiteral("Undo failed");
	}
	PostToMainThread([cb = std::move(callback), msg = std::move(msg)]() mutable {
			cb(msg);
		});
}


void CDocumentServiceBridge::DoRedo(
		const QString& /*collectionId*/,
		const QString& documentId,
		int steps,
		VoidCallback callback)
{
	if (!callback){
		return;
	}
	QString errorMessage;
	idoc::IUndoManager* undoManagerPtr = ResolveUndoManager(
			m_documentManagerCompPtr.GetPtr(), documentId, errorMessage);
	if (undoManagerPtr == nullptr){
		PostToMainThread([cb = std::move(callback), errorMessage]() mutable {
				cb(errorMessage);
			});
		return;
	}
	const bool ok = undoManagerPtr->DoRedo(steps);
	QString msg;
	if (!ok){
		msg = QStringLiteral("Redo failed");
	}
	PostToMainThread([cb = std::move(callback), msg = std::move(msg)]() mutable {
			cb(msg);
		});
}


void CDocumentServiceBridge::ResetUndo(
		const QString& /*collectionId*/,
		const QString& documentId,
		VoidCallback callback)
{
	if (!callback){
		return;
	}
	QString errorMessage;
	idoc::IUndoManager* undoManagerPtr = ResolveUndoManager(
			m_documentManagerCompPtr.GetPtr(), documentId, errorMessage);
	if (undoManagerPtr == nullptr){
		PostToMainThread([cb = std::move(callback), errorMessage]() mutable {
				cb(errorMessage);
			});
		return;
	}
	undoManagerPtr->ResetUndo();
	PostToMainThread([cb = std::move(callback)]() mutable {
			cb(QString());
		});
}


void CDocumentServiceBridge::GetUndoInfo(
		const QString& /*collectionId*/,
		const QString& documentId,
		UndoInfoCallback callback)
{
	if (!callback){
		return;
	}
	QString errorMessage;
	idoc::IUndoManager* undoManagerPtr = ResolveUndoManager(
			m_documentManagerCompPtr.GetPtr(), documentId, errorMessage);
	if (undoManagerPtr == nullptr){
		PostToMainThread([cb = std::move(callback), errorMessage]() mutable {
				cb(0, 0, false, errorMessage);
			});
		return;
	}
	const int undoSteps = undoManagerPtr->GetAvailableUndoSteps();
	const int redoSteps = undoManagerPtr->GetAvailableRedoSteps();
	const bool isDirty = undoSteps > 0;
	PostToMainThread([cb = std::move(callback), undoSteps, redoSteps, isDirty]() mutable {
			cb(undoSteps, redoSteps, isDirty, QString());
		});
}


} // namespace imtqml
