// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>


namespace imtqml
{


/**
	C++ analog of the QML \c CollectionDataController (see
	\c Qml/imtcolgui/CollectionDataController.qml).

	This is a non-functional base class that exposes the same property,
	signal and \c Q_INVOKABLE method shape as the QML original. Subclasses
	(in C++ or QML) are expected to override the slots and emit the
	appropriate signals on completion.

	The class is registered to QML by \c CStaticQmlTypeRegistratorComp under
	\c com.imtcore.imtqml 1.0 as \c CollectionDataControllerBase, allowing
	it to be used as a drop-in replacement for the QML base class while
	keeping the implementation in C++.
*/
class CCollectionDataController: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString collectionId READ GetCollectionId WRITE SetCollectionId NOTIFY collectionIdChanged)

public:
	typedef QObject BaseClass;

	explicit CCollectionDataController(QObject* parent = nullptr);
	~CCollectionDataController() override;

	const QString& GetCollectionId() const;
	void SetCollectionId(const QString& id);

public Q_SLOTS:
	// Default implementations log a warning and emit the corresponding "no-op"
	// notification, mirroring the behaviour of the QML base class.
	virtual void getCollectionHeaders();
	virtual void getObjectData(const QString& objectId);
	virtual void insertNewObject(
				const QString& typeId,
				const QString& name,
				const QString& description,
				const QVariant& objectData = QVariant(),
				const QString& objectId = QString());
	virtual void setObjectData(const QString& objectId, const QVariant& newObjectData);
	virtual void getElements(int count, int offset, const QVariant& filter);
	virtual void removeObjects(const QVariant& objectIds);
	virtual void removeObjectSet(const QVariant& selectionParams);
	virtual void restoreObjects(const QVariant& objectIds);
	virtual void restoreObjectSet(const QVariant& selectionParams);
	virtual void setObjectName(const QString& objectId, const QString& name);
	virtual void setObjectDescription(const QString& objectId, const QString& description);
	virtual void createSubCollection(int offset, int count, const QVariant& selectionParams);

Q_SIGNALS:
	void collectionIdChanged(const QString& collectionId);

	void collectionHeadersReceived(const QVariant& headersModel);
	void collectionHeadersReceiveFailed(const QVariant& headersModel);

	void objectReceived(const QVariant& object);
	void objectReceiveFailed(const QString& message);

	void objectAdded(const QString& id);
	void objectAddFailed(const QString& message);

	void objectUpdated(const QString& id);
	void objectUpdateFailed(const QString& message);

	void objectsRemoved(const QVariant& ids);
	void removeObjectsFailed(const QString& message);

	void objectsRestored(const QVariant& ids);
	void restoreObjectsFailed(const QString& message);

	void listObjectsReceived(const QVariant& listObjects);
	void listObjectsReceiveFailed(const QString& message);

	void objectRenamed(const QString& id, const QString& newName);
	void objectRenameFailed(const QString& message);

	void objectDescriptionSetted(const QString& id, const QString& description);
	void objectSetDescriptionFailed(const QString& message);

	void subCollectionCreated(const QVariant& subCollection);
	void subCollectionCreateFailed(const QString& message);

private:
	QString m_collectionId;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CCollectionDataController*)
