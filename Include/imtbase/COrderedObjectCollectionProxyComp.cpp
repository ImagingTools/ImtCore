// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/COrderedObjectCollectionProxyComp.h>


namespace imtbase
{


// protected methods

// reimplemented (icomp::CComponentBase)

void COrderedObjectCollectionProxyComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_collectionCompPtr.IsValid()){
		SetCollection(m_collectionCompPtr.GetPtr(), false);
	}
}


} // namespace imtbase


