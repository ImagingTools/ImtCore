#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


class IStructuredCollectionInserter: virtual public istd::IChangeable
{
public:
	virtual QByteArray InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				IObjectCollection::DataPtr defaultValuePtr,
				const QByteArray& proposedObjectId = QByteArray(),
				const QByteArray& nodeId = QByteArray(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr = nullptr,
				const IOperationContext* operationContextPtr = nullptr) = 0;
};


} // namespace imtbase


