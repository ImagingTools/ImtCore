// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtqml/CDocumentDataController.h>


namespace imtqml
{


class CGqlRequest;


/**
	C++ analog of the QML \c GqlDocumentDataController
	(see \c Qml/imtdocgui/GqlDocumentDataController.qml and the related
	\c Qml/imtguigql/GqlRequestDocumentDataController.qml).

	The controller provides three GraphQL operations — \c get, \c add and
	\c update — for a single document identified by \c documentId. The
	GraphQL command identifiers are exposed as plain string properties so
	they can be set once from QML or from a C++ subclass.

	The class is registered to QML by \c CStaticQmlTypeRegistratorComp
	under \c com.imtcore.imtqml 1.0 as \c GqlDocumentDataController.
*/
class CGqlDocumentDataController: public CDocumentDataController
{
	Q_OBJECT

	Q_PROPERTY(QVariantMap headers READ GetHeaders WRITE SetHeaders NOTIFY headersChanged)

	Q_PROPERTY(QString gqlGetCommandId MEMBER m_gqlGetCommandId NOTIFY commandIdsChanged)
	Q_PROPERTY(QString gqlAddCommandId MEMBER m_gqlAddCommandId NOTIFY commandIdsChanged)
	Q_PROPERTY(QString gqlUpdateCommandId MEMBER m_gqlUpdateCommandId NOTIFY commandIdsChanged)

public:
	typedef CDocumentDataController BaseClass;

	explicit CGqlDocumentDataController(QObject* parent = nullptr);
	~CGqlDocumentDataController() override;

	const QVariantMap& GetHeaders() const;
	void SetHeaders(const QVariantMap& headers);

public Q_SLOTS:
	// reimplemented (CDocumentDataController)
	void updateDocumentModel() override;
	void insertDocument() override;
	void saveDocument() override;

Q_SIGNALS:
	void headersChanged(const QVariantMap& headers);
	void commandIdsChanged();

private:
	enum class OperationKind {
		Get,
		Add,
		Update,
	};

	void Dispatch(OperationKind kind, const QString& operation, const QString& commandId, const QVariantMap& inputArgs, const QString& selection);
	void HandleReplyState(OperationKind kind, const QString& commandId, CGqlRequest* request, const QString& state);

	QVariantMap m_headers;
	QString m_gqlGetCommandId;
	QString m_gqlAddCommandId;
	QString m_gqlUpdateCommandId;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CGqlDocumentDataController*)
