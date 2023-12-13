#pragma once


// ImtCore includes
#include <imtbase/ICollectionStructureInfo.h>


namespace imtbase
{


const QByteArray ICollectionStructureInfo::CN_ALL_CHANGED = QByteArrayLiteral("imtbase:ICollectionStructureInfo::AllChanged");
const QByteArray ICollectionStructureInfo::CN_NODE_INSERTED = QByteArrayLiteral("imtbase:ICollectionStructureInfo::NodeInserted");
const QByteArray ICollectionStructureInfo::CN_NODE_UPDATED = QByteArrayLiteral("imtbase:ICollectionStructureInfo::NodeUpdated");
const QByteArray ICollectionStructureInfo::CN_NODE_MOVED = QByteArrayLiteral("imtbase:ICollectionStructureInfo::NodeMoved");
const QByteArray ICollectionStructureInfo::CN_NODE_REMOVED = QByteArrayLiteral("imtbase:ICollectionStructureInfo::NodeRemoved");
const QByteArray ICollectionStructureInfo::CN_OBJECT_INSERTED = QByteArrayLiteral("imtbase:ICollectionStructureInfo::ObjectInserted");
const QByteArray ICollectionStructureInfo::CN_OBJECT_MOVED = QByteArrayLiteral("imtbase:ICollectionStructureInfo::ObjectMoved");
const QByteArray ICollectionStructureInfo::CN_OBJECT_REMOVED = QByteArrayLiteral("imtbase:ICollectionStructureInfo::ObjectRemoved");


} // namespace imtbase


