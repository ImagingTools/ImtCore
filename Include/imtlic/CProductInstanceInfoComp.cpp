// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlic/CProductInstanceInfoComp.h>


namespace imtlic
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CProductInstanceInfoComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_productCollectionCompPtr.IsValid()){
		m_productCollectionPtr = m_productCollectionCompPtr.GetPtr();
	}

	if (m_productCollectionCompPtr.IsValid()){
		m_customerCollectionPtr = m_customerCollectionCompPtr.GetPtr();
	}
}


} // namespace imtlic


