// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CCollectionDataController.h>


// Qt includes
#include <QtCore/QDebug>


namespace imtqml
{


CCollectionDataController::CCollectionDataController(QObject* parent)
	: BaseClass(parent)
{
}


CCollectionDataController::~CCollectionDataController() = default;


const QString& CCollectionDataController::GetCollectionId() const
{
	return m_collectionId;
}


void CCollectionDataController::SetCollectionId(const QString& id)
{
	if (m_collectionId != id) {
		m_collectionId = id;
		Q_EMIT collectionIdChanged(m_collectionId);
	}
}


void CCollectionDataController::getCollectionHeaders()
{
	qWarning() << "CCollectionDataController::getCollectionHeaders() should be implemented in a subclass";
	Q_EMIT collectionHeadersReceived(QVariant());
}


void CCollectionDataController::getObjectData(const QString& /*objectId*/)
{
	qWarning() << "CCollectionDataController::getObjectData() should be implemented in a subclass";
	Q_EMIT objectReceived(QVariant());
}


void CCollectionDataController::insertNewObject(
			const QString& /*typeId*/,
			const QString& /*name*/,
			const QString& /*description*/,
			const QVariant& /*objectData*/,
			const QString& objectId)
{
	qWarning() << "CCollectionDataController::insertNewObject() should be implemented in a subclass";
	Q_EMIT objectAdded(objectId);
}


void CCollectionDataController::setObjectData(const QString& objectId, const QVariant& /*newObjectData*/)
{
	qWarning() << "CCollectionDataController::setObjectData() should be implemented in a subclass";
	Q_EMIT objectUpdated(objectId);
}


void CCollectionDataController::getElements(int /*count*/, int /*offset*/, const QVariant& /*filter*/)
{
	qWarning() << "CCollectionDataController::getElements() should be implemented in a subclass";
	Q_EMIT listObjectsReceived(QVariant());
}


void CCollectionDataController::removeObjects(const QVariant& objectIds)
{
	qWarning() << "CCollectionDataController::removeObjects() should be implemented in a subclass";
	Q_EMIT objectsRemoved(objectIds);
}


void CCollectionDataController::removeObjectSet(const QVariant& /*selectionParams*/)
{
	qWarning() << "CCollectionDataController::removeObjectSet() should be implemented in a subclass";
	Q_EMIT objectsRemoved(QVariantList());
}


void CCollectionDataController::restoreObjects(const QVariant& objectIds)
{
	qWarning() << "CCollectionDataController::restoreObjects() should be implemented in a subclass";
	Q_EMIT objectsRestored(objectIds);
}


void CCollectionDataController::restoreObjectSet(const QVariant& /*selectionParams*/)
{
	qWarning() << "CCollectionDataController::restoreObjectSet() should be implemented in a subclass";
	Q_EMIT objectsRestored(QVariantList());
}


void CCollectionDataController::setObjectName(const QString& objectId, const QString& name)
{
	qWarning() << "CCollectionDataController::setObjectName() should be implemented in a subclass";
	Q_EMIT objectRenamed(objectId, name);
}


void CCollectionDataController::setObjectDescription(const QString& objectId, const QString& description)
{
	qWarning() << "CCollectionDataController::setObjectDescription() should be implemented in a subclass";
	Q_EMIT objectDescriptionSetted(objectId, description);
}


void CCollectionDataController::createSubCollection(int /*offset*/, int /*count*/, const QVariant& /*selectionParams*/)
{
	qWarning() << "CCollectionDataController::createSubCollection() should be implemented in a subclass";
	Q_EMIT subCollectionCreated(QVariant());
}


} // namespace imtqml
