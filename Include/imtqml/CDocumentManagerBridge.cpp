// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CDocumentManagerBridge.h>


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


IDocumentDataBridge::OperationStatus MapStatus(imtdoc::IDocumentManager::OperationStatus status)
{
	switch (status){
		case imtdoc::IDocumentManager::OS_OK:
			return IDocumentDataBridge::OS_OK;
		case imtdoc::IDocumentManager::OS_INVALID_USER_ID:
			return IDocumentDataBridge::OS_INVALID_USER_ID;
		case imtdoc::IDocumentManager::OS_INVALID_DOCUMENT_ID:
			return IDocumentDataBridge::OS_INVALID_DOCUMENT_ID;
		case imtdoc::IDocumentManager::OS_INVALID_DOCUMENT_DATA:
			return IDocumentDataBridge::OS_INVALID_DOCUMENT_DATA;
		case imtdoc::IDocumentManager::OS_FAILED:
		default:
			return IDocumentDataBridge::OS_FAILED;
	}
}


QString StatusToErrorMessage(imtdoc::IDocumentManager::OperationStatus status)
{
	switch (status){
		case imtdoc::IDocumentManager::OS_OK:
			return QString();
		case imtdoc::IDocumentManager::OS_INVALID_USER_ID:
			return QStringLiteral("Invalid user id");
		case imtdoc::IDocumentManager::OS_INVALID_DOCUMENT_ID:
			return QStringLiteral("Invalid document id");
		case imtdoc::IDocumentManager::OS_INVALID_DOCUMENT_DATA:
			return QStringLiteral("Invalid document data");
		case imtdoc::IDocumentManager::OS_FAILED:
		default:
			return QStringLiteral("Operation failed");
	}
}


const QString c_noManagerError = QStringLiteral(
		"CDocumentManagerBridge: no IDocumentManager wired (DocumentManager attribute is not set)");


} // anonymous namespace


CDocumentManagerBridge* CDocumentManagerBridge::s_instancePtr = nullptr;


CDocumentManagerBridge::CDocumentManagerBridge() = default;


CDocumentManagerBridge::~CDocumentManagerBridge()
{
	if (s_instancePtr == this){
		s_instancePtr = nullptr;
	}
}


CDocumentManagerBridge* CDocumentManagerBridge::Instance()
{
	return s_instancePtr;
}


// reimplemented (icomp::CComponentBase)

void CDocumentManagerBridge::OnComponentCreated()
{
	BaseClass::OnComponentCreated();
	s_instancePtr = this;
}


// reimplemented (IDocumentDataBridge)

void CDocumentManagerBridge::GetOpenedDocumentList(
		const QString& /*collectionId*/,
		DocumentListCallback callback)
{
	if (!callback){
		return;
	}
	imtdoc::IDocumentManager* mgrPtr = m_documentManagerCompPtr.GetPtr();
	if (mgrPtr == nullptr){
		PostToMainThread([cb = std::move(callback)]() mutable {
				cb({}, c_noManagerError);
			});
		return;
	}
	imtdoc::IDocumentManager::DocumentList list = mgrPtr->GetOpenedDocumentList(QByteArray());
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


void CDocumentManagerBridge::OpenDocument(
		const QString& /*collectionId*/,
		const QString& typeId,
		const QString& documentId,
		DocumentCallback callback)
{
	if (!callback){
		return;
	}
	imtdoc::IDocumentManager* mgrPtr = m_documentManagerCompPtr.GetPtr();
	if (mgrPtr == nullptr){
		PostToMainThread([cb = std::move(callback)]() mutable {
				cb(OpenedDocumentInfo{}, c_noManagerError);
			});
		return;
	}
	const QUrl url(QStringLiteral("collection:///") + documentId);
	const QByteArray openedId = mgrPtr->OpenDocument(QByteArray(), url);
	if (openedId.isEmpty()){
		PostToMainThread([cb = std::move(callback)]() mutable {
				cb(OpenedDocumentInfo{}, QStringLiteral("Failed to open document"));
			});
		return;
	}
	OpenedDocumentInfo info;
	info.documentId = QString::fromUtf8(openedId);
	info.typeId = typeId;
	info.objectId = documentId;
	QString name;
	if (mgrPtr->GetDocumentName(QByteArray(), openedId, name) == imtdoc::IDocumentManager::OS_OK){
		info.name = name;
	}
	PostToMainThread([cb = std::move(callback), info = std::move(info)]() mutable {
			cb(info, QString());
		});
}


void CDocumentManagerBridge::CreateDocument(
		const QString& /*collectionId*/,
		const QString& typeId,
		DocumentCallback callback)
{
	if (!callback){
		return;
	}
	imtdoc::IDocumentManager* mgrPtr = m_documentManagerCompPtr.GetPtr();
	if (mgrPtr == nullptr){
		PostToMainThread([cb = std::move(callback)]() mutable {
				cb(OpenedDocumentInfo{}, c_noManagerError);
			});
		return;
	}
	const QByteArray newId = mgrPtr->CreateNewDocument(QByteArray(), typeId.toUtf8());
	if (newId.isEmpty()){
		PostToMainThread([cb = std::move(callback)]() mutable {
				cb(OpenedDocumentInfo{}, QStringLiteral("Failed to create document"));
			});
		return;
	}
	OpenedDocumentInfo info;
	info.documentId = QString::fromUtf8(newId);
	info.typeId = typeId;
	QString name;
	if (mgrPtr->GetDocumentName(QByteArray(), newId, name) == imtdoc::IDocumentManager::OS_OK){
		info.name = name;
	}
	PostToMainThread([cb = std::move(callback), info = std::move(info)]() mutable {
			cb(info, QString());
		});
}


void CDocumentManagerBridge::SaveDocument(
		const QString& /*collectionId*/,
		const QString& documentId,
		const QString& documentName,
		StatusCallback callback)
{
	if (!callback){
		return;
	}
	imtdoc::IDocumentManager* mgrPtr = m_documentManagerCompPtr.GetPtr();
	if (mgrPtr == nullptr){
		PostToMainThread([cb = std::move(callback)]() mutable {
				cb(OS_FAILED, c_noManagerError);
			});
		return;
	}
	QString errorMessage;
	const auto status = mgrPtr->SaveDocument(
			QByteArray(),
			documentId.toUtf8(),
			documentName,
			&errorMessage);
	const auto mapped = MapStatus(status);
	QString msg = errorMessage;
	if (msg.isEmpty() && status != imtdoc::IDocumentManager::OS_OK){
		msg = StatusToErrorMessage(status);
	}
	PostToMainThread([cb = std::move(callback), mapped, msg = std::move(msg)]() mutable {
			cb(mapped, msg);
		});
}


void CDocumentManagerBridge::CloseDocument(
		const QString& /*collectionId*/,
		const QString& documentId,
		VoidCallback callback)
{
	if (!callback){
		return;
	}
	imtdoc::IDocumentManager* mgrPtr = m_documentManagerCompPtr.GetPtr();
	if (mgrPtr == nullptr){
		PostToMainThread([cb = std::move(callback)]() mutable {
				cb(c_noManagerError);
			});
		return;
	}
	const auto status = mgrPtr->CloseDocument(QByteArray(), documentId.toUtf8());
	QString msg;
	if (status != imtdoc::IDocumentManager::OS_OK){
		msg = StatusToErrorMessage(status);
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
		imtdoc::IDocumentManager* mgrPtr,
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
	if (status != imtdoc::IDocumentManager::OS_OK || undoManagerPtr == nullptr){
		errorMessage = StatusToErrorMessage(status);
		if (errorMessage.isEmpty()){
			errorMessage = QStringLiteral("Undo manager is not available");
		}
		return nullptr;
	}
	return undoManagerPtr;
}


} // anonymous namespace


void CDocumentManagerBridge::DoUndo(
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


void CDocumentManagerBridge::DoRedo(
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


void CDocumentManagerBridge::ResetUndo(
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


void CDocumentManagerBridge::GetUndoInfo(
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
