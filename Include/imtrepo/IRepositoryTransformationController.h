#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/TRange.h>


namespace imtrepo
{


class IFileObjectCollection;


/**
	\ingroup Collection
*/
class IRepositoryTransformationController: virtual public istd::IPolymorphic
{
public:
	/**
		Apply transformation to a repository. Transformation can be an update or an upgrade of the collection.
	*/
	virtual bool TransformRepository(IFileObjectCollection& repository, int fromRevision, int toRevision) const = 0;
};


} // namespace imtrepo


