// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CDocumentServiceController.h>


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QPointer>

// ImtCore includes
#include <imtqml/CDocumentServiceBridge.h>
#include <imtqml/IDocumentServiceBridge.h>


namespace imtqml
{


// public methods

CDocumentServiceController::CDocumentServiceController(QObject* parent)
	:BaseClass(parent)
{
	// Mirror QML behaviour: when collectionId becomes non-empty, fetch
	// the opened-document list automatically.
	connect(this, &CDocumentServiceController::collectionIdChanged, this,
			[this](const QString& id){
				if (!id.isEmpty()){
					getOpenedDocumentList();
				}
			});

	// Mirror QML onDocumentSaved / onUndoInfoReceived / onStartCloseDocument /
	// onDocumentClosed / onDocumentCreated / onDocumentOpened wiring.
	connect(this, &CDocumentServiceController::documentSaved, this,
			[this](const QString& documentId){
				setDocumentIsNew(documentId, false);
			});
	connect(this, &CDocumentServiceController::undoInfoReceived, this,
			[this](const QString& documentId, int, int, bool isDirty){
				setDocumentIsDirty(documentId, isDirty);
			});
	connect(this, &CDocumentServiceController::startCloseDocument, this,
			[this](const QString& documentId){
				int idx = IndexOfDocument(documentId);
				if (idx >= 0){
					m_openedDocuments[idx].isClosing = true;
				}
			});
	connect(this, &CDocumentServiceController::documentClosed, this,
			[this](const QString& documentId){
				RemoveDocumentDataInternal(documentId);
			});
	connect(this, &CDocumentServiceController::documentCreated, this,
			[this](const QString& documentId, const QString& typeId){
				CreateDocumentDataInternal(documentId, typeId, true);
			});
	connect(this, &CDocumentServiceController::documentOpened, this,
			[this](const QString& documentId, const QString& typeId){
				CreateDocumentDataInternal(documentId, typeId, false);
			});
}


CDocumentServiceController::~CDocumentServiceController() = default;


const QString& CDocumentServiceController::GetCollectionId() const
{
	return m_collectionId;
}


void CDocumentServiceController::SetCollectionId(const QString& id)
{
	if (m_collectionId != id){
		m_collectionId = id;
		Q_EMIT collectionIdChanged(m_collectionId);
	}
}


// --- DocumentServiceBase: registration / introspection ---

void CDocumentServiceController::registerDocumentViewData(
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


QObject* CDocumentServiceController::getDocumentEditorFactory(
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


QObject* CDocumentServiceController::getDocumentRepresentationControllerFactory(
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


QStringList CDocumentServiceController::getSupportedDocumentTypeIds() const
{
	return m_documentTypeEditors.keys();
}


QStringList CDocumentServiceController::getSupportedDocumentViewTypeIds(
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


QString CDocumentServiceController::getViewTypeIdByViewFactory(
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


// --- DocumentServiceBase: opened-document state ---

QStringList CDocumentServiceController::getOpenedDocumentIds() const
{
	QStringList result;
	result.reserve(m_openedDocuments.size());
	for (const FOpenedDocument& doc : m_openedDocuments){
		result.append(doc.id);
	}
	return result;
}


bool CDocumentServiceController::documentIsOpened(const QString& documentId) const
{
	return IndexOfDocument(documentId) >= 0;
}


bool CDocumentServiceController::documentIsNew(const QString& documentId) const
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return true;
	}
	return m_openedDocuments[idx].isNew;
}


void CDocumentServiceController::setDocumentIsNew(const QString& documentId, bool isNew)
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return;
	}
	m_openedDocuments[idx].isNew = isNew;
}


bool CDocumentServiceController::documentIsDirty(const QString& documentId) const
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return false;
	}
	return m_openedDocuments[idx].isDirty;
}


void CDocumentServiceController::setDocumentIsDirty(const QString& documentId, bool isDirty)
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return;
	}
	m_openedDocuments[idx].isDirty = isDirty;
	Q_EMIT documentIsDirtyChanged(documentId, isDirty);
}


bool CDocumentServiceController::documentIsLoading(const QString& documentId) const
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return false;
	}
	return m_openedDocuments[idx].isLoading;
}


void CDocumentServiceController::setDocumentIsLoading(const QString& documentId, bool isLoading)
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


int CDocumentServiceController::getDocumentIndexByDocumentId(const QString& documentId) const
{
	return IndexOfDocument(documentId);
}


QString CDocumentServiceController::getDocumentIdByObjectId(const QString& objectId) const
{
	for (const FOpenedDocument& doc : m_openedDocuments){
		if (doc.objectId == objectId){
			return doc.id;
		}
	}
	return QString();
}


QString CDocumentServiceController::getDocumentIdByView(QObject* view) const
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


QString CDocumentServiceController::getDocumentTypeId(const QString& documentId) const
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return QString();
	}
	return m_openedDocuments[idx].typeId;
}


void CDocumentServiceController::setDocumentObjectId(const QString& documentId, const QString& objectId)
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return;
	}
	m_openedDocuments[idx].objectId = objectId;
}


void CDocumentServiceController::setDocumentName(const QString& documentId, const QString& name)
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


QString CDocumentServiceController::getDocumentName(const QString& documentId) const
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return QString();
	}
	return m_openedDocuments[idx].name;
}


QString CDocumentServiceController::getDefaultDocumentName() const
{
	return tr("<no name>");
}


// --- DocumentServiceBase: views ---

QObject* CDocumentServiceController::getDocumentViewInstance(
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


void CDocumentServiceController::onViewInstanceCreated(
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


QObject* CDocumentServiceController::getDocumentServiceActiveView() const
{
	return m_documentManagerActiveView.data();
}


void CDocumentServiceController::setDocumentServiceActiveView(QObject* view)
{
	if (m_documentManagerActiveView.data() == view){
		return;
	}
	m_documentManagerActiveView = view;
	Q_EMIT documentManagerActiveViewChanged();
}


void CDocumentServiceController::setDocumentDecorator(const QString& documentId, QObject* decorator)
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return;
	}
	m_openedDocuments[idx].documentDecorator = decorator;
}


QObject* CDocumentServiceController::getDocumentDecorator(const QString& documentId) const
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return nullptr;
	}
	return m_openedDocuments[idx].documentDecorator.data();
}


// --- DocumentServiceBase: name providers ---

void CDocumentServiceController::setAutoNamedTypeId(const QString& typeId, bool hasProvider)
{
	m_autoNamedTypeIds.insert(typeId, hasProvider);
}


bool CDocumentServiceController::hasDocumentNameProvider(const QString& typeId) const
{
	const auto it = m_autoNamedTypeIds.constFind(typeId);
	if (it == m_autoNamedTypeIds.constEnd()){
		return false;
	}
	return *it;
}


// --- private helpers ---

IDocumentServiceBridge* CDocumentServiceController::ResolveBridge() const
{
	return CDocumentServiceBridge::Instance();
}


void CDocumentServiceController::CreateDocumentDataInternal(
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


void CDocumentServiceController::RemoveDocumentDataInternal(const QString& documentId)
{
	int idx = IndexOfDocument(documentId);
	if (idx < 0){
		return;
	}
	m_openedDocuments.removeAt(idx);
	Q_EMIT openedDocumentIdsChanged();
}


int CDocumentServiceController::IndexOfDocument(const QString& documentId) const
{
	for (int i = 0; i < m_openedDocuments.size(); ++i){
		if (m_openedDocuments[i].id == documentId){
			return i;
		}
	}
	return -1;
}


// --- Transport (delegated to IDocumentServiceBridge) ---

namespace
{

QString NoBridgeError()
{
	return QStringLiteral("CDocumentServiceController: no IDocumentServiceBridge available "
			"(CDocumentServiceBridge component is not loaded)");
}

} // namespace


void CDocumentServiceController::getOpenedDocumentList()
{
	IDocumentServiceBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT openedDocumentListReceiveFailed(NoBridgeError());
		return;
	}

	Q_EMIT startGetOpenedDocumentList();

	QPointer<CDocumentServiceController> self(this);
	bridge->GetOpenedDocumentList(m_collectionId,
			[self](QList<IDocumentServiceBridge::OpenedDocumentInfo> list, QString errorMessage){
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


void CDocumentServiceController::openDocument(const QString& typeId, const QString& documentId)
{
	const QString existingDocumentId = getDocumentIdByObjectId(documentId);
	if (!existingDocumentId.isEmpty()){
		Q_EMIT documentAlreadyOpened(existingDocumentId, typeId);
		return;
	}

	IDocumentServiceBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT openDocumentFailed(documentId, NoBridgeError());
		return;
	}

	Q_EMIT startOpenDocument(documentId, typeId);

	QPointer<CDocumentServiceController> self(this);
	bridge->OpenDocument(m_collectionId, typeId, documentId,
			[self](IDocumentServiceBridge::OpenedDocumentInfo info, QString errorMessage){
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


void CDocumentServiceController::createDocument(const QString& typeId)
{
	IDocumentServiceBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT createDocumentFailed(typeId, NoBridgeError());
		return;
	}

	Q_EMIT startCreateDocument(typeId);

	QPointer<CDocumentServiceController> self(this);
	bridge->CreateDocument(m_collectionId, typeId,
			[self, typeId](IDocumentServiceBridge::OpenedDocumentInfo info, QString errorMessage){
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


void CDocumentServiceController::saveDocument(const QString& documentId, const QString& documentName)
{
	IDocumentServiceBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT saveDocumentFailed(documentId, NoBridgeError());
		return;
	}

	Q_EMIT startSaveDocument(documentId);

	const QString resolvedName = documentName.isEmpty() ? getDocumentName(documentId) : documentName;
	QPointer<CDocumentServiceController> self(this);
	bridge->SaveDocument(m_collectionId, documentId, resolvedName,
			[self, documentId, resolvedName](IDocumentServiceBridge::OperationStatus status, QString errorMessage){
				if (!self){
					return;
				}
				if (status == IDocumentServiceBridge::OS_OK && errorMessage.isEmpty()){
					if (!resolvedName.isEmpty()){
						self->setDocumentName(documentId, resolvedName);
					}
					Q_EMIT self->documentSaved(documentId);
					return;
				}
				const QString msg = errorMessage.isEmpty()
						? CDocumentServiceController::tr("Save document failed")
						: errorMessage;
				Q_EMIT self->saveDocumentFailed(documentId, msg);
			});
}


void CDocumentServiceController::closeDocument(const QString& documentId)
{
	IDocumentServiceBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT closeDocumentFailed(documentId, NoBridgeError());
		return;
	}

	Q_EMIT startCloseDocument(documentId);

	QPointer<CDocumentServiceController> self(this);
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


void CDocumentServiceController::doUndo(const QString& documentId, int steps)
{
	IDocumentServiceBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT undoFailed(documentId, NoBridgeError());
		return;
	}

	Q_EMIT startUndo(documentId, steps);

	QPointer<CDocumentServiceController> self(this);
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


void CDocumentServiceController::doRedo(const QString& documentId, int steps)
{
	IDocumentServiceBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT redoFailed(documentId, NoBridgeError());
		return;
	}

	Q_EMIT startRedo(documentId, steps);

	QPointer<CDocumentServiceController> self(this);
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


void CDocumentServiceController::resetUndo(const QString& documentId)
{
	IDocumentServiceBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT resetUndoFailed(documentId, NoBridgeError());
		return;
	}

	Q_EMIT startResetUndo(documentId);

	QPointer<CDocumentServiceController> self(this);
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


void CDocumentServiceController::getUndoInfo(const QString& documentId)
{
	IDocumentServiceBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT undoInfoReceiveFailed(documentId, NoBridgeError());
		return;
	}

	Q_EMIT startUndoInfoReceive(documentId);

	QPointer<CDocumentServiceController> self(this);
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
