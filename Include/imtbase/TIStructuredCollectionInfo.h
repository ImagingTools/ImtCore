#pragma once


// ImtCore includes
#include <imtbase/IObjectCollectionInfo.h>


namespace idoc
{
	class IDocumentMetaInfo;
}


namespace imtbase
{


class IOperationContext;


template <class BaseInterface>
class TIStructuredCollectionInfo : virtual public BaseInterface
{
public:
	enum ElementType
	{
		ET_UNKNOWN,
		ET_NODE,
		ET_OBJECT
	};

	virtual ElementType GetElementType(const QByteArray& elementId) const = 0;
	virtual QByteArrayList GetElementBasePath(const QByteArray& elementId) const = 0;
};

typedef TIStructuredCollectionInfo<imtbase::ICollectionInfo> IStructuredCollectionInfo;
typedef TIStructuredCollectionInfo<imtbase::IObjectCollectionInfo> IStructuredObjectCollectionInfo;


} // namespace imtbase


