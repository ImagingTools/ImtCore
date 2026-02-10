// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlic/CProductInfoComp.h>



namespace imtlic
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CProductInfoComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_featureInfoProviderCompPtr.IsValid()){
		m_featureInfoProviderPtr = m_featureInfoProviderCompPtr.GetPtr();
	}
}


} // namespace imtlic


