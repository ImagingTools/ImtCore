// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>


namespace imtqml
{


/**
	C++ analog of the QML \c DocumentDataController
	(see \c Qml/imtdocgui/DocumentDataController.qml).

	The class exposes the same property, signal and \c Q_INVOKABLE method
	shape as the QML original. Default \c Q_INVOKABLE implementations are
	non-functional placeholders that emit \c saved with the current
	identifier — mirroring the behaviour of the QML base class.

	The class is registered to QML by \c CStaticQmlTypeRegistratorComp under
	\c com.imtcore.imtqml 1.0 as \c DocumentDataControllerBase.
*/
class CDocumentDataController: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString documentId READ GetDocumentId WRITE SetDocumentId NOTIFY documentIdChanged)
	Q_PROPERTY(QString documentName READ GetDocumentName WRITE SetDocumentName NOTIFY documentNameChanged)
	Q_PROPERTY(QString documentDescription READ GetDocumentDescription WRITE SetDocumentDescription NOTIFY documentDescriptionChanged)
	Q_PROPERTY(QString typeId READ GetTypeId WRITE SetTypeId NOTIFY typeIdChanged)
	Q_PROPERTY(bool hasRemoteChanges READ GetHasRemoteChanges WRITE SetHasRemoteChanges NOTIFY hasRemoteChangesChanged)
	Q_PROPERTY(QVariant documentModel READ GetDocumentModel WRITE SetDocumentModel NOTIFY documentModelChanged)

public:
	typedef QObject BaseClass;

	explicit CDocumentDataController(QObject* parent = nullptr);
	~CDocumentDataController() override;

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
	virtual QString getDocumentId() const;
	virtual QString getDocumentName() const;
	virtual QString getDocumentTypeId() const;
	virtual QVariant getDocumentModel() const;
	virtual QString getDocumentDescription() const;

	virtual void updateDocumentModel();
	virtual void insertDocument();
	virtual void saveDocument();

Q_SIGNALS:
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
	QString m_documentId;
	QString m_documentName;
	QString m_documentDescription;
	QString m_typeId;
	bool m_hasRemoteChanges = false;
	QVariant m_documentModel;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CDocumentDataController*)
