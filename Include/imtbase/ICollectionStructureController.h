#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


class IOperationContext;


class ICollectionStructureController: virtual public istd::IPolymorphic
{
public:
	typedef QByteArray Id;

	virtual Id InsertNewObjectIntoCollection(
			IObjectCollection*  objectCollectionPtr,
			const Id& nodeId,
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			IObjectCollection::DataPtr defaultValuePtr = IObjectCollection::DataPtr(),
			const Id& proposedElementId = Id(),
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
			const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
			const IOperationContext* operationContextPtr = nullptr) = 0;
};


} // namespace imtbase


