#pragma once

// Qt includes
#include <QtCore/QTimer>

// ACF includes
#include <icomp/CComponentBase.h>
#include <ifile/IFileNameParam.h>
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFilePersistence.h>


// ImtCore includes
#include <imtdb/IDatabaseLoginSettings.h>


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
		I_ASSIGN(m_backupPathCompPtr, "BackupFolderPath", "Backup folder path", false, "BackupFolderPath");
		I_ASSIGN(m_startTimeAttrPtr, "StartTime", "The time at which backup will start", false, "00:00");
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	bool CheckTodayBackup() const;
private Q_SLOTS:
	void OnTimeout();

protected:
	QTimer m_timer;

protected:
	I_REF(imtdb::IDatabaseLoginSettings, m_databaseLoginSettingsCompPtr);
	I_REF(ifile::IFileNameParam, m_backupPathCompPtr);
	I_ATTR(QString, m_startTimeAttrPtr);
};


} // namespace imtdb


