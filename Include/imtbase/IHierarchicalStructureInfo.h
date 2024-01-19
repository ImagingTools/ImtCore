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


class IHierarchicalStructureInfo : virtual public istd::IChangeable
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
	typedef NotifierInfo2 ObjectInsertInfo;
	typedef NotifierInfo3 ObjectMoveInfo;
	typedef NotifierInfo2 ObjectRemoveInfo;

	static const QByteArray CN_ALL_CHANGED;
	static const QByteArray CN_NODE_INSERTED;
	static const QByteArray CN_NODE_UPDATED;
	static const QByteArray CN_NODE_MOVED;
	static const QByteArray CN_NODE_REMOVED;
	static const QByteArray CN_OBJECT_INSERTED;
	static const QByteArray CN_OBJECT_MOVED;
	static const QByteArray CN_OBJECT_REMOVED;

	struct PathElement
	{
		Id id;
		QString name;
	};

	struct NodeInfo: public PathElement
	{
		QString description;
		QSharedPointer<idoc::IDocumentMetaInfo> metaInfo;
		Ids childNodeIds;
	};

	struct Node
	{
		QString name;
		QString description;
		QSharedPointer<idoc::IDocumentMetaInfo> metaInfo;
		Id parentId;
		Ids childs;
	};

	struct Leaf
	{
		QString name;
		Id parentId;
		Id itemId;
	};

	typedef QList<PathElement> NodePath;

	virtual int GetNodeCount(const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
	virtual Ids GetNodeIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
	/**
		Create a structure iterator according to the given filtering/sorting parameters.
		\param offset				[optional] Index offset of the first node
		\param count				[optional] If positive, the number of nodes should be returned.
		\param selectionParamsPtr	[optional] Additional parameters for filtering/ordering nodes.
	*/
	virtual imtbase::IHierarchicalStructureIterator* CreateCollectionStructureIterator(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;
	virtual NodeInfo GetNodeInfo(const Id& nodeId) = 0;
	virtual NodePath GetNodePath(const Id& nodeId) const = 0;
};


} // namespace imtbase

Q_DECLARE_METATYPE(imtbase::IHierarchicalStructureInfo::NotifierInfo2);


