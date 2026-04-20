// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>


namespace imtqml
{


class CGqlClientBridge;


/**
	C++ analog of the QML \c GqlDocumentDataController
	(see \c Qml/imtdocgui/GqlDocumentDataController.qml).

	\details
	The controller is a thin \c QObject facade over the SDL-generated
	request classes from \c Sdl/imtbase/1.0/CollectionDocumentManager.sdl
	(\c CCreateNewDocumentGqlRequest, \c COpenDocumentGqlRequest,
	\c CSaveDocumentGqlRequest). Every operation is dispatched
	asynchronously through \c QtConcurrent::run via
	\c CGqlClientBridge::SendSdlRequest(); resulting signals are always
	re-emitted on the GUI thread via \c QMetaObject::invokeMethod with
	\c Qt::QueuedConnection.

	A \c collectionId property is required because the
	\c CollectionDocumentManager schema scopes every operation to a
	collection.

	The class is registered to QML by \c CStaticQmlTypeRegistratorComp
	under \c com.imtcore.imtqml 1.0 as \c GqlDocumentDataController.
*/
class CGqlDocumentDataController: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QObject* apiClient READ GetApiClient WRITE SetApiClient NOTIFY apiClientChanged)
	Q_PROPERTY(QString collectionId READ GetCollectionId WRITE SetCollectionId NOTIFY collectionIdChanged)
	Q_PROPERTY(QString documentId READ GetDocumentId WRITE SetDocumentId NOTIFY documentIdChanged)
	Q_PROPERTY(QString documentName READ GetDocumentName WRITE SetDocumentName NOTIFY documentNameChanged)
	Q_PROPERTY(QString documentDescription READ GetDocumentDescription WRITE SetDocumentDescription NOTIFY documentDescriptionChanged)
	Q_PROPERTY(QString typeId READ GetTypeId WRITE SetTypeId NOTIFY typeIdChanged)
	Q_PROPERTY(bool hasRemoteChanges READ GetHasRemoteChanges WRITE SetHasRemoteChanges NOTIFY hasRemoteChangesChanged)
	Q_PROPERTY(QVariant documentModel READ GetDocumentModel WRITE SetDocumentModel NOTIFY documentModelChanged)

public:
	typedef QObject BaseClass;

	explicit CGqlDocumentDataController(QObject* parent = nullptr);
	~CGqlDocumentDataController() override;

	QObject* GetApiClient() const;
	void SetApiClient(QObject* apiClient);

	const QString& GetCollectionId() const;
	void SetCollectionId(const QString& id);

	const QString& GetDocumentId() const;
	void SetDocumentId(const QString& id);

	const QString& GetDocumentName() const;
	void SetDocumentName(const QString& name);

	const QString& GetDocumentDescription() const;
	void SetDocumentDescription(const QString& description);

	const QString& GetTypeId() const;
	void SetTypeId(const QString& typeId);

	bool GetHasRemoteChanges() const;
	void SetHasRemoteChanges(bool value);

	const QVariant& GetDocumentModel() const;
	void SetDocumentModel(const QVariant& model);

public Q_SLOTS:
	QString getDocumentId() const;
	QString getDocumentName() const;
	QString getDocumentTypeId() const;
	QVariant getDocumentModel() const;
	QString getDocumentDescription() const;

	void updateDocumentModel();
	void insertDocument();
	void saveDocument();

Q_SIGNALS:
	void apiClientChanged(QObject* apiClient);
	void collectionIdChanged(const QString& collectionId);
	void documentIdChanged(const QString& documentId);
	void documentNameChanged(const QString& documentName);
	void documentDescriptionChanged(const QString& documentDescription);
	void typeIdChanged(const QString& typeId);
	void hasRemoteChangesChanged(bool hasRemoteChanges);
	void documentModelChanged();

	void saved(const QString& id, const QString& name);
	void error(const QString& message, const QString& type);
	void modelChanged();

private:
	CGqlClientBridge* ResolveBridge() const;

	QObject* m_apiClient = nullptr;
	QString m_collectionId;
	QString m_documentId;
	QString m_documentName;
	QString m_documentDescription;
	QString m_typeId;
	bool m_hasRemoteChanges = false;
	QVariant m_documentModel;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CGqlDocumentDataController*)
