// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
		I_ASSIGN(m_startTimeAttrPtr, "StartTime", "The time at which backup will start", true, "dd-MM-yyyy HH:mm");
		I_ASSIGN(m_backupIntervalAttrPtr, "BackupInterval", "Backup interval", true, 0);
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


