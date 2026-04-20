// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtqml/CCollectionDataController.h>


namespace imtqml
{


class CGqlRequest;


/**
	C++ analog of the QML \c GqlBasedCollectionDataController
	(see \c Qml/imtguigql/GqlBasedCollectionDataController.qml).

	Each operation is dispatched as a GraphQL request through an internal
	\c imtqml::CGqlRequest instance. The GraphQL command identifiers used
	by the QML version come from \c ImtbaseImtCollectionSdlCommandIds.s_*
	at runtime; here they are exposed as plain string properties so that
	subclasses or QML callers can configure them once.

	Response payloads are emitted as \c QVariant maps via the inherited
	signals defined in \c CCollectionDataController.

	The class is registered to QML by \c CStaticQmlTypeRegistratorComp
	under \c com.imtcore.imtqml 1.0 as \c GqlBasedCollectionDataController.
*/
class CGqlBasedCollectionDataController: public CCollectionDataController
{
	Q_OBJECT

	Q_PROPERTY(QVariantMap headers READ GetHeaders WRITE SetHeaders NOTIFY headersChanged)

	Q_PROPERTY(QString getCollectionHeadersCommandId MEMBER m_getCollectionHeadersCommandId NOTIFY getCollectionHeadersCommandIdChanged)
	Q_PROPERTY(QString getObjectDataCommandId MEMBER m_getObjectDataCommandId NOTIFY getObjectDataCommandIdChanged)
	Q_PROPERTY(QString insertNewObjectCommandId MEMBER m_insertNewObjectCommandId NOTIFY insertNewObjectCommandIdChanged)
	Q_PROPERTY(QString setObjectDataCommandId MEMBER m_setObjectDataCommandId NOTIFY setObjectDataCommandIdChanged)
	Q_PROPERTY(QString removeElementsCommandId MEMBER m_removeElementsCommandId NOTIFY removeElementsCommandIdChanged)
	Q_PROPERTY(QString removeElementSetCommandId MEMBER m_removeElementSetCommandId NOTIFY removeElementSetCommandIdChanged)
	Q_PROPERTY(QString restoreObjectsCommandId MEMBER m_restoreObjectsCommandId NOTIFY restoreObjectsCommandIdChanged)
	Q_PROPERTY(QString restoreObjectSetCommandId MEMBER m_restoreObjectSetCommandId NOTIFY restoreObjectSetCommandIdChanged)
	Q_PROPERTY(QString setObjectNameCommandId MEMBER m_setObjectNameCommandId NOTIFY setObjectNameCommandIdChanged)
	Q_PROPERTY(QString setObjectDescriptionCommandId MEMBER m_setObjectDescriptionCommandId NOTIFY setObjectDescriptionCommandIdChanged)
	Q_PROPERTY(QString createSubCollectionCommandId MEMBER m_createSubCollectionCommandId NOTIFY createSubCollectionCommandIdChanged)

public:
	typedef CCollectionDataController BaseClass;

	explicit CGqlBasedCollectionDataController(QObject* parent = nullptr);
	~CGqlBasedCollectionDataController() override;

	const QVariantMap& GetHeaders() const;
	void SetHeaders(const QVariantMap& headers);

public Q_SLOTS:
	// reimplemented (CCollectionDataController)
	void getCollectionHeaders() override;
	void getObjectData(const QString& objectId) override;
	void insertNewObject(
				const QString& typeId,
				const QString& name,
				const QString& description,
				const QVariant& objectData = QVariant(),
				const QString& objectId = QString()) override;
	void setObjectData(const QString& objectId, const QVariant& newObjectData) override;
	void removeObjects(const QVariant& objectIds) override;
	void removeObjectSet(const QVariant& selectionParams) override;
	void restoreObjects(const QVariant& objectIds) override;
	void restoreObjectSet(const QVariant& selectionParams) override;
	void setObjectName(const QString& objectId, const QString& name) override;
	void setObjectDescription(const QString& objectId, const QString& description) override;
	void createSubCollection(int offset, int count, const QVariant& selectionParams) override;

Q_SIGNALS:
	void headersChanged(const QVariantMap& headers);
	void getCollectionHeadersCommandIdChanged(const QString& commandId);
	void getObjectDataCommandIdChanged(const QString& commandId);
	void insertNewObjectCommandIdChanged(const QString& commandId);
	void setObjectDataCommandIdChanged(const QString& commandId);
	void removeElementsCommandIdChanged(const QString& commandId);
	void removeElementSetCommandIdChanged(const QString& commandId);
	void restoreObjectsCommandIdChanged(const QString& commandId);
	void restoreObjectSetCommandIdChanged(const QString& commandId);
	void setObjectNameCommandIdChanged(const QString& commandId);
	void setObjectDescriptionCommandIdChanged(const QString& commandId);
	void createSubCollectionCommandIdChanged(const QString& commandId);

private:
	enum class OperationKind {
		GetCollectionHeaders,
		GetObjectData,
		InsertNewObject,
		SetObjectData,
		RemoveElements,
		RemoveElementSet,
		RestoreObjects,
		RestoreObjectSet,
		SetObjectName,
		SetObjectDescription,
		CreateSubCollection,
	};

	struct PendingOperation {
		OperationKind kind;
		CGqlRequest* request;
		QString commandId;
		// Context preserved for use during result handling, where applicable.
		QString objectId;
		QString name;
		QString description;
		QVariant objectIds;
	};

	void Dispatch(
				OperationKind kind,
				const QString& operation,
				const QString& commandId,
				const QVariantMap& inputArgs,
				const QString& selection,
				PendingOperation context);
	void HandleReplyState(const QString& state, PendingOperation context);
	void EmitFailure(OperationKind kind, const QString& message);

	QVariantMap m_headers;

	QString m_getCollectionHeadersCommandId;
	QString m_getObjectDataCommandId;
	QString m_insertNewObjectCommandId;
	QString m_setObjectDataCommandId;
	QString m_removeElementsCommandId;
	QString m_removeElementSetCommandId;
	QString m_restoreObjectsCommandId;
	QString m_restoreObjectSetCommandId;
	QString m_setObjectNameCommandId;
	QString m_setObjectDescriptionCommandId;
	QString m_createSubCollectionCommandId;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CGqlBasedCollectionDataController*)
