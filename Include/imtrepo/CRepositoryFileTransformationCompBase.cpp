// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


