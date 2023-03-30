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

	QObject::connect(&m_timer, &QTimer::timeout, this, &CDatabaseAutomaticBackupComp::OnTimeout);

	if (*m_backupOnStartAttrPtr){
		Backup();
	}

	int interval = m_checkIntervalAttrPtr.IsValid() ? *m_checkIntervalAttrPtr : 60000;
	m_timer.start(interval);
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

	QProcess process(0);

	QString program = "pg_dump";
	QStringList arguments;

	arguments << "-h";
	arguments << host;

	arguments << "-p";
	arguments << QString::number(port);

	arguments << "-U";
	arguments << userName;

	arguments << "-b";
	arguments << "-v";
	arguments << "-f";

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
	arguments << backupFolderPath + "/" + fileName;

	arguments << dbName;

	QStringList envList;
	envList << "PGPASSWORD=" + password;
	process.setEnvironment(envList);

	process.start(*m_programAttrPtr, arguments);

	m_lastBackupDateTime = QDateTime::currentDateTime();

	process.waitForStarted();
	process.waitForFinished();

	return true;
}


// private slots

void CDatabaseAutomaticBackupComp::OnTimeout()
{
	if (m_databaseLoginSettingsCompPtr.IsValid() && m_backupSettingsCompPtr.IsValid()){
		iprm::TParamsPtr<imtapp::ISchedulerParams> schedulerParamPtr(m_backupSettingsCompPtr.GetPtr(), "SchedulerParams");
		if (schedulerParamPtr.IsValid()){
			QDateTime currentDateTime = QDateTime::currentDateTime();
			QDateTime startTime = schedulerParamPtr->GetStartTime();
			if (startTime.isValid()){
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
						Backup();
					}
				}
			}
			else{
				SendErrorMessage(0, "time is not valid", "Database automatic backup");
			}
		}
	}
	else{
		SendErrorMessage(0, "m_databaseLoginSettingsCompPtr is not valid", "Database automatic backup");
	}
}


} // namespace imtdb


