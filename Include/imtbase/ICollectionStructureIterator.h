#pragma once


// ImtCore includes
#include <imtbase/ICollectionStructureInfo.h>


namespace imtbase
{


/**
	Interface for an object collection iteration.
	\ingroup Collection
*/
class ICollectionStructureIterator: virtual public istd::IPolymorphic
{
public:
	/**
		Move iterator to the next position, if available.
	*/
	virtual bool Next() = 0;

	/**
		Move iterator to the previous position, if available.
	*/
	virtual bool Previous() = 0;

	/**
		Get node-ID at the current iterator position.
	*/
	virtual ICollectionStructureInfo::Id GetNodeId() const = 0;

	/**
		Get node info at the current iterator position.
	*/
	virtual ICollectionStructureInfo::NodeInfo GetNodeInfo() const = 0;
};


} // namespace imtbase


