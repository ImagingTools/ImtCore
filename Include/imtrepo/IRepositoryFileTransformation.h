#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/TRange.h>


namespace imtrepo
{


class IRepositoryItemInfo;

/**
	Interface defining the transformation of a file inside of the repository structure during repository conversion procedure.
	\ingroup Collection
	\ingroup FileRepository
*/
class IRepositoryFileTransformation: public istd::IPolymorphic
{
public:
	/**
		Get the revision range supported by the file transformation.
		\note Please note that the upgrade range should be provided.
	*/
	virtual istd::CIntRange GetSupportedRevisionRange() const = 0;

	/**
		Execute the file transformation for the given revision range.
	*/
	virtual bool TransformFile(const IRepositoryItemInfo& repositoryIteminfo, int fromRevision, int toRevision) const = 0;
};


} // namespace imtrepo


