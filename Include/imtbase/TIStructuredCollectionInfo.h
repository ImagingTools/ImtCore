#pragma once


// ImtCore includes
#include <imtbase/IObjectCollectionInfo.h>


namespace idoc
{
	class IDocumentMetaInfo;
}


namespace imtbase
{


template <class BaseInterface>
class TIStructuredCollectionInfo : virtual public BaseInterface
{
public:
	enum ElementType
	{
		ET_NODE,
		ET_ELEMENT
	};

	virtual ElementType GetElementType(const QByteArray& elementId) const = 0;
	virtual QByteArrayList GetBasePath() const = 0;
};

typedef TIStructuredCollectionInfo<imtbase::ICollectionInfo> IStructuredCollectionInfo;
typedef TIStructuredCollectionInfo<imtbase::IObjectCollectionInfo> IStructuredObjectCollectionInfo;


} // namespace imtbase


