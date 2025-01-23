#include <imtdb/CDatabaseAutomaticBackupComp.h>

// Qt includes
#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>

// ACF includes
#include <iprm/TParamsPtr.h>

namespace imtdb
{

// reimplemented (icomp::CComponentBase)

void CDatabaseAutomaticBackupComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_backupSettingsCompPtr.IsValid()){
		QObject::connect(&m_timer, &QTimer::timeout, this, &CDatabaseAutomaticBackupComp::OnTimeout);

		if (*m_backupOnStartAttrPtr){
			Backup();
		}

		int interval = m_checkIntervalAttrPtr.IsValid() ? *m_checkIntervalAttrPtr : 60000;
		m_timer.start(interval);
	}
}


void CDatabaseAutomaticBackupComp::OnComponentDestroyed()
{
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

	QString host = m_databaseLoginSettingsCompPtr->GetHost();
	QString dbName = m_databaseLoginSettingsCompPtr->GetDatabaseName();
	QString password = m_databaseLoginSettingsCompPtr->GetPassword();
	QString userName = m_databaseLoginSettingsCompPtr->GetUserName();
	int port = m_databaseLoginSettingsCompPtr->GetPort();

	iprm::TParamsPtr<ifile::IFileNameParam> fileNameParamPtr(m_backupSettingsCompPtr.GetPtr(), "BackupFolder");

	QString backupFolderPath = fileNameParamPtr.IsValid() ? fileNameParamPtr->GetPath() : ".";
	QDir folder(backupFolderPath);

	if (!folder.exists() && !folder.mkpath(backupFolderPath)){
		SendErrorMessage(0, QString("Failed to create backup folder: '%1'").arg(backupFolderPath), "CDatabaseAutomaticBackupComp");
		return false;
	}

	QString fmt = "yyyyMMddhhmmss";
	QString fileName = dbName + "_" + QDateTime::currentDateTime().toString(fmt) + ".backup";
	QString filePath = folder.filePath(fileName);

	// Use pg_dump with compression to reduce backup size
	QString pgDumpCommand = QString("pg_dump -h %1 -U %2 -p %3 -Fc -f \"%4\" \"%5\"")
								.arg(host, userName, QString::number(port), filePath, dbName);

	QProcess process;
	process.setProcessEnvironment(QProcessEnvironment::systemEnvironment());
	process.setEnvironment(QStringList() << "PGPASSWORD=" + password);
	process.startCommand(pgDumpCommand);

	if (!process.waitForFinished(-1)){
		SendErrorMessage(0, QString("Backup process timed out"), "CDatabaseAutomaticBackupComp");
		return false;
	}

	if (process.exitCode() != 0){
		SendErrorMessage(0, QString("Backup failed: '%1'").arg(process.readAllStandardError()), "CDatabaseAutomaticBackupComp");
		return false;
	}

	SendInfoMessage(0, QString("Backup successful: '%1'").arg(filePath), "CDatabaseAutomaticBackupComp");

	m_lastBackupDateTime = QDateTime::currentDateTime();

	CleanupOldBackups(backupFolderPath);

	return true;
}


void CDatabaseAutomaticBackupComp::CleanupOldBackups(const QString& backupFolderPath)
{
	if (!m_databaseLoginSettingsCompPtr.IsValid()){
		return;
	}

	QString dbName = m_databaseLoginSettingsCompPtr->GetDatabaseName();

	QDir folder(backupFolderPath);
	QStringList filters;
	filters << dbName + "_*.backup";

	folder.setSorting(QDir::Time | QDir::Reversed);
	folder.setFilter(QDir::Files);

	QStringList backupFiles = folder.entryList(filters);

	// Maximum number of backups to keep
	int maxBackups = m_maxBackupCountAttrPtr.IsValid() ? *m_maxBackupCountAttrPtr : 20;
	if (maxBackups < 0){
		return;
	}

	while (backupFiles.size() > maxBackups){
		QString oldestBackup = backupFiles.takeFirst();
		if (!folder.remove(oldestBackup)){
			SendErrorMessage(0, QString("Failed to remove old backup: '%1'").arg(oldestBackup), "CDatabaseAutomaticBackupComp");
		}
		else{
			SendInfoMessage(0, QString("Deleted old backup: '%1'").arg(oldestBackup), "CDatabaseAutomaticBackupComp");
		}
	}
}


// private slots

void CDatabaseAutomaticBackupComp::OnTimeout()
{
	if (!m_backupSettingsCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'BackupSettings' was not set", "CDatabaseAutomaticBackupComp");
		return;
	}

	iprm::TParamsPtr<imtapp::ISchedulerParams> schedulerParamPtr(m_backupSettingsCompPtr.GetPtr(), "SchedulerParams");
	if (!schedulerParamPtr.IsValid()){
		SendErrorMessage(0, QString("Scheduler parameters are invalid"), "CDatabaseAutomaticBackupComp");
		return;
	}

	QDateTime currentDateTime = QDateTime::currentDateTime();
	QDateTime startTime = schedulerParamPtr->GetStartTime();

	if (!startTime.isValid()){
		SendErrorMessage(0, QString("Invalid start time"), "CDatabaseAutomaticBackupComp");
		return;
	}

	if (currentDateTime >= startTime){
		bool shouldBackup = false;

		if (m_lastBackupDateTime.isValid()){
			int interval = schedulerParamPtr->GetInterval();
			int secs = m_lastBackupDateTime.secsTo(currentDateTime);
			if (secs >= interval){
				shouldBackup = true;
			}
		}
		else{
			shouldBackup = true;
		}

		if (shouldBackup){
			SendInfoMessage(0, QString("Starting database backup..."), "CDatabaseAutomaticBackupComp");

			if (Backup()){
				SendInfoMessage(0, QString("Database backup completed successfully"), "CDatabaseAutomaticBackupComp");
			}
			else{
				SendWarningMessage(0, QString("Database backup failed"), "CDatabaseAutomaticBackupComp");
			}
		}
	}
}


} // namespace imtdb
