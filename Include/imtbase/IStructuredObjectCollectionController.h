#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


class IOperationContext;
class IObjectCollection;


class IStructuredObjectCollectionController: virtual public istd::IChangeable
{
public:
	typedef QByteArray Id;

	virtual Id InsertNewObjectIntoCollection(
				IObjectCollection* structurePtr,
				const Id& nodeId,
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable* defaultValuePtr = nullptr,
				const Id& proposedElementId = Id(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* elementMetaInfoPtr = nullptr,
				const IOperationContext* operationContextPtr = nullptr) = 0;
};


} // namespace imtbase


