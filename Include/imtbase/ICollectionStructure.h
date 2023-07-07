#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QVector>

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


class ICollectionNode;


/**
	Interface for non-iterable colllection info.
	\ingroup Collection
*/
class ICollectionStructure: virtual public istd::IPolymorphic
{
public:
	typedef QByteArray Id;
	typedef QVector<Id> Ids;

	/*
		Node operations.
	*/
	virtual ICollectionNode* CreateNode(const Id& nodeId, const Id& parentId) = 0;
	virtual ICollectionNode* GetNode(const Id& nodeId) const = 0;
	virtual bool MoveNode(const Id& nodeId, const Id& newParentId) = 0;
	virtual bool RemoveNode(const Id& nodeId) = 0;

	/*
		Object operations
	*/
	virtual bool InsertObject(const Id& objectId, const Id& parentId) = 0;
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
	virtual bool MoveObject(const Id& objectId, const Id& parentId, const Id& newParentId) = 0;
	virtual bool RemoveObjectFromStructure(const Id& objectId, const Id& parentId) = 0;

	/*
		Find operations
	*/
	virtual Ids FindObjectNodes(const Id& objectId) const = 0;
};


} // namespace imtbase


