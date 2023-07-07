#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QVector>

// ImtCore includes
#include <istd/IChangeable.h>


namespace imtbase
{


class ICollectionNode;


/**
	Interface describing the structure of a object collection.
	\ingroup Collection
*/
class ICollectionStructure: virtual public istd::IChangeable
{
public:
	typedef QByteArray Id;
	typedef QVector<Id> Ids;

	struct NodeInfo
	{
		QString name;
		// istd::TSmartPtr<idoc::IDocumentMetaInfo> metaInfoPtr;
	};

	virtual QByteArray InsertNewNode(const QString& name, const Id& parentNodeId, const Id& proposedNodeId = QByteArray()) = 0;
	virtual ICollectionNode* GetNode(const Id& nodeId) const = 0;
	virtual bool MoveNode(const Id& nodeId, const Id& parentNodeId) = 0;
	virtual bool RemoveNode(const Id& nodeId) = 0;
	virtual NodeInfo GetNodeInfo(const Id& nodeId) const = 0;
	virtual bool SetNodeInfo(const Id& nodeId, const NodeInfo& nodeInfo) = 0;

	/**
		Add a collection object to the structure
		\param objectId	ID of the object in the data collection
		\param nodeId	ID of the node in the collection structure
	*/
	virtual bool AddObject(const Id& objectId, const Id& nodeId) = 0;
	virtual bool MoveObject(const Id& objectId, const Id& parentNodeId, const Id& newParentNodeId) = 0;
	virtual bool RemoveObjectFromStructure(const Id& objectId, const Id& nodeId) = 0;

	// TODO: Move to IStructuredCollectionInserter
	virtual bool InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				IObjectCollection::DataPtr defaultValuePtr = IObjectCollection::DataPtr(),
				const Id& proposedObjectId = Id(),
				const Id& parentId = Id(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
				const IOperationContext* operationContextPtr = nullptr) = 0;
	/**
		Get the list of nodes containing the given object.
	*/
	virtual Ids FindObjectNodes(const Id& objectId) const = 0;
};


} // namespace imtbase


