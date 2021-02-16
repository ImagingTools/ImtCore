#include <imtrepo/CRepositoryFileTransformationCompBase.h>


namespace imtrepo
{


// public methods

// reimplemented (IRepositoryFileTransformation)

istd::CIntRange CRepositoryFileTransformationCompBase::GetSupportedRevisionRange() const
{
	return istd::CIntRange(*m_minRevisionAttrPtr, *m_maxRevisionAttrPtr);
}


bool CRepositoryFileTransformationCompBase::TransformFile(const IRepositoryItemInfo& /*repositoryIteminfo*/, int /*fromRevision*/, int /*toRevision*/) const
{
	return true;
}



} // namespace imtrepo


