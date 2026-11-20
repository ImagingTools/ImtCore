// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CCollectionStorageResolverComp.h>


namespace imtbase
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CCollectionStorageResolverComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	// The type-ID is needed only to insert objects, so the resolver works without it.
	const QByteArray typeId = m_typeIdAttrPtr.IsValid() ? *m_typeIdAttrPtr : QByteArray();

	SetCollection(m_collectionCompPtr.GetPtr(), typeId);
}


} // namespace imtbase


