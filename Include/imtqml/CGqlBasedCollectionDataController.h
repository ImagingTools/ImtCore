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
	C++ analog of the QML \c GqlBasedCollectionDataController
	(see \c Qml/imtguigql/GqlBasedCollectionDataController.qml).

	\details
	The controller is a thin \c QObject facade over the SDL-generated
	request classes from \c Sdl/imtbase/1.0/ImtCollection.sdl. Every
	operation is dispatched asynchronously through \c QtConcurrent::run
	using \c CGqlClientBridge::SendSdlRequest(); resulting signals are
	always re-emitted on the GUI thread via \c QMetaObject::invokeMethod
	with \c Qt::QueuedConnection.

	\note Operations whose SDL input requires a complex \c ParamsSet
	(\c removeObjectSet, \c restoreObjectSet, \c createSubCollection)
	emit the matching failure signal — they remain part of the QML API
	for source compatibility but are not yet supported by the SDL
	bridge.

	The class is registered to QML by \c CStaticQmlTypeRegistratorComp
	under \c com.imtcore.imtqml 1.0 as \c GqlBasedCollectionDataController.
*/
class CGqlBasedCollectionDataController: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString collectionId READ GetCollectionId WRITE SetCollectionId NOTIFY collectionIdChanged)

public:
	typedef QObject BaseClass;

	explicit CGqlBasedCollectionDataController(QObject* parent = nullptr);
	~CGqlBasedCollectionDataController() override;

	const QString& GetCollectionId() const;
	void SetCollectionId(const QString& id);

public Q_SLOTS:
	void getCollectionHeaders();
	void getObjectData(const QString& objectId);
	void insertNewObject(
				const QString& typeId,
				const QString& name,
				const QString& description,
				const QVariant& objectData = QVariant(),
				const QString& objectId = QString());
	void setObjectData(const QString& objectId, const QVariant& newObjectData);
	void removeObjects(const QVariant& objectIds);
	void removeObjectSet(const QVariant& selectionParams);
	void restoreObjects(const QVariant& objectIds);
	void restoreObjectSet(const QVariant& selectionParams);
	void setObjectName(const QString& objectId, const QString& name);
	void setObjectDescription(const QString& objectId, const QString& description);
	void createSubCollection(int offset, int count, const QVariant& selectionParams);

Q_SIGNALS:
	void collectionIdChanged(const QString& collectionId);

	void collectionHeadersReceived(const QVariant& headersModel);
	void collectionHeadersReceiveFailed(const QString& message);

	void objectReceived(const QVariant& objectData);
	void objectReceiveFailed(const QString& message);

	void objectAdded(const QString& id);
	void objectAddFailed(const QString& message);

	void objectUpdated(const QString& id);
	void objectUpdateFailed(const QString& message);

	void objectsRemoved(const QVariant& ids);
	void removeObjectsFailed(const QString& message);

	void objectsRestored(const QVariant& ids);
	void restoreObjectsFailed(const QString& message);

	void objectRenamed(const QString& id, const QString& newName);
	void objectRenameFailed(const QString& message);

	void objectDescriptionSetted(const QString& id, const QString& description);
	void objectSetDescriptionFailed(const QString& message);

	void subCollectionCreated(const QVariant& subCollection);
	void subCollectionCreateFailed(const QString& message);

private:
	CGqlClientBridge* ResolveBridge() const;

	QString m_collectionId;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CGqlBasedCollectionDataController*)
