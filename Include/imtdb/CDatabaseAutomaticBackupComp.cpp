// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CDatabaseAutomaticBackupComp.h>


// Qt includes
#include <QtCore/QProcess>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtConcurrent/QtConcurrent>

// ACF includes
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtbase/imtbase.h>


namespace imtdb
{


// reimplemented (icomp::CComponentBase)

void CDatabaseAutomaticBackupComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (!m_backupSettingsCompPtr.IsValid()){
		return;
	}

	connect(&m_backupWatcher, &QFutureWatcher<bool>::finished, this, &CDatabaseAutomaticBackupComp::OnBackupFinished);
	connect(&m_timer, &QTimer::timeout, this, &CDatabaseAutomaticBackupComp::OnTimeout);

	m_timer.setInterval(*m_checkIntervalAttrPtr);

	if (!*m_backupOnStartAttrPtr){
		m_timer.start();
	}
	else{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
		m_backupWatcher.setFuture(QtConcurrent::run(this, &CDatabaseAutomaticBackupComp::Backup));
#else
		m_backupWatcher.setFuture(QtConcurrent::run(&CDatabaseAutomaticBackupComp::Backup, this));
#endif
	}
}


void CDatabaseAutomaticBackupComp::OnComponentDestroyed()
{
	m_backupWatcher.disconnect(this);

	if (m_backupWatcher.isStarted()){
		m_backupWatcher.cancel();
		m_backupWatcher.waitForFinished();
	}

	BaseClass::OnComponentDestroyed();
}


// private methods

bool CDatabaseAutomaticBackupComp::Backup()
{
	if (!m_databaseLoginSettingsCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'DatabaseLoginSettings' was not set", "CDatabaseAutomaticBackupComp");

		return false;
	}

	if (!m_backupSettingsCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'BackupSettings' was not set", "CDatabaseAutomaticBackupComp");

		return false;
	}

	static const QString host		= m_databaseLoginSettingsCompPtr->GetHost();
	static const QString dbName		= m_databaseLoginSettingsCompPtr->GetDatabaseName();
	static const QString password	= m_databaseLoginSettingsCompPtr->GetPassword();
	static const QString userName	= m_databaseLoginSettingsCompPtr->GetUserName();
	static const int port			= m_databaseLoginSettingsCompPtr->GetPort();

	iprm::TParamsPtr<ifile::IFileNameParam> fileNameParamPtr(m_backupSettingsCompPtr.GetPtr(), QByteArrayLiteral("BackupFolder"), false);

	const QString backupFolderPath = fileNameParamPtr.IsValid() ? fileNameParamPtr->GetPath() : QStringLiteral(".");
	QDir folder(backupFolderPath);

	if (!folder.exists() && !folder.mkpath(backupFolderPath)){
		SendErrorMessage(0, QString("Failed to create backup folder: '%1'").arg(backupFolderPath), QStringLiteral("CDatabaseAutomaticBackupComp"));

		return false;
	}

	static const QString fmt = QStringLiteral("yyyyMMddhhmmss");
	const QString fileName = dbName + '_' + QDateTime::currentDateTime().toString(fmt) + QStringLiteral(".backup");
	const QString filePath = folder.filePath(fileName);

	// Use pg_dump with compression to reduce backup size
	QString pgDumpCommand = QStringLiteral("pg_dump -h %1 -U %2 -p %3 -Fc -f \"%4\" \"%5\"")
								.arg(host, userName, QString::number(port), filePath, dbName);

	QProcess process;
	process.setProcessEnvironment(QProcessEnvironment::systemEnvironment());
	process.setEnvironment({ QStringLiteral("PGPASSWORD=") + password });

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	process.start(pgDumpCommand);
#else
	process.startCommand(pgDumpCommand);
#endif

	if (!process.waitForFinished(-1)){
		SendErrorMessage(0, QStringLiteral("Backup process timed out"), QStringLiteral("CDatabaseAutomaticBackupComp"));

		return false;
	}

	if (process.exitCode() != 0){
		SendErrorMessage(0, QStringLiteral("Backup failed: '%1'").arg(QString::fromUtf8(process.readAllStandardError())), QStringLiteral("CDatabaseAutomaticBackupComp"));

		return false;
	}

	SendInfoMessage(0, QStringLiteral("Backup successful: '%1'").arg(filePath), QStringLiteral("CDatabaseAutomaticBackupComp"));

	m_lastBackupDateTime = QDateTime::currentDateTime();

	CleanupOldBackups(backupFolderPath);

	return true;
}


void CDatabaseAutomaticBackupComp::CleanupOldBackups(const QString& backupFolderPath)
{
	// Maximum number of backups to keep
	if (*m_maxBackupCountAttrPtr < 0){
		return;
	}

	QStringList backupFiles = GetPrevBackupsList(backupFolderPath);

	QDir folder(backupFolderPath);
	while (backupFiles.size() > *m_maxBackupCountAttrPtr){
		const QString oldestBackup = backupFiles.takeFirst();
		if (!folder.remove(oldestBackup)){
			SendErrorMessage(0, QStringLiteral("Failed to remove old backup: '%1'").arg(oldestBackup), QStringLiteral("CDatabaseAutomaticBackupComp"));
		}
		else{
			SendInfoMessage(0, QStringLiteral("Deleted old backup: '%1'").arg(oldestBackup), QStringLiteral("CDatabaseAutomaticBackupComp"));
		}
	}
}


QStringList CDatabaseAutomaticBackupComp::GetPrevBackupsList(const QString& backupFolderPath, const QDir::SortFlags& sortFlags)
{
	QDir folder(backupFolderPath);
	if (!m_databaseLoginSettingsCompPtr.IsValid() && !folder.exists()){
		return QStringList();
	}

	static const QString dbName = m_databaseLoginSettingsCompPtr->GetDatabaseName();
	const QStringList filters({ dbName + QStringLiteral("_*.backup") });

	folder.setSorting(sortFlags);
	folder.setFilter(QDir::Files);

	return folder.entryList(filters);
}


// private slots

void CDatabaseAutomaticBackupComp::OnBackupFinished()
{
	bool backupSuccess = m_backupWatcher.result();
	if(backupSuccess){
		SendInfoMessage(0, QStringLiteral("Database backup completed successfully"), QStringLiteral("CDatabaseAutomaticBackupComp"));
	}
	else{
		SendWarningMessage(0, QStringLiteral("Database backup failed"), QStringLiteral("CDatabaseAutomaticBackupComp"));
	}

	if (!m_timer.isActive()){
		m_timer.start();
	}
}


void CDatabaseAutomaticBackupComp::OnTimeout()
{
	if (!m_backupSettingsCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'BackupSettings' was not set", "CDatabaseAutomaticBackupComp");

		return;
	}

	if(m_backupWatcher.isRunning()){
		return;
	}

	iprm::TParamsPtr<imtapp::ISchedulerParams> schedulerParamPtr(m_backupSettingsCompPtr.GetPtr(), QByteArrayLiteral("SchedulerParams"));
	if (!schedulerParamPtr.IsValid()){
		SendErrorMessage(0, QStringLiteral("Scheduler parameters are invalid"), QStringLiteral("CDatabaseAutomaticBackupComp"));

		return;
	}

	const QDateTime currentDateTime = QDateTime::currentDateTime();
	QDateTime startTime = schedulerParamPtr->GetStartTime();
	if(currentDateTime.daysTo(startTime) <= 0){
		startTime.setDate(currentDateTime.date());
	}

	if (!startTime.isValid()){
		SendErrorMessage(0, QStringLiteral("Invalid start time"), QStringLiteral("CDatabaseAutomaticBackupComp"));

		return;
	}

	if (currentDateTime < startTime){
		return;
	}

	/** Prevent reduntant backups right after startup if any backups within the given time constraint already exist.
		Check latest backup modification time if such backup exists and set it as last backup time.
		Ensure continued backup creation in case of manual backup directory cleanup.
	*/
	iprm::TParamsPtr<ifile::IFileNameParam> fileNameParamPtr(m_backupSettingsCompPtr.GetPtr(), QByteArrayLiteral("BackupFolder"), false);
	const QString backupFolderPath = fileNameParamPtr.IsValid() ? fileNameParamPtr->GetPath() : QStringLiteral(".");

	QStringList backupFiles = GetPrevBackupsList(backupFolderPath, QDir::Time);
	if(backupFiles.isEmpty()){
		m_lastBackupDateTime = QDateTime();
	}
	else{
		const QString latestBackupName = backupFiles.takeFirst();
		const QFile latestBackup(backupFolderPath + '/' + latestBackupName);
		if(latestBackup.exists()){\
			const qint64 latestBackupTimeSecs = latestBackup.fileTime(QFileDevice::FileModificationTime).toSecsSinceEpoch();
			const QDateTime latestBackupTime = QDateTime::fromSecsSinceEpoch(latestBackupTimeSecs);
			if(latestBackupTime.isValid()){
				m_lastBackupDateTime = latestBackupTime;
			}
		}
	}

	const bool validLastBackupDate = m_lastBackupDateTime.isValid();
	bool shouldBackup = !validLastBackupDate;
	if (validLastBackupDate){
		const int interval = schedulerParamPtr->GetInterval();
		const int secs = imtbase::narrow_cast<int>(m_lastBackupDateTime.secsTo(currentDateTime));
		shouldBackup = bool(secs >= interval);
	}

	if (shouldBackup){
		SendInfoMessage(0, QStringLiteral("Starting database backup..."), QStringLiteral("CDatabaseAutomaticBackupComp"));

		m_timer.stop();

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
		m_backupWatcher.setFuture(QtConcurrent::run(this, &CDatabaseAutomaticBackupComp::Backup));
#else
		m_backupWatcher.setFuture(QtConcurrent::run(&CDatabaseAutomaticBackupComp::Backup, this));
#endif
	}
}


} // namespace imtdb

