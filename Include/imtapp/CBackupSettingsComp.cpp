// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtapp/CBackupSettingsComp.h>


namespace imtapp
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CBackupSettingsComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	QString backupPath = m_backupPathCompPtr->GetPath();
	BaseClass2::SetPath(backupPath);

	QDateTime time = QDateTime::fromString(*m_startTimeAttrPtr, "dd-MM-yyyy HH:mm");
	if (time.isValid()){
		BaseClass2::SetStartTime(time);
	}

	BaseClass2::SetInterval(*m_backupIntervalAttrPtr);
}


} // namespace imtapp


