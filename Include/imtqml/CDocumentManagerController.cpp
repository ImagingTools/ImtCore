// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CDocumentManagerController.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QPointer>

// ImtCore includes
#include <imtqml/CDocumentManagerBridge.h>
#include <imtqml/IDocumentManagerBridge.h>


namespace imtqml
{


// public methods

CDocumentManagerController::CDocumentManagerController(QObject* parent)
	:BaseClass(parent)
{
	// Mirror QML behaviour: when collectionId becomes non-empty, fetch
	// the opened-document list automatically.
	connect(this, &CDocumentManagerController::collectionIdChanged, this,
			[this](const QString& id){
				if (!id.isEmpty()){
					getOpenedDocumentList();
				}
			});

	// Mirror QML onDocumentSaved / onUndoInfoReceived / onStartCloseDocument /
	// onDocumentClosed / onDocumentCreated / onDocumentOpened wiring.
	connect(this, &CDocumentManagerController::documentSaved, this,
			[this](const QString& documentId){
				setDocumentIsNew(documentId, false);
			});
	connect(this, &CDocumentManagerController::undoInfoReceived, this,
			[this](const QString& documentId, int, int, bool isDirty){
				setDocumentIsDirty(documentId, isDirty);
			});
	connect(this, &CDocumentManagerController::startCloseDocument, this,
			[this](const QString& documentId){
				int idx = IndexOfDocument(documentId);
				if (idx >= 0){
					m_openedDocuments[idx].isClosing = true;
				}
			});
	connect(this, &CDocumentManagerController::documentClosed, this,
			[this](const QString& documentId){
				RemoveDocumentDataInternal(documentId);
			});
	connect(this, &CDocumentManagerController::documentCreated, this,
			[this](const QString& documentId, const QString& typeId){
				CreateDocumentDataInternal(documentId, typeId, true);
			});
	connect(this, &CDocumentManagerController::documentOpened, this,
			[this](const QString& documentId, const QString& typeId){
				CreateDocumentDataInternal(documentId, typeId, false);
			});
}


CDocumentManagerController::~CDocumentManagerController() = default;


const QString& CDocumentManagerController::GetCollectionId() const
{
	return m_collectionId;
}


void CDocumentManagerController::SetCollectionId(const QString& id)
{
	if (m_collectionId != id){
		m_collectionId = id;
		Q_EMIT collectionIdChanged(m_collectionId);
	}
}


// --- DocumentManagerBase: registration / introspection ---

void CDocumentManagerController::registerDocumentViewData(
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


QObject* CDocumentManagerController::getDocumentEditorFactory(
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


QObject* CDocumentManagerController::getDocumentRepresentationControllerFactory(
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


QStringList CDocumentManagerController::getSupportedDocumentTypeIds() const
{
	return m_documentTypeEditors.keys();
}


QStringList CDocumentManagerController::getSupportedDocumentViewTypeIds(
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


QString CDocumentManagerController::getViewTypeIdByViewFactory(
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

QStringList CDocumentManagerController::getOpenedDocumentIds() const
{
	QStringList result;
	result.reserve(m_openedDocuments.size());
	for (const FOpenedDocument& doc : m_openedDocuments){
		result.append(doc.id);
	}
	return result;
}


bool CDocumentManagerController::documentIsOpened(const QString& documentId) const
{
	return IndexOfDocument(documentId) >= 0;
}


bool CDocumentManagerController::documentIsNew(const QString& documentId) const
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return true;
	}
	return m_openedDocuments[idx].isNew;
}


void CDocumentManagerController::setDocumentIsNew(const QString& documentId, bool isNew)
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return;
	}
	m_openedDocuments[idx].isNew = isNew;
}


bool CDocumentManagerController::documentIsDirty(const QString& documentId) const
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return false;
	}
	return m_openedDocuments[idx].isDirty;
}


void CDocumentManagerController::setDocumentIsDirty(const QString& documentId, bool isDirty)
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return;
	}
	m_openedDocuments[idx].isDirty = isDirty;
	Q_EMIT documentIsDirtyChanged(documentId, isDirty);
}


bool CDocumentManagerController::documentIsLoading(const QString& documentId) const
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return false;
	}
	return m_openedDocuments[idx].isLoading;
}


void CDocumentManagerController::setDocumentIsLoading(const QString& documentId, bool isLoading)
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


int CDocumentManagerController::getDocumentIndexByDocumentId(const QString& documentId) const
{
	return IndexOfDocument(documentId);
}


QString CDocumentManagerController::getDocumentIdByObjectId(const QString& objectId) const
{
	for (const FOpenedDocument& doc : m_openedDocuments){
		if (doc.objectId == objectId){
			return doc.id;
		}
	}
	return QString();
}


QString CDocumentManagerController::getDocumentIdByView(QObject* view) const
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


QString CDocumentManagerController::getDocumentTypeId(const QString& documentId) const
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return QString();
	}
	return m_openedDocuments[idx].typeId;
}


void CDocumentManagerController::setDocumentObjectId(const QString& documentId, const QString& objectId)
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return;
	}
	m_openedDocuments[idx].objectId = objectId;
}


void CDocumentManagerController::setDocumentName(const QString& documentId, const QString& name)
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


QString CDocumentManagerController::getDocumentName(const QString& documentId) const
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return QString();
	}
	return m_openedDocuments[idx].name;
}


QString CDocumentManagerController::getDefaultDocumentName() const
{
	return tr("<no name>");
}


// --- DocumentManagerBase: views ---

QObject* CDocumentManagerController::getDocumentViewInstance(
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


void CDocumentManagerController::onViewInstanceCreated(
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


QObject* CDocumentManagerController::getDocumentManagerActiveView() const
{
	return m_documentManagerActiveView.data();
}


void CDocumentManagerController::setDocumentManagerActiveView(QObject* view)
{
	if (m_documentManagerActiveView.data() == view){
		return;
	}
	m_documentManagerActiveView = view;
	Q_EMIT documentManagerActiveViewChanged();
}


void CDocumentManagerController::setDocumentDecorator(const QString& documentId, QObject* decorator)
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return;
	}
	m_openedDocuments[idx].documentDecorator = decorator;
}


QObject* CDocumentManagerController::getDocumentDecorator(const QString& documentId) const
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return nullptr;
	}
	return m_openedDocuments[idx].documentDecorator.data();
}


// --- DocumentManagerBase: name providers ---

void CDocumentManagerController::setAutoNamedTypeId(const QString& typeId, bool hasProvider)
{
	m_autoNamedTypeIds.insert(typeId, hasProvider);
}


bool CDocumentManagerController::hasDocumentNameProvider(const QString& typeId) const
{
	const auto it = m_autoNamedTypeIds.constFind(typeId);
	if (it == m_autoNamedTypeIds.constEnd()){
		return false;
	}
	return *it;
}


// --- private helpers ---

IDocumentManagerBridge* CDocumentManagerController::ResolveBridge() const
{
	return CDocumentManagerBridge::Instance();
}


void CDocumentManagerController::CreateDocumentDataInternal(
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


void CDocumentManagerController::RemoveDocumentDataInternal(const QString& documentId)
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return;
	}
	m_openedDocuments.removeAt(idx);
	Q_EMIT openedDocumentIdsChanged();
}


int CDocumentManagerController::IndexOfDocument(const QString& documentId) const
{
	for (int i = 0; i < m_openedDocuments.size(); ++i){
		if (m_openedDocuments[i].id == documentId){
			return i;
		}
	}
	return -1;
}


// --- Transport (delegated to IDocumentManagerBridge) ---

namespace
{

QString NoBridgeError()
{
	return QStringLiteral("CDocumentManagerController: no IDocumentManagerBridge available "
			"(CDocumentManagerBridge component is not loaded)");
}

} // namespace


void CDocumentManagerController::getOpenedDocumentList()
{
	IDocumentManagerBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT openedDocumentListReceiveFailed(NoBridgeError());
		return;
	}

	Q_EMIT startGetOpenedDocumentList();

	QPointer<CDocumentManagerController> self(this);
	bridge->GetOpenedDocumentList(m_collectionId,
			[self](QList<IDocumentManagerBridge::OpenedDocumentInfo> list, QString errorMessage){
				if (!self){
					return;
				}
				if (!errorMessage.isEmpty()){
					Q_EMIT self->openedDocumentListReceiveFailed(errorMessage);
					return;
				}
				QVariantList documents;
				documents.reserve(list.size());
				for (const auto& info : list){
					QVariantMap entry;
					entry.insert(QStringLiteral("documentId"),      info.documentId);
					entry.insert(QStringLiteral("documentName"),    info.name);
					entry.insert(QStringLiteral("objectTypeId"),    info.typeId);
					entry.insert(QStringLiteral("objectId"),        info.objectId);
					entry.insert(QStringLiteral("hasNameProvider"), info.hasNameProvider);
					documents.append(entry);
				}
				Q_EMIT self->openedDocumentListReceived(QVariant(documents));
			});
}


void CDocumentManagerController::openDocument(const QString& typeId, const QString& documentId)
{
	const QString existingDocumentId = getDocumentIdByObjectId(documentId);
	if (!existingDocumentId.isEmpty()){
		Q_EMIT documentAlreadyOpened(existingDocumentId, typeId);
		return;
	}

	IDocumentManagerBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT openDocumentFailed(documentId, NoBridgeError());
		return;
	}

	Q_EMIT startOpenDocument(documentId, typeId);

	QPointer<CDocumentManagerController> self(this);
	bridge->OpenDocument(m_collectionId, typeId, documentId,
			[self](IDocumentManagerBridge::OpenedDocumentInfo info, QString errorMessage){
				if (!self){
					return;
				}
				if (!errorMessage.isEmpty()){
					Q_EMIT self->openDocumentFailed(info.documentId, errorMessage);
					return;
				}
				self->setAutoNamedTypeId(info.typeId, info.hasNameProvider);
				self->setDocumentName(info.documentId, info.name);
				self->CreateDocumentDataInternal(info.documentId, info.typeId, false);
				self->setDocumentObjectId(info.documentId, info.objectId);
				self->setDocumentIsLoading(info.documentId, true);
				Q_EMIT self->documentOpened(info.documentId, info.typeId);
			});
}


void CDocumentManagerController::createDocument(const QString& typeId)
{
	IDocumentManagerBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT createDocumentFailed(typeId, NoBridgeError());
		return;
	}

	Q_EMIT startCreateDocument(typeId);

	QPointer<CDocumentManagerController> self(this);
	bridge->CreateDocument(m_collectionId, typeId,
			[self, typeId](IDocumentManagerBridge::OpenedDocumentInfo info, QString errorMessage){
				if (!self){
					return;
				}
				if (!errorMessage.isEmpty()){
					Q_EMIT self->createDocumentFailed(typeId, errorMessage);
					return;
				}
				const QString resolvedTypeId = info.typeId.isEmpty() ? typeId : info.typeId;
				self->setAutoNamedTypeId(resolvedTypeId, info.hasNameProvider);
				self->setDocumentName(info.documentId, info.name);
				self->CreateDocumentDataInternal(info.documentId, resolvedTypeId, true);
				self->setDocumentIsLoading(info.documentId, true);
				Q_EMIT self->documentCreated(info.documentId, resolvedTypeId);
			});
}


void CDocumentManagerController::saveDocument(const QString& documentId, const QString& documentName)
{
	IDocumentManagerBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT saveDocumentFailed(documentId, NoBridgeError());
		return;
	}

	Q_EMIT startSaveDocument(documentId);

	const QString resolvedName = documentName.isEmpty() ? getDocumentName(documentId) : documentName;
	QPointer<CDocumentManagerController> self(this);
	bridge->SaveDocument(m_collectionId, documentId, resolvedName,
			[self, documentId, resolvedName](IDocumentManagerBridge::OperationStatus status, QString errorMessage){
				if (!self){
					return;
				}
				if (status == IDocumentManagerBridge::OS_OK && errorMessage.isEmpty()){
					if (!resolvedName.isEmpty()){
						self->setDocumentName(documentId, resolvedName);
					}
					Q_EMIT self->documentSaved(documentId);
					return;
				}
				const QString msg = errorMessage.isEmpty()
						? CDocumentManagerController::tr("Save document failed")
						: errorMessage;
				Q_EMIT self->saveDocumentFailed(documentId, msg);
			});
}


void CDocumentManagerController::closeDocument(const QString& documentId)
{
	IDocumentManagerBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT closeDocumentFailed(documentId, NoBridgeError());
		return;
	}

	Q_EMIT startCloseDocument(documentId);

	QPointer<CDocumentManagerController> self(this);
	bridge->CloseDocument(m_collectionId, documentId,
			[self, documentId](QString errorMessage){
				if (!self){
					return;
				}
				if (!errorMessage.isEmpty()){
					Q_EMIT self->closeDocumentFailed(documentId, errorMessage);
					return;
				}
				Q_EMIT self->documentClosed(documentId);
			});
}


void CDocumentManagerController::doUndo(const QString& documentId, int steps)
{
	IDocumentManagerBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT undoFailed(documentId, NoBridgeError());
		return;
	}

	Q_EMIT startUndo(documentId, steps);

	QPointer<CDocumentManagerController> self(this);
	bridge->DoUndo(m_collectionId, documentId, steps,
			[self, documentId](QString errorMessage){
				if (!self){
					return;
				}
				if (!errorMessage.isEmpty()){
					Q_EMIT self->undoFailed(documentId, errorMessage);
					return;
				}
				Q_EMIT self->undoDone(documentId);
			});
}


void CDocumentManagerController::doRedo(const QString& documentId, int steps)
{
	IDocumentManagerBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT redoFailed(documentId, NoBridgeError());
		return;
	}

	Q_EMIT startRedo(documentId, steps);

	QPointer<CDocumentManagerController> self(this);
	bridge->DoRedo(m_collectionId, documentId, steps,
			[self, documentId](QString errorMessage){
				if (!self){
					return;
				}
				if (!errorMessage.isEmpty()){
					Q_EMIT self->redoFailed(documentId, errorMessage);
					return;
				}
				Q_EMIT self->redoDone(documentId);
			});
}


void CDocumentManagerController::resetUndo(const QString& documentId)
{
	IDocumentManagerBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT resetUndoFailed(documentId, NoBridgeError());
		return;
	}

	Q_EMIT startResetUndo(documentId);

	QPointer<CDocumentManagerController> self(this);
	bridge->ResetUndo(m_collectionId, documentId,
			[self, documentId](QString errorMessage){
				if (!self){
					return;
				}
				if (!errorMessage.isEmpty()){
					Q_EMIT self->resetUndoFailed(documentId, errorMessage);
					return;
				}
				Q_EMIT self->resetUndoDone(documentId);
			});
}


void CDocumentManagerController::getUndoInfo(const QString& documentId)
{
	IDocumentManagerBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT undoInfoReceiveFailed(documentId, NoBridgeError());
		return;
	}

	Q_EMIT startUndoInfoReceive(documentId);

	QPointer<CDocumentManagerController> self(this);
	bridge->GetUndoInfo(m_collectionId, documentId,
			[self, documentId](int undoSteps, int redoSteps, bool isDirty, QString errorMessage){
				if (!self){
					return;
				}
				if (!errorMessage.isEmpty()){
					Q_EMIT self->undoInfoReceiveFailed(documentId, errorMessage);
					return;
				}
				Q_EMIT self->undoInfoReceived(documentId, undoSteps, redoSteps, isDirty);
			});
}


} // namespace imtqml
