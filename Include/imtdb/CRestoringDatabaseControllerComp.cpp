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

bool CRestoringDatabaseControllerComp::SetData(const QByteArray& data, QByteArray& /*dataId*/) const
{
	if (!m_databaseLoginSettingsCompPtr.IsValid()){
		SendErrorMessage(0, "Internal error. Attribute is invalid.", "CRestoringDatabaseControllerComp");

		return false;
	}

	QString host = m_databaseLoginSettingsCompPtr->GetHost();
	QString dbName = m_databaseLoginSettingsCompPtr->GetDatabaseName();
	QString password = m_databaseLoginSettingsCompPtr->GetPassword();
	QString userName = m_databaseLoginSettingsCompPtr->GetUserName();
	int port = m_databaseLoginSettingsCompPtr->GetPort();

	QTemporaryDir tempDir;
	QString filePathTmp = tempDir.path() + "/" + QUuid::createUuid().toString() + ".backup";

	QFile file(filePathTmp);
	if (!file.open(QIODevice::WriteOnly)){
		SendErrorMessage(0, QString("Unable to open file with name %1").arg(filePathTmp), "CRestoringDatabaseControllerComp");

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
			SendErrorMessage(0, QString("Unable to execute command %1").arg(pgDumpCommand), "CRestoringDatabaseControllerComp");

			return false;
		}
	}

	QString dropCommand = QString("psql -h %1 -U %2 -p %3 -c \"%4\"")
			.arg(host)
			.arg(userName)
			.arg(QString::number(port))
			.arg(QString("DROP DATABASE \"%1\" WITH (FORCE)").arg(dbName));

	if (!ExecuteCommand(process, dropCommand)){
		SendErrorMessage(0, QString("Unable to execute command %1").arg(dropCommand), "CRestoringDatabaseControllerComp");

		return false;
	}

	QString createCommand = QString("psql -h %1 -U %2 -p %3 -c \"%4\"")
				.arg(host)
				.arg(userName)
				.arg(QString::number(port))
				.arg(QString("CREATE DATABASE \"%1\"").arg(dbName));

	if (!ExecuteCommand(process, createCommand)){
		SendErrorMessage(0, QString("Unable to execute command %1").arg(createCommand), "CRestoringDatabaseControllerComp");

		return false;
	}

	QString restoreCommand = QString("psql -h %1 -U %2 -p %3 -d \"%4\" -f \"%5\"")
				.arg(host)
				.arg(userName)
				.arg(QString::number(port))
				.arg(dbName)
				.arg(filePathTmp);

	if (!ExecuteCommand(process, restoreCommand)){
		SendErrorMessage(0, QString("Unable to execute command %1").arg(restoreCommand), "CRestoringDatabaseControllerComp");

		return false;
	}

	QByteArray error = process.readAllStandardError();
	QByteArray output = process.readAllStandardOutput();

	if (!error.isEmpty()){
		SendErrorMessage(0, QString("Unable to restore database: %1").arg(qPrintable(error)), "CRestoringDatabaseControllerComp");

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

bool CRestoringDatabaseControllerComp::GetData(
			QByteArray& data,
			const QByteArray& dataId,
			qint64 /*readFromPosition*/,
			qint64 /*readMaxLength*/) const
{
	if (!m_databaseLoginSettingsCompPtr.IsValid()){
		SendErrorMessage(0, "Internal error. Attribute is invalid.", "CRestoringDatabaseControllerComp");

		return false;
	}

	if (*m_commandIdAttrPtr != dataId){
		SendWarningMessage(0, QString("Unable to get data with command-ID %1").arg(qPrintable(dataId)), "CRestoringDatabaseControllerComp");

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
		SendErrorMessage(0, QString("Unable to execute command %1").arg(pgDumpCommand), "CRestoringDatabaseControllerComp");

		return false;
	}

	QFile file(filePathTmp);
	if (!file.open(QIODevice::ReadOnly)){
		SendErrorMessage(0, QString("Unable to open file with name %1").arg(filePathTmp), "CRestoringDatabaseControllerComp");

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


