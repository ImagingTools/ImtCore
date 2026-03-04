// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlic/CProductLicensingInfoComp.h>


namespace imtlic
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CProductLicensingInfoComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_featurePackagesCompPtr.IsValid()){
		m_featurePackagesPtr = m_featurePackagesCompPtr.GetPtr();
	}
}


} // namespace imtlic


