// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CMigrationControllerComp.h>


// Qt includes
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtCore/QDir>
#include <QtCore/QRegularExpression>


namespace imtdb
{


// protected methods

// reimplemented (imtdb::IMigrationController)

bool CMigrationControllerComp::DoMigration(int& resultRevision, const istd::CIntRange& subRange) const
{
	resultRevision = -1;

	if (!m_databaseEngineCompPtr.IsValid()){
		qCritical() << __func__ << "Unable to migrate. Database engine component is not set";

		return false;
	}

	if (!m_migrationFolderPathCompPtr.IsValid()){
		qCritical() << __func__ << "Unable to migrate. Migration folder path component is not set";

		return false;
	}

	int min = m_range.GetMinValue();
	int max = m_range.GetMaxValue();

	if (subRange.IsValid() && !m_range.Contains(subRange)){
		qWarning() << __func__ << "Unable to migrate. Unexpected range provided";

		return false;
	}

	QDir folder(m_migrationFolderPathCompPtr->GetPath());
	if (!folder.exists()){
		qWarning() << __func__ << "Unable to migrate. Migration dir does not exist" << folder.absolutePath() << m_migrationFolderPathCompPtr->GetPath();

		return false;
	}

	int startIndex = min;
	int endIndex = max;

	if (subRange.IsValid()){
		startIndex = subRange.GetMinValue();
		endIndex = subRange.GetMaxValue();
	}
	else{
		if (subRange.GetMinValue() >= 0){
			startIndex = subRange.GetMinValue();
		}
	}

	QStringList nameFilter = {"migration_*.sql"};
	folder.setNameFilters(nameFilter);

	for (int i = startIndex; i <= endIndex; i++){
		QString migrationFilePath = folder.filePath("migration_" + QString::number(i) + ".sql");

		QFileInfo fileInfo(migrationFilePath);
		if (!fileInfo.exists()){
			SendErrorMessage(0, QString("Unable to do migration number '%1'. Error: File '%2' was not found").arg(i).arg(migrationFilePath), "CMigrationControllerComp");

			continue;
		}

		QSqlError sqlError;
		m_databaseEngineCompPtr->ExecSqlQueryFromFile(migrationFilePath.toUtf8(), &sqlError);

		if (sqlError.type() != QSqlError::NoError){
			SendErrorMessage(0, QString("Unable to execute migration file '%1'. Error: '%2'").arg(migrationFilePath, sqlError.text()), "CMigrationControllerComp");

			return false;
		}

		resultRevision = endIndex;

		SendInfoMessage(0, QString("Migration '%1' succesfully migrated").arg(i), "CMigrationControllerComp");
	}

	return true;
}


// reimplemented (icomp::CComponentBase)

void CMigrationControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	int min = *m_rangeFromAttrPtr;
	int max = *m_rangeToAttrPtr;

	if (min < 0){
		min = 0;
	}

	if (max < 0){
		if (m_migrationFolderPathCompPtr.IsValid()){
			QString migrationFolder = m_migrationFolderPathCompPtr->GetPath();

			QString errorMessage;
			max = GetLastMigration(migrationFolder, errorMessage);
		}
	}

	m_range = istd::CIntRange(min, max);
}


// private methods

int CMigrationControllerComp::GetLastMigration(const QString& migrationFolder, QString& errorMessage) const
{
	QDir folder(migrationFolder);
	if (folder.exists()){
		QStringList nameFilter = {"migration_*.sql"};
		folder.setNameFilters(nameFilter);
		int avaliableMigration = -1;
		if (!folder.entryList().isEmpty()){
			QStringList files = folder.entryList();
			for (int index = 0; index < files.size(); index++){
				QString nameFile = files[index];
				nameFile.remove("migration_").remove(".sql");
				QRegularExpression re("\\d*");
				if (re.match(nameFile).hasMatch()){
					if (avaliableMigration < nameFile.toInt()){
						avaliableMigration = nameFile.toInt();
					}
				}
			}
		}

		return avaliableMigration;
	}

	errorMessage = QString("Directory containing SQL-migration files doesn't exist: '%1'").arg(migrationFolder);

	return -1;
}


} // namespace imtdb


