#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtbase
{


class IStructuredCollectionFinder : virtual public istd::IPolymorphic
{
public:
	virtual QByteArrayList FindObjectParentNodes(const QByteArray& objectId) const = 0;
};


} // namespace imtbase


