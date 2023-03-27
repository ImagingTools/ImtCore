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

	QTime time = QTime::fromString(*m_startTimeAttrPtr, "HH:mm");
	if (time.isValid()){
		BaseClass2::SetStartTime(time);
	}

	switch (*m_backupIntervalAttrPtr){
	case IBackupSettings::BI_NONE:
		BaseClass2::SetInterval(IBackupSettings::BI_NONE);
		break;
	case IBackupSettings::BI_DAY:
		BaseClass2::SetInterval(IBackupSettings::BI_DAY);
		break;
	case IBackupSettings::BI_WEEK:
		BaseClass2::SetInterval(IBackupSettings::BI_WEEK);
		break;
	case IBackupSettings::BI_MONTH:
		BaseClass2::SetInterval(IBackupSettings::BI_MONTH);
		break;
	}
}


} // namespace imtapp


