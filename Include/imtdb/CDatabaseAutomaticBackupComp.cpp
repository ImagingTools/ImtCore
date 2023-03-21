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

	m_timer.start(5000);
}


void CDatabaseAutomaticBackupComp::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


// private methods

bool CDatabaseAutomaticBackupComp::CheckTodayBackup() const
{
	if (!m_databaseLoginSettingsCompPtr.IsValid()){
		return false;
	}

	QString dbName = m_databaseLoginSettingsCompPtr->GetDatabaseName();
	QDir folder(m_backupPathCompPtr->GetPath());
	if (folder.exists()){
		QDate currentDate = QDate::currentDate();
		QStringList nameFilter = {dbName + "_" + currentDate.toString("yyyy-MM-dd")};
		folder.setNameFilters(nameFilter);
		if (!folder.entryList().isEmpty()){
			return false;
		}
	}

	return true;
}


// private slots

void CDatabaseAutomaticBackupComp::OnTimeout()
{
	if (m_databaseLoginSettingsCompPtr.IsValid()){
		if (!CheckTodayBackup()){
			return;
		}

		QString host = m_databaseLoginSettingsCompPtr->GetHost();
		QString dbName = m_databaseLoginSettingsCompPtr->GetDatabaseName();
		QString password = m_databaseLoginSettingsCompPtr->GetPassword();
		QString userName = m_databaseLoginSettingsCompPtr->GetUserName();
		int port = m_databaseLoginSettingsCompPtr->GetPort();

		QDateTime currentDateTime = QDateTime::currentDateTime();

		QTime time = QTime::fromString(*m_startTimeAttrPtr, "HH:mm");
		if (time.isValid()){
			QDateTime dateTime;

			dateTime.setTime(time);
			dateTime.setDate(currentDateTime.date());

			if (currentDateTime >= dateTime){
				QProcess process(0);

				QString program = "pg_dump";
				QStringList arguments;

				// -h <HostName> -p 5432 -U <UserName> -F d -b -v -f %BACKUP_FILE% <DATABASENAME>

				arguments << "-h";
				arguments << host;

				arguments << "-p";
				arguments << QString::number(port);

				arguments << "-U";
				arguments << userName;

				arguments << "-b";
				arguments << "-v";
				arguments << "-f";

				QString backupFolderPath = m_backupPathCompPtr->GetPath();

				QDir folder(backupFolderPath);
				if (!folder.exists()){
					folder.mkdir(backupFolderPath);
				}

				QString fileName = dbName + "_" + currentDateTime.date().toString("yyyy-MM-dd");
				arguments << backupFolderPath + "/" + fileName;

				arguments << dbName;

				QStringList envList;
				envList << "PGPASSWORD=" + password;
				process.setEnvironment(envList);

				process.start(program, arguments);

				process.waitForStarted();
				process.waitForFinished();

				QByteArray allStandardError = process.readAllStandardError();
				SendCriticalMessage(0, allStandardError, "Database automatic backup");
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


