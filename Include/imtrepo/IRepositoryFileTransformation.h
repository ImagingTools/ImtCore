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
class IRepositoryFileTransformaton: public istd::IPolymorphic
{
public:
	virtual istd::CIntRange GetSupportedRevisionRange() const = 0;
	virtual bool TransformFile(const QString& filePath, int fromRevision, int toRevision) const = 0;
};


} // namespace imtrepo


