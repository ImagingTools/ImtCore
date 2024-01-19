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
const QByteArray IHierarchicalStructureInfo::CN_LEAF_INSERTED = QByteArrayLiteral("imtbase:IHierarchicalStructureInfo::LeafInserted");
const QByteArray IHierarchicalStructureInfo::CN_LEAF_MOVED = QByteArrayLiteral("imtbase:IHierarchicalStructureInfo::LeafMoved");
const QByteArray IHierarchicalStructureInfo::CN_LEAF_REMOVED = QByteArrayLiteral("imtbase:IHierarchicalStructureInfo::LeafRemoved");


} // namespace imtbase


