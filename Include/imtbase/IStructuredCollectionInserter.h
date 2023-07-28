#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


class ICollectionNode;


class IStructuredCollectionInserter: virtual public istd::IChangeable
{
public:
	typedef QByteArray Id;
	typedef QVector<Id> Ids;

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
};


} // namespace imtbase


