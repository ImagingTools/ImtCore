#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtapp/CBackupSettings.h>


namespace imtapp
{


class CBackupSettingsComp:
			public icomp::CComponentBase,
			virtual public CBackupSettings
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef CBackupSettings BaseClass2;

	I_BEGIN_COMPONENT(CBackupSettingsComp);
		I_REGISTER_INTERFACE(IBackupSettings);
		I_REGISTER_INTERFACE(iser::IObject);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(istd::IChangeable);
		I_ASSIGN(m_backupPathCompPtr, "BackupFolderPath", "Backup folder path", true, "BackupFolderPath");
		I_ASSIGN(m_startTimeAttrPtr, "StartTime", "The time at which backup will start", true, "00:00");
		I_ASSIGN(m_backupIntervalAttrPtr, "BackupInterval", "Backup interval: 0 - BI_NONE, 1 - BI_DAY, 2 - BI_WEEK, 3 - BI_MONTH", true, BI_NONE);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_REF(ifile::IFileNameParam, m_backupPathCompPtr);
	I_ATTR(QString, m_startTimeAttrPtr);
	I_ATTR(int, m_backupIntervalAttrPtr);
};


} // namespace imtapp


