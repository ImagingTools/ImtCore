#include <imtdb/CDatabaseAutomaticBackupComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QDateTime>

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
		return false;
	}

	if (!m_backupSettingsCompPtr.IsValid()){
		return false;
	}

	QString host = m_databaseLoginSettingsCompPtr->GetHost();
	QString dbName = m_databaseLoginSettingsCompPtr->GetDatabaseName();
	QString password = m_databaseLoginSettingsCompPtr->GetPassword();
	QString userName = m_databaseLoginSettingsCompPtr->GetUserName();
	int port = m_databaseLoginSettingsCompPtr->GetPort();

	iprm::TParamsPtr<ifile::IFileNameParam> fileNameParamPtr(m_backupSettingsCompPtr.GetPtr(), "BackupFolder");

	QString backupFolderPath = ".";
	if (fileNameParamPtr.IsValid()){
		backupFolderPath = fileNameParamPtr->GetPath();
	}

	QDir folder(backupFolderPath);
	if (!folder.exists()){
		folder.mkdir(backupFolderPath);
	}

	QString fmt = "yyyyMMddhhmmss";
	QString fileName = dbName + "_" + QDateTime::currentDateTime().toString(fmt);

	QString pgDumpCommand = QString("pg_dump -h %1 -U %2 -p %3 -b -v -f \"%4\" \"%5\"")
			.arg(host)
			.arg(userName)
			.arg(QString::number(port))
			.arg(backupFolderPath + "/" + fileName)
			.arg(dbName);

	QProcess process;

	QStringList envList;
	envList << "PGPASSWORD=" + password;
	process.setEnvironment(envList);

	process.start(pgDumpCommand);

	if(!process.waitForFinished()){
		return false;
	}

	m_lastBackupDateTime = QDateTime::currentDateTime();

	return true;
}


// private slots

void CDatabaseAutomaticBackupComp::OnTimeout()
{
	if (!m_backupSettingsCompPtr.IsValid()){
		return;
	}

	iprm::TParamsPtr<imtapp::ISchedulerParams> schedulerParamPtr(m_backupSettingsCompPtr.GetPtr(), "SchedulerParams");
	if (!schedulerParamPtr.IsValid()){
		SendErrorMessage(0, QString("Error when trying to backup the database. Error: Backup settings is invalid."), "CDatabaseAutomaticBackupComp");
		return;
	}

	QDateTime currentDateTime = QDateTime::currentDateTime();
	QDateTime startTime = schedulerParamPtr->GetStartTime();
	if (!startTime.isValid()){
		SendErrorMessage(0, QString("Error when trying to backup the database. Error: Start time from backup settings is invalid."), "CDatabaseAutomaticBackupComp");

		return;
	}

	if (currentDateTime >= startTime){
		bool ok = false;
		if (m_lastBackupDateTime.isValid()){
			int interval = schedulerParamPtr->GetInterval();
			int secs = m_lastBackupDateTime.secsTo(currentDateTime);
			if (secs >= interval){
				ok = true;
			}
		}
		else{
			ok = true;
		}

		if (ok){
			SendInfoMessage(0, QString("Attempt to create a backup of the database ..."), "CDatabaseAutomaticBackupComp");

			bool result = Backup();
			if (result){
				SendInfoMessage(0, QString("The database backup was completed successfully"), "CDatabaseAutomaticBackupComp");
			}
			else{
				SendInfoMessage(0, QString("Error when trying to make a database backup"), "CDatabaseAutomaticBackupComp");
			}
		}
	}
}


} // namespace imtdb


