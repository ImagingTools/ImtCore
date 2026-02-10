// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QSharedPointer>

// ACF includes
#include <idoc/IDocumentMetaInfo.h>


namespace iprm
{
	class IParamsSet;
}


namespace imtbase
{

class IHierarchicalStructureIterator;


class IHierarchicalStructureInfo: virtual public istd::IChangeable
{
public:
	typedef QByteArray Id;
	typedef QByteArrayList Ids;

	struct NotifierInfo1
	{
		Id id;
	};

	struct NotifierInfo2
	{
		Id id;
		Id parentNodeId;
	};

	struct NotifierInfo3
	{
		Id id;
		Id sourceNodeId;
		Id targetNodeId;
	};

	typedef NotifierInfo2 NodeInsertInfo;
	typedef NotifierInfo1 NodeUpdateInfo;
	typedef NotifierInfo2 NodeMoveInfo;
	typedef NotifierInfo1 NodeRemoveInfo;
	typedef NotifierInfo2 LeafInsertInfo;
	typedef NotifierInfo3 LeafMoveInfo;
	typedef NotifierInfo2 LeafRemoveInfo;

	static const QByteArray CN_ALL_CHANGED;
	static const QByteArray CN_NODE_INSERTED;
	static const QByteArray CN_NODE_UPDATED;
	static const QByteArray CN_NODE_MOVED;
	static const QByteArray CN_NODE_REMOVED;
	static const QByteArray CN_LEAF_INSERTED;
	static const QByteArray CN_LEAF_MOVED;
	static const QByteArray CN_LEAF_REMOVED;

	enum ItemType
	{
		IT_NODE,
		IT_LEAF
	};

	struct ItemInfo
	{
		ItemType itemType = IT_NODE;
		Id itemId;
		Id parentId;
		QString name;
		QString description;
		QSharedPointer<idoc::IDocumentMetaInfo> metaInfo;
	};

	typedef QList<ItemInfo> ItemInfoList;

	virtual int GetItemCount(const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
	virtual Ids GetItemIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;

	virtual ItemInfoList GetItemInfos(const Ids& itemIds) = 0;
	virtual Ids GetItemPath(const Id& itemId) const = 0;

	/**
		Create a structure iterator according to the given filtering/sorting parameters.
		\param offset				[optional] Index offset of the first item
		\param count				[optional] If positive, the number of items should be returned.
		\param selectionParamsPtr	[optional] Additional parameters for filtering/ordering items.
	*/
	virtual imtbase::IHierarchicalStructureIterator* CreateHierarchicalStructureIterator(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
};


} // namespace imtbase

Q_DECLARE_METATYPE(imtbase::IHierarchicalStructureInfo::NotifierInfo2);
Q_DECLARE_METATYPE(imtbase::IHierarchicalStructureInfo::ItemInfoList);


