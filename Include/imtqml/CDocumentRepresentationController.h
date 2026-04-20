// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QString>
#include <QtCore/QVariant>


namespace imtqml
{


/**
	C++ analog of the QML \c DocumentRepresentationController
	(see \c Qml/imtdocgui/DocumentRepresentationController.qml).

	\details
	Subclassable base \c QObject that owns its \c representationModel
	and \c view and exposes the \c updateRepresentationFromDocument /
	\c updateDocumentFromRepresentation entry points.

	The base implementation mirrors the QML behaviour exactly: each
	method first emits the matching \c start* signal, then warns
	(\c qWarning) that it should be implemented in a subclass, and
	finally emits the success signal. Concrete subclasses (for example
	\c CContactInfoRepresentationController) override the methods,
	perform the actual SDL transport (typically asynchronously through
	\c QtConcurrent::run + \c CGqlClientBridge) and emit the
	\c representationUpdated / \c documentUpdated signals once done.

	An internal monitor reproduces the QML \c signalMonitor: it logs an
	error (\c qCritical) if a \c *Updated signal is observed without a
	preceding \c start* signal.

	The class is registered to QML by \c CStaticQmlTypeRegistratorComp
	under \c com.imtcore.imtqml 1.0 as
	\c DocumentRepresentationController. To create a concrete
	controller from QML, derive a C++ class and register it with
	\c qmlRegisterType.

	\sa CGqlClientBridge — common bridge that also keeps a registry of
	    representation controllers keyed by document id.
*/
class CDocumentRepresentationController: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString documentId READ GetDocumentId WRITE SetDocumentId NOTIFY documentIdChanged)
	Q_PROPERTY(QVariant representationModel READ GetRepresentationModel WRITE SetRepresentationModel NOTIFY representationModelChanged)
	Q_PROPERTY(QObject* view READ GetView WRITE SetView NOTIFY viewChanged)

public:
	typedef QObject BaseClass;

	explicit CDocumentRepresentationController(QObject* parent = nullptr);
	~CDocumentRepresentationController() override;

	const QString& GetDocumentId() const;
	void SetDocumentId(const QString& documentId);

	const QVariant& GetRepresentationModel() const;
	void SetRepresentationModel(const QVariant& representationModel);

	QObject* GetView() const;
	void SetView(QObject* view);

public Q_SLOTS:
	/**
		\brief Recompute the representation from the underlying
		document. Subclasses MUST reimplement to perform the actual
		work. The base implementation only emits the matching
		signals and logs a warning.
	*/
	virtual void updateRepresentationFromDocument();

	/**
		\brief Push the current representation back to the underlying
		document. Subclasses MUST reimplement to perform the actual
		work. The base implementation only emits the matching
		signals and logs a warning.
	*/
	virtual void updateDocumentFromRepresentation();

Q_SIGNALS:
	void documentIdChanged(const QString& documentId);
	void representationModelChanged(const QVariant& representationModel);
	void viewChanged(QObject* view);

	void startUpdateDocument(const QString& documentId);
	void updateDocumentFailed(const QString& documentId, const QString& message);
	void documentUpdated(const QString& documentId);

	void startUpdateRepresentation(const QString& documentId, const QVariant& representation);
	void representationUpdated(const QString& documentId, const QVariant& representation);
	void updateRepresentationFailed(const QString& documentId, const QString& message);

private Q_SLOTS:
	void OnStartUpdateRepresentation(const QString& documentId, const QVariant& representation);
	void OnRepresentationUpdated(const QString& documentId, const QVariant& representation);
	void OnUpdateRepresentationFailed(const QString& documentId, const QString& message);
	void OnStartUpdateDocument(const QString& documentId);
	void OnDocumentUpdated(const QString& documentId);
	void OnUpdateDocumentFailed(const QString& documentId, const QString& message);

private:
	QString m_documentId;
	QVariant m_representationModel;
	QPointer<QObject> m_view;

	bool m_isRepresentationUpdatePending = false;
	bool m_isDocumentUpdatePending = false;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CDocumentRepresentationController*)
