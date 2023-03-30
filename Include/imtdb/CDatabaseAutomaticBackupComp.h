#pragma once

// Qt includes
#include <QtCore/QTimer>

// ACF includes
#include <icomp/CComponentBase.h>
#include <ifile/IFileNameParam.h>
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFilePersistence.h>
#include <ifile/IRelativeFilePath.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtdb/IDatabaseLoginSettings.h>
#include <imtapp/IBackupSettings.h>


namespace imtdb
{


class CDatabaseAutomaticBackupComp:
		public QObject,
		public ilog::CLoggerComponentBase
{
	Q_OBJECT
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDatabaseAutomaticBackupComp);
		I_ASSIGN(m_databaseLoginSettingsCompPtr, "DatabaseLoginSettings", "Database login settings", true, "DatabaseLoginSettings");
		I_ASSIGN(m_backupSettingsCompPtr, "BackupSettings", "Backup settings", true, "BackupSettings");
		I_ASSIGN(m_programAttrPtr, "Program", "Program name", true, "Program");
		I_ASSIGN(m_checkIntervalAttrPtr, "CheckInterval", "Interval for backup timer", false, 60000);
		I_ASSIGN(m_backupOnStartAttrPtr, "BackupOnStart", "Backup on start", true, true);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	bool Backup();

private Q_SLOTS:
	void OnTimeout();

protected:
	QTimer m_timer;

protected:
	I_REF(imtdb::IDatabaseLoginSettings, m_databaseLoginSettingsCompPtr);
	I_REF(iprm::IParamsSet, m_backupSettingsCompPtr);
	I_ATTR(QString, m_programAttrPtr);
	I_ATTR(int, m_checkIntervalAttrPtr);
	I_ATTR(bool, m_backupOnStartAttrPtr);

private:
	QDateTime m_lastBackupDateTime;
};


} // namespace imtdb


