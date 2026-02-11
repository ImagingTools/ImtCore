// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CStatusManagerComp.h>


namespace imtbase
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CStatusManagerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_defaultDescriptionAttrPtr.IsValid()){
		SetInformationDescription(*m_defaultDescriptionAttrPtr);
	}

	if (m_defaultSourceAttrPtr.IsValid()){
		SetInformationSource(*m_defaultSourceAttrPtr);
	}
}


} // namespace imtbase


