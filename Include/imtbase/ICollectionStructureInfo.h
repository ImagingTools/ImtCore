#pragma once


// Qt includes
#include <QtCore/QSharedPointer>

// ACF includes
#include <istd/IChangeable.h>
#include <idoc/IDocumentMetaInfo.h>


namespace iprm
{
	class IParamsSet;
}


namespace imtbase
{

class ICollectionStructureIterator;


class ICollectionStructureInfo : virtual public istd::IChangeable
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
	virtual imtbase::ICollectionStructureIterator* CreateCollectionStructureIterator(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr) const = 0;

	virtual NodeInfo GetNodeInfo(const Id& nodeId) = 0;
	virtual Ids GetObjectParentNodeIds(const Id& objectId) const = 0;

	typedef QList<PathElement> NodePath;
	virtual NodePath GetNodePath(const Id& nodeId) const = 0;
};


} // namespace imtbase

Q_DECLARE_METATYPE(imtbase::ICollectionStructureInfo::NotifierInfo2);


