// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CGqlDocumentDataController.h>


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QPointer>
#include <QtConcurrent/QtConcurrentRun>

// ImtCore includes
#include <imtqml/CGqlClientBridge.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/CollectionDocumentManager.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/UndoManager.h>


namespace imtqml
{


namespace docmgr = sdl::imtbase::CollectionDocumentManager;


namespace
{


/**
	Schedule a callable on the GUI thread. The receiver is the global
	\c QCoreApplication instance (essentially app-lifetime), so the post
	itself never races with object destruction; the callable is expected
	to validate the \c QPointer it captured before touching the
	controller.
*/
template<class Fn>
void PostToMainThread(Fn&& fn)
{
	QCoreApplication* appPtr = QCoreApplication::instance();
	if (appPtr == nullptr){
		return;
	}
	QMetaObject::invokeMethod(appPtr, std::forward<Fn>(fn), Qt::QueuedConnection);
}


template<class Opt>
QString OptString(const Opt& value)
{
	return value.HasValue() ? QString(*value) : QString();
}


template<class Opt>
QString OptByteArrayAsString(const Opt& value)
{
	return value.HasValue() ? QString::fromUtf8(*value) : QString();
}


template<class Opt>
bool OptBool(const Opt& value, bool fallback = false)
{
	return value.HasValue() ? *value : fallback;
}


template<class Opt>
int OptInt(const Opt& value, int fallback = 0)
{
	return value.HasValue() ? *value : fallback;
}


} // namespace


// public methods

CGqlDocumentDataController::CGqlDocumentDataController(QObject* parent)
	:BaseClass(parent)
{
	// Mirror QML behaviour: when collectionId becomes non-empty, fetch
	// the opened-document list automatically.
	connect(this, &CGqlDocumentDataController::collectionIdChanged, this,
			[this](const QString& id){
				if (!id.isEmpty()){
					getOpenedDocumentList();
				}
			});

	// Mirror QML onDocumentSaved / onUndoInfoReceived / onStartCloseDocument /
	// onDocumentClosed / onDocumentCreated / onDocumentOpened wiring.
	connect(this, &CGqlDocumentDataController::documentSaved, this,
			[this](const QString& documentId){
				setDocumentIsNew(documentId, false);
			});
	connect(this, &CGqlDocumentDataController::undoInfoReceived, this,
			[this](const QString& documentId, int, int, bool isDirty){
				setDocumentIsDirty(documentId, isDirty);
			});
	connect(this, &CGqlDocumentDataController::startCloseDocument, this,
			[this](const QString& documentId){
				int idx = IndexOfDocument(documentId);
				if (idx >= 0){
					m_openedDocuments[idx].isClosing = true;
				}
			});
	connect(this, &CGqlDocumentDataController::documentClosed, this,
			[this](const QString& documentId){
				RemoveDocumentDataInternal(documentId);
			});
	connect(this, &CGqlDocumentDataController::documentCreated, this,
			[this](const QString& documentId, const QString& typeId){
				CreateDocumentDataInternal(documentId, typeId, true);
			});
	connect(this, &CGqlDocumentDataController::documentOpened, this,
			[this](const QString& documentId, const QString& typeId){
				CreateDocumentDataInternal(documentId, typeId, false);
			});
}


CGqlDocumentDataController::~CGqlDocumentDataController() = default;


const QString& CGqlDocumentDataController::GetCollectionId() const
{
	return m_collectionId;
}


void CGqlDocumentDataController::SetCollectionId(const QString& id)
{
	if (m_collectionId != id){
		m_collectionId = id;
		Q_EMIT collectionIdChanged(m_collectionId);
	}
}


// --- DocumentManagerBase: registration / introspection ---

void CGqlDocumentDataController::registerDocumentViewData(
	const QString& documentTypeId,
	const QString& viewTypeId,
	QObject* viewEditorComp,
	QObject* representationControllerComp)
{
	QList<FViewEntry>& views = m_documentTypeEditors[documentTypeId];
	for (const FViewEntry& entry : views){
		if (entry.viewTypeId == viewTypeId){
			qWarning() << "Unable to register view with type-ID"
					<< viewTypeId << "for document type:" << documentTypeId
					<< "Error: View already registered";
			return;
		}
	}

	FViewEntry entry;
	entry.viewTypeId = viewTypeId;
	entry.viewEditorComp = viewEditorComp;
	entry.representationControllerComp = representationControllerComp;
	views.append(entry);

	Q_EMIT documentTypeEditorsChanged();
	Q_EMIT documentViewRegistered(documentTypeId, viewTypeId);
}


QObject* CGqlDocumentDataController::getDocumentEditorFactory(
	const QString& documentTypeId,
	const QString& viewTypeId) const
{
	const auto it = m_documentTypeEditors.constFind(documentTypeId);
	if (it == m_documentTypeEditors.constEnd()){
		return nullptr;
	}
	for (const FViewEntry& entry : *it){
		if (viewTypeId.isEmpty() || entry.viewTypeId == viewTypeId){
			return entry.viewEditorComp.data();
		}
	}
	return nullptr;
}


QObject* CGqlDocumentDataController::getDocumentRepresentationControllerFactory(
	const QString& documentTypeId,
	const QString& viewTypeId) const
{
	const auto it = m_documentTypeEditors.constFind(documentTypeId);
	if (it == m_documentTypeEditors.constEnd()){
		return nullptr;
	}
	for (const FViewEntry& entry : *it){
		if (viewTypeId.isEmpty() || entry.viewTypeId == viewTypeId){
			return entry.representationControllerComp.data();
		}
	}
	return nullptr;
}


QStringList CGqlDocumentDataController::getSupportedDocumentTypeIds() const
{
	return m_documentTypeEditors.keys();
}


QStringList CGqlDocumentDataController::getSupportedDocumentViewTypeIds(
	const QString& documentTypeId) const
{
	QStringList result;
	const auto it = m_documentTypeEditors.constFind(documentTypeId);
	if (it == m_documentTypeEditors.constEnd()){
		return result;
	}
	result.reserve(it->size());
	for (const FViewEntry& entry : *it){
		result.append(entry.viewTypeId);
	}
	return result;
}


QString CGqlDocumentDataController::getViewTypeIdByViewFactory(
	const QString& documentTypeId,
	QObject* viewFactory) const
{
	const auto it = m_documentTypeEditors.constFind(documentTypeId);
	if (it == m_documentTypeEditors.constEnd()){
		return QString();
	}
	for (const FViewEntry& entry : *it){
		if (entry.viewEditorComp.data() == viewFactory){
			return entry.viewTypeId;
		}
	}
	return QString();
}


// --- DocumentManagerBase: opened-document state ---

QStringList CGqlDocumentDataController::getOpenedDocumentIds() const
{
	QStringList result;
	result.reserve(m_openedDocuments.size());
	for (const FOpenedDocument& doc : m_openedDocuments){
		result.append(doc.id);
	}
	return result;
}


bool CGqlDocumentDataController::documentIsOpened(const QString& documentId) const
{
	return IndexOfDocument(documentId) >= 0;
}


bool CGqlDocumentDataController::documentIsNew(const QString& documentId) const
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return true;
	}
	return m_openedDocuments[idx].isNew;
}


void CGqlDocumentDataController::setDocumentIsNew(const QString& documentId, bool isNew)
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return;
	}
	m_openedDocuments[idx].isNew = isNew;
}


bool CGqlDocumentDataController::documentIsDirty(const QString& documentId) const
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return false;
	}
	return m_openedDocuments[idx].isDirty;
}


void CGqlDocumentDataController::setDocumentIsDirty(const QString& documentId, bool isDirty)
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return;
	}
	m_openedDocuments[idx].isDirty = isDirty;
	Q_EMIT documentIsDirtyChanged(documentId, isDirty);
}


bool CGqlDocumentDataController::documentIsLoading(const QString& documentId) const
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return false;
	}
	return m_openedDocuments[idx].isLoading;
}


void CGqlDocumentDataController::setDocumentIsLoading(const QString& documentId, bool isLoading)
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return;
	}
	FOpenedDocument& docData = m_openedDocuments[idx];
	if (docData.isClosing){
		return;
	}

	docData.isLoading = isLoading;

	if (!isLoading){
		QObject* decorator = docData.documentDecorator.data();
		const bool isNew = docData.isNew;
		if (decorator != nullptr){
			if (!isNew){
				QMetaObject::invokeMethod(decorator, "updateRepresentationForAllViews",
						Qt::DirectConnection);
			}
			else{
				QVariant registeredViewsVar = decorator->property("registeredViews");
				const QVariantList registeredViews = registeredViewsVar.toList();
				for (const QVariant& viewVar : registeredViews){
					QObject* viewObj = viewVar.value<QObject*>();
					if (viewObj == nullptr){
						continue;
					}
					QMetaObject::invokeMethod(viewObj, "setBlockingUpdateModel",
							Qt::DirectConnection, Q_ARG(QVariant, false));
					QMetaObject::invokeMethod(viewObj, "doUpdateGui",
							Qt::DirectConnection);
				}
			}
		}
		Q_EMIT documentDataLoaded(documentId);
	}
}


int CGqlDocumentDataController::getDocumentIndexByDocumentId(const QString& documentId) const
{
	return IndexOfDocument(documentId);
}


QString CGqlDocumentDataController::getDocumentIdByObjectId(const QString& objectId) const
{
	for (const FOpenedDocument& doc : m_openedDocuments){
		if (doc.objectId == objectId){
			return doc.id;
		}
	}
	return QString();
}


QString CGqlDocumentDataController::getDocumentIdByView(QObject* view) const
{
	for (const FOpenedDocument& doc : m_openedDocuments){
		for (auto it = doc.views.constBegin(); it != doc.views.constEnd(); ++it){
			if (it.value().data() == view){
				return doc.id;
			}
		}
	}
	return QString();
}


QString CGqlDocumentDataController::getDocumentTypeId(const QString& documentId) const
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return QString();
	}
	return m_openedDocuments[idx].typeId;
}


void CGqlDocumentDataController::setDocumentObjectId(const QString& documentId, const QString& objectId)
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return;
	}
	m_openedDocuments[idx].objectId = objectId;
}


void CGqlDocumentDataController::setDocumentName(const QString& documentId, const QString& name)
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		m_cachedDocumentNames[documentId] = name;
		return;
	}
	const QString oldName = m_openedDocuments[idx].name;
	m_openedDocuments[idx].name = name;
	Q_EMIT documentNameChanged(documentId, oldName, name);
}


QString CGqlDocumentDataController::getDocumentName(const QString& documentId) const
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return QString();
	}
	return m_openedDocuments[idx].name;
}


QString CGqlDocumentDataController::getDefaultDocumentName() const
{
	return tr("<no name>");
}


// --- DocumentManagerBase: views ---

QObject* CGqlDocumentDataController::getDocumentViewInstance(
	const QString& documentId,
	const QString& viewTypeId) const
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		qWarning() << "Unable to get view for document" << documentId
				<< "Error: Document not found";
		return nullptr;
	}

	const QHash<QString, QPointer<QObject>>& views = m_openedDocuments[idx].views;
	if (views.contains(viewTypeId)){
		return views.value(viewTypeId).data();
	}
	if (viewTypeId.isEmpty() && !views.isEmpty()){
		return views.constBegin().value().data();
	}

	qWarning() << "Unable to get view for document" << documentId
			<< "Error: View" << viewTypeId << "not found";
	return nullptr;
}


void CGqlDocumentDataController::onViewInstanceCreated(
	const QString& documentId,
	QObject* view,
	const QString& viewTypeId)
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return;
	}

	QString resolvedViewTypeId = viewTypeId;
	if (resolvedViewTypeId.isEmpty()){
		const QString docTypeId = m_openedDocuments[idx].typeId;
		const QStringList viewTypeIds = getSupportedDocumentViewTypeIds(docTypeId);
		if (!viewTypeIds.isEmpty()){
			resolvedViewTypeId = viewTypeIds.first();
		}
	}

	m_openedDocuments[idx].views.insert(resolvedViewTypeId, view);
}


QObject* CGqlDocumentDataController::getDocumentManagerActiveView() const
{
	return m_documentManagerActiveView.data();
}


void CGqlDocumentDataController::setDocumentManagerActiveView(QObject* view)
{
	if (m_documentManagerActiveView.data() == view){
		return;
	}
	m_documentManagerActiveView = view;
	Q_EMIT documentManagerActiveViewChanged();
}


void CGqlDocumentDataController::setDocumentDecorator(const QString& documentId, QObject* decorator)
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return;
	}
	m_openedDocuments[idx].documentDecorator = decorator;
}


QObject* CGqlDocumentDataController::getDocumentDecorator(const QString& documentId) const
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return nullptr;
	}
	return m_openedDocuments[idx].documentDecorator.data();
}


// --- DocumentManagerBase: name providers ---

void CGqlDocumentDataController::setAutoNamedTypeId(const QString& typeId, bool hasProvider)
{
	m_autoNamedTypeIds.insert(typeId, hasProvider);
}


bool CGqlDocumentDataController::hasDocumentNameProvider(const QString& typeId) const
{
	const auto it = m_autoNamedTypeIds.constFind(typeId);
	if (it == m_autoNamedTypeIds.constEnd()){
		return false;
	}
	return *it;
}


// --- private helpers ---

CGqlClientBridge* CGqlDocumentDataController::ResolveBridge() const
{
	return CGqlClientBridge::Instance();
}


void CGqlDocumentDataController::CreateDocumentDataInternal(
	const QString& id, const QString& typeId, bool isNew)
{
	if (IndexOfDocument(id) >= 0){
		return;
	}
	FOpenedDocument data;
	data.id = id;
	data.typeId = typeId;
	data.isNew = isNew;

	const auto cachedNameIt = m_cachedDocumentNames.find(id);
	if (cachedNameIt != m_cachedDocumentNames.end()){
		data.name = cachedNameIt.value();
		m_cachedDocumentNames.erase(cachedNameIt);
	}

	m_openedDocuments.append(data);
	Q_EMIT openedDocumentIdsChanged();
}


void CGqlDocumentDataController::RemoveDocumentDataInternal(const QString& documentId)
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return;
	}
	m_openedDocuments.removeAt(idx);
	Q_EMIT openedDocumentIdsChanged();
}


int CGqlDocumentDataController::IndexOfDocument(const QString& documentId) const
{
	for (int i = 0; i < m_openedDocuments.size(); ++i){
		if (m_openedDocuments[i].id == documentId){
			return i;
		}
	}
	return -1;
}


// --- GQL transport (async dispatch via QtConcurrent) ---

void CGqlDocumentDataController::getOpenedDocumentList()
{
	CGqlClientBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT openedDocumentListReceiveFailed(
				QStringLiteral("GQL client bridge is not available"));
		return;
	}

	Q_EMIT startGetOpenedDocumentList();

	const QByteArray collectionId = m_collectionId.toUtf8();
	QPointer<CGqlDocumentDataController> self(this);

	QtConcurrent::run([self, bridge, collectionId]{
		docmgr::GetOpenedDocumentListRequestArguments arguments;
		arguments.input.Version_1_0.Emplace();
		arguments.input.Version_1_0->collectionId = collectionId;

		docmgr::CDocumentList payload;
		QString errorMessage;
		bool ok = bridge->SendSdlRequest<
				docmgr::GetOpenedDocumentListRequestArguments,
				docmgr::CDocumentList,
				docmgr::CGetOpenedDocumentListGqlRequest>(arguments, payload, errorMessage);

		if (!ok){
			const QString msg = errorMessage.isEmpty()
					? QStringLiteral("Failed to get opened document list")
					: errorMessage;
			PostToMainThread([self, msg]{
				if (self){
					Q_EMIT self->openedDocumentListReceiveFailed(msg);
				}
			});
			return;
		}

		QVariantList documents;
		if (payload.Version_1_0->documentList.HasValue()){
			for (const auto& docPtr : *payload.Version_1_0->documentList){
				if (!docPtr){
					continue;
				}
				const auto& doc = *docPtr;
				QVariantMap entry;
				entry.insert(QStringLiteral("documentId"),   OptByteArrayAsString(doc.documentId));
				entry.insert(QStringLiteral("documentName"), OptString(doc.documentName));
				entry.insert(QStringLiteral("objectTypeId"), OptByteArrayAsString(doc.objectTypeId));
				entry.insert(QStringLiteral("objectId"),     OptByteArrayAsString(doc.objectId));
				entry.insert(QStringLiteral("isDirty"),      OptBool(doc.isDirty));
				entry.insert(QStringLiteral("hasNameProvider"), OptBool(doc.hasNameProvider));
				entry.insert(QStringLiteral("isLoading"),    OptBool(doc.isLoading));
				documents.append(entry);
			}
		}

		PostToMainThread([self, documents]{
			if (self){
				Q_EMIT self->openedDocumentListReceived(QVariant(documents));
			}
		});
	});
}


void CGqlDocumentDataController::openDocument(const QString& typeId, const QString& documentId)
{
	const QString existingDocumentId = getDocumentIdByObjectId(documentId);
	if (!existingDocumentId.isEmpty()){
		Q_EMIT documentAlreadyOpened(existingDocumentId, typeId);
		return;
	}

	CGqlClientBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT openDocumentFailed(documentId,
				QStringLiteral("GQL client bridge is not available"));
		return;
	}

	Q_EMIT startOpenDocument(documentId, typeId);

	const QByteArray collectionId = m_collectionId.toUtf8();
	const QByteArray objectId = documentId.toUtf8();
	QPointer<CGqlDocumentDataController> self(this);

	QtConcurrent::run([self, bridge, collectionId, objectId]{
		docmgr::OpenDocumentRequestArguments arguments;
		arguments.input.Version_1_0.Emplace();
		arguments.input.Version_1_0->collectionId = collectionId;
		arguments.input.Version_1_0->id = objectId;

		docmgr::CDocumentInfo payload;
		QString errorMessage;
		bool ok = bridge->SendSdlRequest<
				docmgr::OpenDocumentRequestArguments,
				docmgr::CDocumentInfo,
				docmgr::COpenDocumentGqlRequest>(arguments, payload, errorMessage);

		if (!ok){
			const QString msg = errorMessage.isEmpty()
					? QStringLiteral("Failed to open document")
					: errorMessage;
			PostToMainThread([self, msg]{
				if (self){
					Q_EMIT self->openDocumentFailed(QString(), msg);
				}
			});
			return;
		}

		const auto& info = *payload.Version_1_0;
		const QString resolvedTypeId   = OptByteArrayAsString(info.objectTypeId);
		const QString resolvedDocId    = OptByteArrayAsString(info.documentId);
		const QString resolvedDocName  = OptString(info.documentName);
		const QString resolvedObjectId = OptByteArrayAsString(info.objectId);
		const bool    hasNameProvider  = OptBool(info.hasNameProvider);

		PostToMainThread([self, resolvedTypeId, resolvedDocId, resolvedDocName,
				resolvedObjectId, hasNameProvider]{
			if (!self){
				return;
			}
			self->setAutoNamedTypeId(resolvedTypeId, hasNameProvider);
			self->setDocumentName(resolvedDocId, resolvedDocName);
			self->CreateDocumentDataInternal(resolvedDocId, resolvedTypeId, false);
			self->setDocumentObjectId(resolvedDocId, resolvedObjectId);
			self->setDocumentIsLoading(resolvedDocId, true);
			Q_EMIT self->documentOpened(resolvedDocId, resolvedTypeId);
		});
	});
}


void CGqlDocumentDataController::createDocument(const QString& typeId)
{
	CGqlClientBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT createDocumentFailed(typeId,
				QStringLiteral("GQL client bridge is not available"));
		return;
	}

	Q_EMIT startCreateDocument(typeId);

	const QByteArray collectionId = m_collectionId.toUtf8();
	const QByteArray typeIdUtf8 = typeId.toUtf8();
	QPointer<CGqlDocumentDataController> self(this);

	QtConcurrent::run([self, bridge, collectionId, typeIdUtf8]{
		docmgr::CreateNewDocumentRequestArguments arguments;
		arguments.input.Version_1_0.Emplace();
		arguments.input.Version_1_0->collectionId = collectionId;
		arguments.input.Version_1_0->typeId = typeIdUtf8;

		docmgr::CDocumentInfo payload;
		QString errorMessage;
		bool ok = bridge->SendSdlRequest<
				docmgr::CreateNewDocumentRequestArguments,
				docmgr::CDocumentInfo,
				docmgr::CCreateNewDocumentGqlRequest>(arguments, payload, errorMessage);

		if (!ok){
			const QString msg = errorMessage.isEmpty()
					? QStringLiteral("Failed to create document")
					: errorMessage;
			PostToMainThread([self, msg]{
				if (self){
					Q_EMIT self->createDocumentFailed(QString(), msg);
				}
			});
			return;
		}

		const auto& info = *payload.Version_1_0;
		const QString resolvedTypeId  = OptByteArrayAsString(info.objectTypeId);
		const QString resolvedDocId   = OptByteArrayAsString(info.documentId);
		const QString resolvedDocName = OptString(info.documentName);
		const bool    hasNameProvider = OptBool(info.hasNameProvider);

		PostToMainThread([self, resolvedTypeId, resolvedDocId, resolvedDocName,
				hasNameProvider]{
			if (!self){
				return;
			}
			self->setAutoNamedTypeId(resolvedTypeId, hasNameProvider);
			self->setDocumentName(resolvedDocId, resolvedDocName);
			self->CreateDocumentDataInternal(resolvedDocId, resolvedTypeId, true);
			self->setDocumentIsLoading(resolvedDocId, true);
			Q_EMIT self->documentCreated(resolvedDocId, resolvedTypeId);
		});
	});
}


void CGqlDocumentDataController::saveDocument(const QString& documentId, const QString& documentName)
{
	CGqlClientBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT saveDocumentFailed(documentId,
				QStringLiteral("GQL client bridge is not available"));
		return;
	}

	Q_EMIT startSaveDocument(documentId);

	const QByteArray collectionId = m_collectionId.toUtf8();
	const QByteArray documentIdUtf8 = documentId.toUtf8();
	const QString documentIdStr = documentId;
	const QString resolvedName = documentName.isEmpty() ? getDocumentName(documentId) : documentName;
	QPointer<CGqlDocumentDataController> self(this);

	QtConcurrent::run([self, bridge, collectionId, documentIdUtf8, documentIdStr, resolvedName]{
		docmgr::SaveDocumentRequestArguments arguments;
		arguments.input.Version_1_0.Emplace();
		arguments.input.Version_1_0->collectionId = collectionId;
		arguments.input.Version_1_0->documentId = documentIdUtf8;
		arguments.input.Version_1_0->documentName = resolvedName;

		docmgr::CDocumentOperationStatus payload;
		QString errorMessage;
		bool ok = bridge->SendSdlRequest<
				docmgr::SaveDocumentRequestArguments,
				docmgr::CDocumentOperationStatus,
				docmgr::CSaveDocumentGqlRequest>(arguments, payload, errorMessage);

		if (!ok){
			const QString msg = errorMessage.isEmpty()
					? QStringLiteral("Failed to save document")
					: errorMessage;
			PostToMainThread([self, documentIdStr, msg]{
				if (self){
					Q_EMIT self->saveDocumentFailed(documentIdStr, msg);
				}
			});
			return;
		}

		const auto& status = *payload.Version_1_0;
		const auto statusValue = status.status.HasValue() ? *status.status
				: docmgr::EDocumentOperationStatus::Failed;
		const QString message = OptString(status.message);
		const QString resolvedDocName = status.documentName.HasValue()
				? *status.documentName : resolvedName;

		auto statusMessage = [&](const QString& defaultMessage){
			return message.isEmpty() ? defaultMessage : message;
		};

		if (statusValue == docmgr::EDocumentOperationStatus::Success){
			PostToMainThread([self, documentIdStr, resolvedDocName]{
				if (!self){
					return;
				}
				if (!resolvedDocName.isEmpty()){
					self->setDocumentName(documentIdStr, resolvedDocName);
				}
				Q_EMIT self->documentSaved(documentIdStr);
			});
			return;
		}

		QString failureMessage;
		switch (statusValue){
			case docmgr::EDocumentOperationStatus::InvalidUserId:
				failureMessage = statusMessage(tr("Invalid user-ID"));
				break;
			case docmgr::EDocumentOperationStatus::InvalidDocumentId:
				failureMessage = statusMessage(tr("Invalid document-ID"));
				break;
			case docmgr::EDocumentOperationStatus::InvalidDocumentData:
				failureMessage = statusMessage(tr("Document data is invalid"));
				break;
			default:
				failureMessage = statusMessage(tr("Save document failed"));
				break;
		}

		PostToMainThread([self, documentIdStr, failureMessage]{
			if (self){
				Q_EMIT self->saveDocumentFailed(documentIdStr, failureMessage);
			}
		});
	});
}


void CGqlDocumentDataController::closeDocument(const QString& documentId)
{
	CGqlClientBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT closeDocumentFailed(documentId,
				QStringLiteral("GQL client bridge is not available"));
		return;
	}

	Q_EMIT startCloseDocument(documentId);

	const QByteArray collectionId = m_collectionId.toUtf8();
	const QByteArray documentIdUtf8 = documentId.toUtf8();
	const QString documentIdStr = documentId;
	QPointer<CGqlDocumentDataController> self(this);

	QtConcurrent::run([self, bridge, collectionId, documentIdUtf8, documentIdStr]{
		docmgr::CloseDocumentRequestArguments arguments;
		arguments.input.Version_1_0.Emplace();
		arguments.input.Version_1_0->collectionId = collectionId;
		arguments.input.Version_1_0->id = documentIdUtf8;

		docmgr::CDocumentOperationStatus payload;
		QString errorMessage;
		bool ok = bridge->SendSdlRequest<
				docmgr::CloseDocumentRequestArguments,
				docmgr::CDocumentOperationStatus,
				docmgr::CCloseDocumentGqlRequest>(arguments, payload, errorMessage);

		if (!ok){
			const QString msg = errorMessage.isEmpty()
					? QStringLiteral("Failed to close document")
					: errorMessage;
			PostToMainThread([self, documentIdStr, msg]{
				if (self){
					Q_EMIT self->closeDocumentFailed(documentIdStr, msg);
				}
			});
			return;
		}

		const auto& status = *payload.Version_1_0;
		const auto statusValue = status.status.HasValue() ? *status.status
				: docmgr::EDocumentOperationStatus::Failed;

		if (statusValue == docmgr::EDocumentOperationStatus::Success){
			PostToMainThread([self, documentIdStr]{
				if (self){
					Q_EMIT self->documentClosed(documentIdStr);
				}
			});
			return;
		}

		QString failureMessage;
		switch (statusValue){
			case docmgr::EDocumentOperationStatus::InvalidUserId:
				failureMessage = tr("Invalid user-ID");
				break;
			case docmgr::EDocumentOperationStatus::InvalidDocumentId:
				failureMessage = tr("Invalid document-ID");
				break;
			default:
				failureMessage = tr("Close document failed");
				break;
		}

		PostToMainThread([self, documentIdStr, failureMessage]{
			if (self){
				Q_EMIT self->closeDocumentFailed(documentIdStr, failureMessage);
			}
		});
	});
}


namespace
{


/// Translate an SDL undo status enum value into a localised message.
QString UndoStatusToMessage(sdl::imtbase::UndoManager::EUndoStatus status)
{
	using S = sdl::imtbase::UndoManager::EUndoStatus;
	switch (status){
		case S::InvalidUserId:     return CGqlDocumentDataController::tr("Invalid user-ID");
		case S::InvalidDocumentId: return CGqlDocumentDataController::tr("Invalid document-ID");
		case S::InvalidStepCount:  return CGqlDocumentDataController::tr("Invalid step count");
		case S::Failed:            return CGqlDocumentDataController::tr("Operation failed");
		default:                   return QString();
	}
}


} // namespace


void CGqlDocumentDataController::doUndo(const QString& documentId, int steps)
{
	CGqlClientBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT undoFailed(documentId,
				QStringLiteral("GQL client bridge is not available"));
		return;
	}

	Q_EMIT startUndo(documentId, steps);

	const QByteArray collectionId = m_collectionId.toUtf8();
	const QByteArray documentIdUtf8 = documentId.toUtf8();
	const QString documentIdStr = documentId;
	QPointer<CGqlDocumentDataController> self(this);

	QtConcurrent::run([self, bridge, collectionId, documentIdUtf8, documentIdStr, steps]{
		docmgr::DoUndoRequestArguments arguments;
		arguments.input.Version_1_0.Emplace();
		arguments.input.Version_1_0->collectionId = collectionId;
		arguments.input.Version_1_0->undoRedoInput.Emplace();
		arguments.input.Version_1_0->undoRedoInput->documentId = documentIdUtf8;
		arguments.input.Version_1_0->undoRedoInput->steps = steps;

		sdl::imtbase::UndoManager::CUndoStatus payload;
		QString errorMessage;
		bool ok = bridge->SendSdlRequest<
				docmgr::DoUndoRequestArguments,
				sdl::imtbase::UndoManager::CUndoStatus,
				docmgr::CDoUndoGqlRequest>(arguments, payload, errorMessage);

		if (!ok){
			const QString msg = errorMessage.isEmpty()
					? QStringLiteral("Undo failed") : errorMessage;
			PostToMainThread([self, documentIdStr, msg]{
				if (self){
					Q_EMIT self->undoFailed(documentIdStr, msg);
				}
			});
			return;
		}

		const auto statusValue = payload.Version_1_0->status.HasValue()
				? *payload.Version_1_0->status
				: sdl::imtbase::UndoManager::EUndoStatus::Failed;
		if (statusValue == sdl::imtbase::UndoManager::EUndoStatus::Success){
			PostToMainThread([self, documentIdStr]{
				if (self){
					Q_EMIT self->undoDone(documentIdStr);
				}
			});
			return;
		}

		const QString msg = UndoStatusToMessage(statusValue);
		PostToMainThread([self, documentIdStr, msg]{
			if (self){
				Q_EMIT self->undoFailed(documentIdStr, msg);
			}
		});
	});
}


void CGqlDocumentDataController::doRedo(const QString& documentId, int steps)
{
	CGqlClientBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT redoFailed(documentId,
				QStringLiteral("GQL client bridge is not available"));
		return;
	}

	Q_EMIT startRedo(documentId, steps);

	const QByteArray collectionId = m_collectionId.toUtf8();
	const QByteArray documentIdUtf8 = documentId.toUtf8();
	const QString documentIdStr = documentId;
	QPointer<CGqlDocumentDataController> self(this);

	QtConcurrent::run([self, bridge, collectionId, documentIdUtf8, documentIdStr, steps]{
		docmgr::DoRedoRequestArguments arguments;
		arguments.input.Version_1_0.Emplace();
		arguments.input.Version_1_0->collectionId = collectionId;
		arguments.input.Version_1_0->undoRedoInput.Emplace();
		arguments.input.Version_1_0->undoRedoInput->documentId = documentIdUtf8;
		arguments.input.Version_1_0->undoRedoInput->steps = steps;

		sdl::imtbase::UndoManager::CUndoStatus payload;
		QString errorMessage;
		bool ok = bridge->SendSdlRequest<
				docmgr::DoRedoRequestArguments,
				sdl::imtbase::UndoManager::CUndoStatus,
				docmgr::CDoRedoGqlRequest>(arguments, payload, errorMessage);

		if (!ok){
			const QString msg = errorMessage.isEmpty()
					? QStringLiteral("Redo failed") : errorMessage;
			PostToMainThread([self, documentIdStr, msg]{
				if (self){
					Q_EMIT self->redoFailed(documentIdStr, msg);
				}
			});
			return;
		}

		const auto statusValue = payload.Version_1_0->status.HasValue()
				? *payload.Version_1_0->status
				: sdl::imtbase::UndoManager::EUndoStatus::Failed;
		if (statusValue == sdl::imtbase::UndoManager::EUndoStatus::Success){
			PostToMainThread([self, documentIdStr]{
				if (self){
					Q_EMIT self->redoDone(documentIdStr);
				}
			});
			return;
		}

		const QString msg = UndoStatusToMessage(statusValue);
		PostToMainThread([self, documentIdStr, msg]{
			if (self){
				Q_EMIT self->redoFailed(documentIdStr, msg);
			}
		});
	});
}


void CGqlDocumentDataController::resetUndo(const QString& documentId)
{
	CGqlClientBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT resetUndoFailed(documentId,
				QStringLiteral("GQL client bridge is not available"));
		return;
	}

	Q_EMIT startResetUndo(documentId);

	const QByteArray collectionId = m_collectionId.toUtf8();
	const QByteArray documentIdUtf8 = documentId.toUtf8();
	const QString documentIdStr = documentId;
	QPointer<CGqlDocumentDataController> self(this);

	QtConcurrent::run([self, bridge, collectionId, documentIdUtf8, documentIdStr]{
		docmgr::ResetUndoRequestArguments arguments;
		arguments.input.Version_1_0.Emplace();
		arguments.input.Version_1_0->collectionId = collectionId;
		arguments.input.Version_1_0->id = documentIdUtf8;

		sdl::imtbase::UndoManager::CUndoStatus payload;
		QString errorMessage;
		bool ok = bridge->SendSdlRequest<
				docmgr::ResetUndoRequestArguments,
				sdl::imtbase::UndoManager::CUndoStatus,
				docmgr::CResetUndoGqlRequest>(arguments, payload, errorMessage);

		if (!ok){
			const QString msg = errorMessage.isEmpty()
					? QStringLiteral("Reset undo failed") : errorMessage;
			PostToMainThread([self, documentIdStr, msg]{
				if (self){
					Q_EMIT self->resetUndoFailed(documentIdStr, msg);
				}
			});
			return;
		}

		const auto statusValue = payload.Version_1_0->status.HasValue()
				? *payload.Version_1_0->status
				: sdl::imtbase::UndoManager::EUndoStatus::Failed;
		if (statusValue == sdl::imtbase::UndoManager::EUndoStatus::Success){
			PostToMainThread([self, documentIdStr]{
				if (self){
					Q_EMIT self->resetUndoDone(documentIdStr);
				}
			});
			return;
		}

		const QString msg = UndoStatusToMessage(statusValue);
		PostToMainThread([self, documentIdStr, msg]{
			if (self){
				Q_EMIT self->resetUndoFailed(documentIdStr, msg);
			}
		});
	});
}


void CGqlDocumentDataController::getUndoInfo(const QString& documentId)
{
	CGqlClientBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT undoInfoReceiveFailed(documentId,
				QStringLiteral("GQL client bridge is not available"));
		return;
	}

	Q_EMIT startUndoInfoReceive(documentId);

	const QByteArray collectionId = m_collectionId.toUtf8();
	const QByteArray documentIdUtf8 = documentId.toUtf8();
	const QString documentIdStr = documentId;
	QPointer<CGqlDocumentDataController> self(this);

	QtConcurrent::run([self, bridge, collectionId, documentIdUtf8, documentIdStr]{
		docmgr::GetUndoInfoRequestArguments arguments;
		arguments.input.Version_1_0.Emplace();
		arguments.input.Version_1_0->collectionId = collectionId;
		arguments.input.Version_1_0->id = documentIdUtf8;

		sdl::imtbase::UndoManager::CUndoInfo payload;
		QString errorMessage;
		bool ok = bridge->SendSdlRequest<
				docmgr::GetUndoInfoRequestArguments,
				sdl::imtbase::UndoManager::CUndoInfo,
				docmgr::CGetUndoInfoGqlRequest>(arguments, payload, errorMessage);

		if (!ok){
			const QString msg = errorMessage.isEmpty()
					? QStringLiteral("Failed to get undo info") : errorMessage;
			PostToMainThread([self, documentIdStr, msg]{
				if (self){
					Q_EMIT self->undoInfoReceiveFailed(documentIdStr, msg);
				}
			});
			return;
		}

		const auto& info = *payload.Version_1_0;
		const QString resolvedDocId = info.documentId.HasValue()
				? QString::fromUtf8(*info.documentId) : documentIdStr;
		const int undoSteps = OptInt(info.availableUndoSteps);
		const int redoSteps = OptInt(info.availableRedoSteps);
		const bool isDirty  = OptBool(info.isDirty);

		PostToMainThread([self, resolvedDocId, undoSteps, redoSteps, isDirty]{
			if (self){
				Q_EMIT self->undoInfoReceived(resolvedDocId, undoSteps, redoSteps, isDirty);
			}
		});
	});
}


} // namespace imtqml
