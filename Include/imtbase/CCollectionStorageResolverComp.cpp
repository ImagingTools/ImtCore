// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CCollectionStorageResolverComp.h>


namespace imtbase
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CCollectionStorageResolverComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	SetCollection(m_collectionCompPtr.GetPtr(), *m_typeIdAttrPtr);
}


} // namespace imtbase


