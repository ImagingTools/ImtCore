// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtQml/QJSValue>


namespace imtqml
{


class IDocumentManagerBridge;


/**
	\brief Transport-agnostic C++ analog of the QML
	\c DocumentManagerBase / \c GqlBasedCollectionDocumentManager.

	\details
	The controller mirrors the public QML API of the legacy
	GraphQL-based document data controller (signals, properties and
	\c Q_INVOKABLE methods) but does NOT perform any GraphQL/SDL work
	itself. Every transport call is delegated to an
	\c IDocumentManagerBridge implementation resolved at runtime — by
	default to \c CDocumentManagerBridge (in-process, on top of
	\c imtdoc::IDocumentService).

	\note Bridge resolution currently uses
	\c CDocumentManagerBridge::Instance() to keep parity with the
	existing \c CGqlClientBridge lookup convention. The bridge is
	intentionally not exposed to QML; users just write
	\c DocumentManagerController \c { collectionId: "X" } and the QML
	code remains transport-agnostic.

	The class is registered to QML by \c CStaticQmlTypeRegistratorComp
	under \c com.imtcore.imtqml 1.0 as \c DocumentManagerController.
*/
class CDocumentManagerController: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString collectionId READ GetCollectionId WRITE SetCollectionId NOTIFY collectionIdChanged)
	Q_PROPERTY(QObject* documentManagerActiveView READ getDocumentManagerActiveView WRITE setDocumentManagerActiveView NOTIFY documentManagerActiveViewChanged)
	Q_PROPERTY(QStringList supportedDocumentTypeIds READ getSupportedDocumentTypeIds NOTIFY documentTypeEditorsChanged)
	Q_PROPERTY(QStringList openedDocumentIds READ getOpenedDocumentIds NOTIFY openedDocumentIdsChanged)

public:
	typedef QObject BaseClass;

	explicit CDocumentManagerController(QObject* parent = nullptr);
	~CDocumentManagerController() override;

	const QString& GetCollectionId() const;
	void SetCollectionId(const QString& id);

public Q_SLOTS:
	// --- DocumentManagerBase: registration / introspection ---

	void registerDocumentViewData(
			const QString& documentTypeId,
			const QString& viewTypeId,
			QObject* viewEditorComp,
			QObject* representationControllerComp);

	QObject* getDocumentEditorFactory(
			const QString& documentTypeId,
			const QString& viewTypeId = QString()) const;

	QObject* getDocumentRepresentationControllerFactory(
			const QString& documentTypeId,
			const QString& viewTypeId = QString()) const;

	QStringList getSupportedDocumentTypeIds() const;
	QStringList getSupportedDocumentViewTypeIds(const QString& documentTypeId) const;
	QString getViewTypeIdByViewFactory(
			const QString& documentTypeId,
			QObject* viewFactory) const;

	// --- DocumentManagerBase: opened-document state ---

	QStringList getOpenedDocumentIds() const;

	bool documentIsOpened(const QString& documentId) const;
	bool documentIsNew(const QString& documentId) const;
	void setDocumentIsNew(const QString& documentId, bool isNew);

	bool documentIsDirty(const QString& documentId) const;
	void setDocumentIsDirty(const QString& documentId, bool isDirty);

	bool documentIsLoading(const QString& documentId) const;
	void setDocumentIsLoading(const QString& documentId, bool isLoading);

	int getDocumentIndexByDocumentId(const QString& documentId) const;
	QString getDocumentIdByObjectId(const QString& objectId) const;
	QString getDocumentIdByView(QObject* view) const;
	QString getDocumentTypeId(const QString& documentId) const;

	void setDocumentObjectId(const QString& documentId, const QString& objectId);

	void setDocumentName(const QString& documentId, const QString& name);
	QString getDocumentName(const QString& documentId) const;

	QString getDefaultDocumentName() const;

	// --- DocumentManagerBase: views ---

	QObject* getDocumentViewInstance(
			const QString& documentId,
			const QString& viewTypeId = QString()) const;
	void onViewInstanceCreated(
			const QString& documentId,
			QObject* view,
			const QString& viewTypeId = QString());

	QObject* getDocumentManagerActiveView() const;
	void setDocumentManagerActiveView(QObject* view);

	void setDocumentDecorator(const QString& documentId, QObject* decorator);
	QObject* getDocumentDecorator(const QString& documentId) const;

	// --- DocumentManagerBase: name providers ---

	void setAutoNamedTypeId(const QString& typeId, bool hasProvider);
	bool hasDocumentNameProvider(const QString& typeId) const;

	// --- Transport (delegated to IDocumentManagerBridge) ---

	void getOpenedDocumentList();
	void openDocument(const QString& typeId, const QString& documentId);
	void createDocument(const QString& typeId);
	void saveDocument(const QString& documentId, const QString& documentName = QString());
	void closeDocument(const QString& documentId);
	void doUndo(const QString& documentId, int steps = 1);
	void doRedo(const QString& documentId, int steps = 1);
	void resetUndo(const QString& documentId);
	void getUndoInfo(const QString& documentId);

Q_SIGNALS:
	// --- Property NOTIFY ---
	void collectionIdChanged(const QString& collectionId);
	void documentManagerActiveViewChanged();
	void documentTypeEditorsChanged();
	void openedDocumentIdsChanged();

	// --- Mirrored from DocumentManagerBase ---
	void startGetOpenedDocumentList();
	void openedDocumentListReceived(const QVariant& documentListInfo);
	void openedDocumentListReceiveFailed(const QString& message);

	void startOpenDocument(const QString& documentId, const QString& typeId);
	void documentOpened(const QString& documentId, const QString& typeId);
	void openDocumentFailed(const QString& documentId, const QString& message);

	void startCloseDocument(const QString& documentId);
	void documentClosed(const QString& documentId);
	void closeDocumentFailed(const QString& documentId, const QString& message);

	void startSaveDocument(const QString& documentId);
	void saveDocumentFailed(const QString& documentId, const QString& message);
	void documentSaved(const QString& documentId);

	void startCreateDocument(const QString& typeId);
	void createDocumentFailed(const QString& typeId, const QString& message);
	void documentCreated(const QString& documentId, const QString& typeId);

	void startUndo(const QString& documentId, int steps);
	void undoDone(const QString& documentId);
	void undoFailed(const QString& documentId, const QString& message);

	void startRedo(const QString& documentId, int steps);
	void redoDone(const QString& documentId);
	void redoFailed(const QString& documentId, const QString& message);

	void startResetUndo(const QString& documentId);
	void resetUndoDone(const QString& documentId);
	void resetUndoFailed(const QString& documentId, const QString& message);

	void requestDocumentName(const QString& documentId, const QString& typeId);
	void documentNameChanged(const QString& documentId, const QString& oldName, const QString& newName);
	void documentIsDirtyChanged(const QString& documentId, bool isDirty);

	void startUndoInfoReceive(const QString& documentId);
	void undoInfoReceived(const QString& documentId, int availableUndoSteps, int availableRedoSteps, bool isDirty);
	void undoInfoReceiveFailed(const QString& documentId, const QString& message);

	void documentManagerChanged(
			const QString& typeOperation,
			const QString& objectId,
			const QString& documentId,
			const QString& documentName);
	void documentDataLoaded(const QString& documentId);
	void startUpdateRepresentation(const QString& documentId, const QVariant& representation);
	void documentRepresentationUpdated(const QString& documentId, const QVariant& representation);
	void documentGuiUpdated(const QString& documentId, const QVariant& representation);

	void tryCloseDirtyDocument(const QString& documentId, const QJSValue& callback);

	void documentViewRegistered(const QString& typeId, const QString& viewTypeId);

	void documentAlreadyOpened(const QString& documentId, const QString& typeId);

private:
	struct FViewEntry
	{
		QString viewTypeId;
		QPointer<QObject> viewEditorComp;
		QPointer<QObject> representationControllerComp;
	};

	struct FOpenedDocument
	{
		QString id;
		QString typeId;
		QString name;
		QString objectId;
		bool isDirty = false;
		bool isNew = true;
		bool isLoading = false;
		bool isClosing = false;
		QHash<QString, QPointer<QObject>> views;
		QPointer<QObject> documentDecorator;
	};

	IDocumentManagerBridge* ResolveBridge() const;

	void CreateDocumentDataInternal(const QString& id, const QString& typeId, bool isNew);
	void RemoveDocumentDataInternal(const QString& documentId);
	int  IndexOfDocument(const QString& documentId) const;

	QString m_collectionId;

	QList<FOpenedDocument>             m_openedDocuments;
	QHash<QString, QList<FViewEntry>>  m_documentTypeEditors;
	QHash<QString, QString>            m_cachedDocumentNames;
	QHash<QString, bool>               m_autoNamedTypeIds;
	QPointer<QObject>                  m_documentManagerActiveView;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CDocumentManagerController*)
