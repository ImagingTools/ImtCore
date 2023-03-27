#include <imtdb/CDatabaseAutomaticBackupComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QDateTime>


namespace imtdb
{


// reimplemented (icomp::CComponentBase)

void CDatabaseAutomaticBackupComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	QObject::connect(&m_timer, &QTimer::timeout, this, &CDatabaseAutomaticBackupComp::OnTimeout);

	Backup();

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

	if (!m_relativeFilePathCompPtr.IsValid()){
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

	QString backupFolderPath = m_backupSettingsCompPtr->GetPath();

	QDir folder(backupFolderPath);
	if (!folder.exists()){
		folder.mkdir(backupFolderPath);
	}

//	QString fileName = m_relativeFilePathCompPtr->GetPath();
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
		QDateTime currentDateTime = QDateTime::currentDateTime();
		QTime startTime = m_backupSettingsCompPtr->GetStartTime();
		if (startTime.isValid()){
			QDateTime intervalDateTime(m_lastBackupDateTime);

			QDateTime dateTime(currentDateTime.date(), startTime);
			if (currentDateTime >= dateTime){
				imtapp::IBackupSettings::BackupInterval interval = m_backupSettingsCompPtr->GetInterval();

				bool ok = false;
				int countDays = m_lastBackupDateTime.daysTo(currentDateTime);
				if ((interval == imtapp::IBackupSettings::BackupInterval::BI_DAY && countDays >= 1) ||
						(interval == imtapp::IBackupSettings::BackupInterval::BI_WEEK && countDays >= 7) ||
						(interval == imtapp::IBackupSettings::BackupInterval::BI_MONTH && countDays >= 30)){
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
	else{
		SendErrorMessage(0, "m_databaseLoginSettingsCompPtr is not valid", "Database automatic backup");
	}
}


} // namespace imtdb


