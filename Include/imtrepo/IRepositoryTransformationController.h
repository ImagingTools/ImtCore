// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtrepo
{


class IFileObjectCollection;


/**
	Interface for transformation repository from the repository format into another.
	The transformation interface supports both directions: upgrade and downgrade of repository data and internal layout.
	\ingroup Collection
	\ingroup FileRepository
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


