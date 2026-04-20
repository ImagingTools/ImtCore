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


namespace imtqml
{


namespace docmgr = sdl::imtbase::CollectionDocumentManager;


namespace
{


template<class Fn>
void PostToMainThread(Fn&& fn)
{
	QCoreApplication* appPtr = QCoreApplication::instance();
	if (appPtr == nullptr){
		return;
	}
	QMetaObject::invokeMethod(appPtr, std::forward<Fn>(fn), Qt::QueuedConnection);
}


} // namespace


// public methods

CGqlDocumentDataController::CGqlDocumentDataController(QObject* parent)
	:BaseClass(parent)
{
}


CGqlDocumentDataController::~CGqlDocumentDataController() = default;


QObject* CGqlDocumentDataController::GetApiClient() const
{
	return m_apiClient;
}


void CGqlDocumentDataController::SetApiClient(QObject* apiClient)
{
	if (m_apiClient != apiClient){
		m_apiClient = apiClient;
		Q_EMIT apiClientChanged(m_apiClient);
	}
}


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


const QString& CGqlDocumentDataController::GetDocumentId() const
{
	return m_documentId;
}


void CGqlDocumentDataController::SetDocumentId(const QString& id)
{
	if (m_documentId != id){
		m_documentId = id;
		Q_EMIT documentIdChanged(m_documentId);
	}
}


const QString& CGqlDocumentDataController::GetDocumentName() const
{
	return m_documentName;
}


void CGqlDocumentDataController::SetDocumentName(const QString& name)
{
	if (m_documentName != name){
		m_documentName = name;
		Q_EMIT documentNameChanged(m_documentName);
	}
}


const QString& CGqlDocumentDataController::GetDocumentDescription() const
{
	return m_documentDescription;
}


void CGqlDocumentDataController::SetDocumentDescription(const QString& description)
{
	if (m_documentDescription != description){
		m_documentDescription = description;
		Q_EMIT documentDescriptionChanged(m_documentDescription);
	}
}


const QString& CGqlDocumentDataController::GetTypeId() const
{
	return m_typeId;
}


void CGqlDocumentDataController::SetTypeId(const QString& typeId)
{
	if (m_typeId != typeId){
		m_typeId = typeId;
		Q_EMIT typeIdChanged(m_typeId);
	}
}


bool CGqlDocumentDataController::GetHasRemoteChanges() const
{
	return m_hasRemoteChanges;
}


void CGqlDocumentDataController::SetHasRemoteChanges(bool value)
{
	if (m_hasRemoteChanges != value){
		m_hasRemoteChanges = value;
		Q_EMIT hasRemoteChangesChanged(m_hasRemoteChanges);
	}
}


const QVariant& CGqlDocumentDataController::GetDocumentModel() const
{
	return m_documentModel;
}


void CGqlDocumentDataController::SetDocumentModel(const QVariant& model)
{
	if (m_documentModel != model){
		m_documentModel = model;
		Q_EMIT documentModelChanged();
	}
}


// public slots

QString CGqlDocumentDataController::getDocumentId() const
{
	return m_documentId;
}


QString CGqlDocumentDataController::getDocumentName() const
{
	return m_documentName;
}


QString CGqlDocumentDataController::getDocumentTypeId() const
{
	return m_typeId;
}


QVariant CGqlDocumentDataController::getDocumentModel() const
{
	return m_documentModel;
}


QString CGqlDocumentDataController::getDocumentDescription() const
{
	return m_documentDescription;
}


// private methods

CGqlClientBridge* CGqlDocumentDataController::ResolveBridge() const
{
	if (m_apiClient != nullptr){
		auto* bridge = qobject_cast<CGqlClientBridge*>(m_apiClient);
		if (bridge != nullptr){
			return bridge;
		}
	}
	return CGqlClientBridge::Instance();
}


// public slots: GQL operations

void CGqlDocumentDataController::updateDocumentModel()
{
	CGqlClientBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT error(QStringLiteral("GQL client bridge is not available"), QStringLiteral("BridgeUnavailable"));
		return;
	}

	const QByteArray collectionId = m_collectionId.toUtf8();
	const QByteArray documentId = m_documentId.toUtf8();
	QPointer<CGqlDocumentDataController> self(this);

	QtConcurrent::run([self, bridge, collectionId, documentId]{
		docmgr::OpenDocumentRequestArguments arguments;
		arguments.input.Version_1_0.Emplace();
		arguments.input.Version_1_0->collectionId = collectionId;
		arguments.input.Version_1_0->id = documentId;

		docmgr::CDocumentInfo payload;
		QString errorMessage;
		bool ok = bridge->SendSdlRequest<
				docmgr::OpenDocumentRequestArguments,
				docmgr::CDocumentInfo,
				docmgr::COpenDocumentGqlRequest>(arguments, payload, errorMessage);

		if (!ok){
			const QString msg = errorMessage.isEmpty() ? QStringLiteral("Failed to open document") : errorMessage;
			PostToMainThread([self, msg]{
				if (self){
					Q_EMIT self->error(msg, QStringLiteral("OpenDocumentFailed"));
				}
			});
			return;
		}

		QString resolvedName;
		QString resolvedTypeId;
		if (payload.Version_1_0->documentName.HasValue()){
			resolvedName = *payload.Version_1_0->documentName;
		}
		if (payload.Version_1_0->objectTypeId.HasValue()){
			resolvedTypeId = QString::fromUtf8(*payload.Version_1_0->objectTypeId);
		}

		PostToMainThread([self, resolvedName, resolvedTypeId]{
			if (!self){
				return;
			}
			self->SetDocumentName(resolvedName);
			self->SetTypeId(resolvedTypeId);
			Q_EMIT self->modelChanged();
		});
	});
}


void CGqlDocumentDataController::insertDocument()
{
	CGqlClientBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT error(QStringLiteral("GQL client bridge is not available"), QStringLiteral("BridgeUnavailable"));
		return;
	}

	const QByteArray collectionId = m_collectionId.toUtf8();
	const QByteArray typeId = m_typeId.toUtf8();
	QPointer<CGqlDocumentDataController> self(this);

	QtConcurrent::run([self, bridge, collectionId, typeId]{
		docmgr::CreateNewDocumentRequestArguments arguments;
		arguments.input.Version_1_0.Emplace();
		arguments.input.Version_1_0->collectionId = collectionId;
		arguments.input.Version_1_0->typeId = typeId;

		docmgr::CDocumentInfo payload;
		QString errorMessage;
		bool ok = bridge->SendSdlRequest<
				docmgr::CreateNewDocumentRequestArguments,
				docmgr::CDocumentInfo,
				docmgr::CCreateNewDocumentGqlRequest>(arguments, payload, errorMessage);

		if (!ok || !payload.Version_1_0->documentId.HasValue()){
			const QString msg = errorMessage.isEmpty() ? QStringLiteral("Failed to create document") : errorMessage;
			PostToMainThread([self, msg]{
				if (self){
					Q_EMIT self->error(msg, QStringLiteral("CreateNewDocumentFailed"));
				}
			});
			return;
		}

		const QString newId = QString::fromUtf8(*payload.Version_1_0->documentId);
		QString newName;
		if (payload.Version_1_0->documentName.HasValue()){
			newName = *payload.Version_1_0->documentName;
		}

		PostToMainThread([self, newId, newName]{
			if (!self){
				return;
			}
			self->SetDocumentId(newId);
			if (!newName.isEmpty()){
				self->SetDocumentName(newName);
			}
			Q_EMIT self->saved(newId, newName.isEmpty() ? self->GetDocumentName() : newName);
		});
	});
}


void CGqlDocumentDataController::saveDocument()
{
	CGqlClientBridge* bridge = ResolveBridge();
	if (bridge == nullptr){
		Q_EMIT error(QStringLiteral("GQL client bridge is not available"), QStringLiteral("BridgeUnavailable"));
		return;
	}

	const QByteArray collectionId = m_collectionId.toUtf8();
	const QByteArray documentId = m_documentId.toUtf8();
	const QString documentIdStr = m_documentId;
	const QString documentName = m_documentName;
	QPointer<CGqlDocumentDataController> self(this);

	QtConcurrent::run([self, bridge, collectionId, documentId, documentIdStr, documentName]{
		docmgr::SaveDocumentRequestArguments arguments;
		arguments.input.Version_1_0.Emplace();
		arguments.input.Version_1_0->collectionId = collectionId;
		arguments.input.Version_1_0->documentId = documentId;
		arguments.input.Version_1_0->documentName = documentName;

		docmgr::CDocumentOperationStatus payload;
		QString errorMessage;
		bool ok = bridge->SendSdlRequest<
				docmgr::SaveDocumentRequestArguments,
				docmgr::CDocumentOperationStatus,
				docmgr::CSaveDocumentGqlRequest>(arguments, payload, errorMessage);

		if (!ok){
			const QString msg = errorMessage.isEmpty() ? QStringLiteral("Failed to save document") : errorMessage;
			PostToMainThread([self, msg]{
				if (self){
					Q_EMIT self->error(msg, QStringLiteral("SaveDocumentFailed"));
				}
			});
			return;
		}

		const bool isSuccess = payload.Version_1_0->status.HasValue()
				&& *payload.Version_1_0->status == docmgr::EDocumentOperationStatus::Success;
		if (!isSuccess){
			QString msg = QStringLiteral("Save document failed");
			if (payload.Version_1_0->message.HasValue()){
				msg = *payload.Version_1_0->message;
			}
			PostToMainThread([self, msg]{
				if (self){
					Q_EMIT self->error(msg, QStringLiteral("SaveDocumentFailed"));
				}
			});
			return;
		}

		QString resolvedName = documentName;
		if (payload.Version_1_0->documentName.HasValue()){
			resolvedName = *payload.Version_1_0->documentName;
		}

		PostToMainThread([self, documentIdStr, resolvedName]{
			if (self){
				Q_EMIT self->saved(documentIdStr, resolvedName);
			}
		});
	});
}


} // namespace imtqml
