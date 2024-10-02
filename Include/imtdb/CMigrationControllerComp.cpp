#include <imtdb/CMigrationControllerComp.h>


// Qt includes
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtCore/QDir>
#include <QtCore/QRegularExpression>

// ImtCore includes
#include <imtdb/imtdb.h>


namespace imtdb
{


// protected methods

// reimplemented (imtdb::IMigrationController)

istd::CIntRange CMigrationControllerComp::GetMigrationRange() const
{
	return m_range;
}


bool CMigrationControllerComp::DoMigration(const istd::CIntRange& subRange) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return false;
	}

	if (!m_migrationFolderPathCompPtr.IsValid()){
		return false;
	}

	QString migrationFolder = m_migrationFolderPathCompPtr->GetPath();

	int min = m_range.GetMinValue();
	int max = m_range.GetMaxValue();

	if (subRange.IsValid() && !m_range.Contains(subRange)){
		return false;
	}

	QDir folder(m_migrationFolderPathCompPtr->GetPath());
	if (!folder.exists()){
		return false;
	}

	int startIndex = min;
	int endIndex = max;

	if (subRange.IsValid()){
		startIndex = subRange.GetMinValue();
		endIndex = subRange.GetMaxValue();
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
			SendErrorMessage(0, QString("Unable to execute migration file '%1'. Error: '%2'").arg(migrationFilePath).arg(sqlError.text()), "CMigrationControllerComp");

			return false;
		}

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
			max = imtdb::GetLastMigration(migrationFolder, errorMessage);
		}
	}

	m_range = istd::CIntRange(min, max);
}


} // namespace imtdb


