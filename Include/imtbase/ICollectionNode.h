#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtbase/TIIdBasedHierarchical.h>


namespace imtbase
{


class ICollectionInfo;


/**
	Interface for non-iterable colllection info.
	\ingroup Collection
*/
class ICollectionNode: virtual public istd::IChangeable
{
public:
	/**
		Get contents of the node.
	*/
	virtual const ICollectionInfo& GetObjectsList() const = 0;
};


typedef TIIdBasedHierarchical<ICollectionNode> IHierarchicalCollectionNode;


} // namespace imtbase


