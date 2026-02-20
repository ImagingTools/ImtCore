// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imt3dgui/CPointCloudShapeComp.h>


namespace imt3dgui
{

void CPointCloudShapeComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_isInfoBoxEnabledAttrPtr.IsValid()){
		SetInfoBoxEnabled(*m_isInfoBoxEnabledAttrPtr);
	}
}


} // namespace imt3dgui


