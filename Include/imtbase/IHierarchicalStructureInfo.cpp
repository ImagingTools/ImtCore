#pragma once


// ImtCore includes
#include <imtbase/IHierarchicalStructureInfo.h>


namespace imtbase
{


const QByteArray IHierarchicalStructureInfo::CN_ALL_CHANGED = QByteArrayLiteral("imtbase:IHierarchicalStructureInfo::AllChanged");
const QByteArray IHierarchicalStructureInfo::CN_NODE_INSERTED = QByteArrayLiteral("imtbase:IHierarchicalStructureInfo::NodeInserted");
const QByteArray IHierarchicalStructureInfo::CN_NODE_UPDATED = QByteArrayLiteral("imtbase:IHierarchicalStructureInfo::NodeUpdated");
const QByteArray IHierarchicalStructureInfo::CN_NODE_MOVED = QByteArrayLiteral("imtbase:IHierarchicalStructureInfo::NodeMoved");
const QByteArray IHierarchicalStructureInfo::CN_NODE_REMOVED = QByteArrayLiteral("imtbase:IHierarchicalStructureInfo::NodeRemoved");
const QByteArray IHierarchicalStructureInfo::CN_OBJECT_INSERTED = QByteArrayLiteral("imtbase:IHierarchicalStructureInfo::ObjectInserted");
const QByteArray IHierarchicalStructureInfo::CN_OBJECT_MOVED = QByteArrayLiteral("imtbase:IHierarchicalStructureInfo::ObjectMoved");
const QByteArray IHierarchicalStructureInfo::CN_OBJECT_REMOVED = QByteArrayLiteral("imtbase:IHierarchicalStructureInfo::ObjectRemoved");


} // namespace imtbase


