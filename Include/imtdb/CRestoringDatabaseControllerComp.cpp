#include <imtdb/CRestoringDatabaseControllerComp.h>


// Qt includes
#include <QtCore/QTemporaryDir>
#include <QtCore/QTemporaryFile>
#include <QtCore/QUuid>

// ACF includes
#include <iprm/TParamsPtr.h>
#include <ifile/IFileNameParam.h>


namespace imtdb
{


// public methods

// reimplemented (imtbase::IBinaryDataController)

bool CRestoringDatabaseControllerComp::SetData(const QByteArray& data, QByteArray& dataId) const
{
	if (!m_databaseLoginSettingsCompPtr.IsValid()){
		return false;
	}

	QString host = m_databaseLoginSettingsCompPtr->GetHost();
	QString dbName = m_databaseLoginSettingsCompPtr->GetDatabaseName();
	QString password = m_databaseLoginSettingsCompPtr->GetPassword();
	QString userName = m_databaseLoginSettingsCompPtr->GetUserName();
	int port = m_databaseLoginSettingsCompPtr->GetPort();

	dbName = "prolife";

	QTemporaryDir tempDir;
	QString filePathTmp = tempDir.path() + "/" + QUuid::createUuid().toString() + ".backup";

	QFile file(filePathTmp);
	if (!file.open(QIODevice::WriteOnly)){
		return false;
	}

	file.write(data);

	file.close();

	QProcess process;

	QStringList envList;
	envList << "PGPASSWORD=" + password;
	process.setEnvironment(envList);

	if (m_backupBeforeRestoreAttrPtr.IsValid() && *m_backupBeforeRestoreAttrPtr){
		QString backupFolderPath = QDir::tempPath();
		iprm::TParamsPtr<ifile::IFileNameParam> fileNameParamPtr(m_backupSettingsCompPtr.GetPtr(), "BackupFolder");
		if (m_backupSettingsCompPtr.IsValid()){
			backupFolderPath = fileNameParamPtr->GetPath();
		}

		QDir folder(backupFolderPath);
		if (!folder.exists()){
			folder.mkdir(backupFolderPath);
		}

		QString fmt = "yyyyMMddhhmmss";
		QString fileName = dbName + "_" + QDateTime::currentDateTime().toString(fmt);

		QString fullPath = backupFolderPath + "/" + fileName;

		QString pgDumpCommand = QString("pg_dump -h %1 -U %2 -p %3 -b -v -f \"%4\" \"%5\"")
				.arg(host)
				.arg(userName)
				.arg(QString::number(port))
				.arg(fullPath)
				.arg(dbName);

		if (!ExecuteCommand(process, pgDumpCommand)){
			return false;
		}
	}

	QString dropCommand = QString("psql -h %1 -U %2 -p %3 -c \"%4\"")
			.arg(host)
			.arg(userName)
			.arg(QString::number(port))
			.arg(QString("DROP DATABASE \"%1\" WITH (FORCE)").arg(dbName));

	if (!ExecuteCommand(process, dropCommand)){
		return false;
	}

	QString createCommand = QString("psql -h %1 -U %2 -p %3 -c \"%4\"")
			.arg(host)
			.arg(userName)
			.arg(QString::number(port))
			.arg(QString("CREATE DATABASE \"%1\"").arg(dbName));

	if (!ExecuteCommand(process, createCommand)){
		return false;
	}

	QString restoreCommand = QString("psql -h %1 -U %2 -p %3 -d \"%4\" -f \"%5\"")
			.arg(host)
			.arg(userName)
			.arg(QString::number(port))
			.arg(dbName)
			.arg(filePathTmp);

	if (!ExecuteCommand(process, restoreCommand)){
		return false;
	}

	QByteArray error = process.readAllStandardError();
	QByteArray output = process.readAllStandardOutput();

	if (!error.isEmpty()){
		return false;
	}

	return true;
}


bool CRestoringDatabaseControllerComp::RemoveData(const QByteArray& /*dataId*/) const
{
	return false;
}


bool CRestoringDatabaseControllerComp::EnsureDataExists(const QByteArray& /*dataId*/) const
{
	return false;
}


// reimplemented (imtbase::IBinaryDataProvider)

bool CRestoringDatabaseControllerComp::GetData(QByteArray& data, const QByteArray& dataId) const
{
	if (!m_databaseLoginSettingsCompPtr.IsValid()){
		return false;
	}

	if (*m_commandIdAttrPtr != dataId){
		return false;
	}

	QString host = m_databaseLoginSettingsCompPtr->GetHost();
	QString dbName = m_databaseLoginSettingsCompPtr->GetDatabaseName();
	QString password = m_databaseLoginSettingsCompPtr->GetPassword();
	QString userName = m_databaseLoginSettingsCompPtr->GetUserName();
	int port = m_databaseLoginSettingsCompPtr->GetPort();

	QProcess process;

	QStringList envList;
	envList << "PGPASSWORD=" + password;
	process.setEnvironment(envList);

	QTemporaryDir tempDir;
	QString filePathTmp = tempDir.path() + "/" + QUuid::createUuid().toString() + ".backup";

	QString pgDumpCommand = QString("pg_dump -h %1 -U %2 -p %3 -b -v -f \"%4\" \"%5\"")
			.arg(host)
			.arg(userName)
			.arg(QString::number(port))
			.arg(filePathTmp)
			.arg(dbName);

	if (!ExecuteCommand(process, pgDumpCommand)){
		return false;
	}

	QFile file(filePathTmp);
	if (!file.open(QIODevice::ReadOnly)){
		return false;
	}

	data = file.readAll();

	file.close();

	return true;
}


// protected methods

bool CRestoringDatabaseControllerComp::ExecuteCommand(QProcess& process, const QString& command) const
{
	process.start(command);

	if(!process.waitForFinished()){
		return false;
	}

	return true;
}


} // namespace imtdb


