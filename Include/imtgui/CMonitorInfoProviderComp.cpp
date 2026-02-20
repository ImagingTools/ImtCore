// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgui/CMonitorInfoProviderComp.h>


// Qt includes
#include <QtGui/QGuiApplication>


namespace imtgui
{


// reimplemented (icomp::CComponentBase)

void CMonitorInfoProviderComp::OnComponentCreated()
{
	if (m_physicalWidthAttrPtr.IsValid()){
		if (m_physicalHeightAttrPtr.IsValid()){
			istd::IChangeable::ChangeSet changeSet(MCE_PHYSICAL_SIZE);
			istd::CChangeNotifier changeNotifier(this, &changeSet);

			m_physicalSize = QSize(*m_physicalWidthAttrPtr, *m_physicalHeightAttrPtr);
			UpdateMonitorsInfo();
		}
	}
}



} // namespace imtgui


