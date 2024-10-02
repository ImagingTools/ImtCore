#include <imtdb/CCompositeMigrationControllerComp.h>


// Qt includes
#include <QtSql/QSqlError>
#include <QtCore/QDir>
#include <QtCore/QRegularExpression>

// ImtCore includes
#include <imtdb/imtdb.h>


namespace imtdb
{


// protected methods

const imtdb::IMigrationController* CCompositeMigrationControllerComp::FindMigrationController(int migrationIndex) const
{
	for (int i = 0; i < m_migrationControllersCompPtr.GetCount(); i++){
		const imtdb::IMigrationController* migrationControllerPtr = m_migrationControllersCompPtr[i];
		if (migrationControllerPtr != nullptr){
			istd::CIntRange range = migrationControllerPtr->GetMigrationRange();
			if (range.Contains(migrationIndex)){
				return migrationControllerPtr;
			}
		}
	}

	return nullptr;
}


// reimplemented (imtdb::IMigrationController)

istd::CIntRange CCompositeMigrationControllerComp::GetMigrationRange() const
{
	return istd::CIntRange();
}


bool CCompositeMigrationControllerComp::DoMigration(const istd::CIntRange& subRange) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return false;
	}

	if (!m_migrationFolderPathCompPtr.IsValid()){
		return false;
	}

	QString migrationFolder = m_migrationFolderPathCompPtr->GetPath();

	int min = subRange.GetMinValue();
	int max = subRange.GetMaxValue();

	if (min < 0){
		min = 0;
	}

	if (max < 0){
		QString errorMessage;
		int lastMigration = imtdb::GetLastMigration(migrationFolder, errorMessage);
		if (lastMigration < 0){
			SendErrorMessage(0, errorMessage, "CCompositeMigrationControllerComp");

			return false;
		}

		max = lastMigration;
	}

	istd::CIntRange inputRange(min, max);

	istd::CIntRange availableRange(0, 0);
	for (int i = 0; i < m_migrationControllersCompPtr.GetCount(); i++){
		const imtdb::IMigrationController* migrationControllerPtr = m_migrationControllersCompPtr[i];
		if (migrationControllerPtr != nullptr){
			istd::CIntRange range = migrationControllerPtr->GetMigrationRange();
			availableRange.Unite(range);
		}
	}

	if (!availableRange.Contains(inputRange)){
		return false;
	}

	MigrationSteps steps;

	for (int i = 0; i < m_migrationControllersCompPtr.GetCount(); i++){
		const imtdb::IMigrationController* migrationControllerPtr = m_migrationControllersCompPtr[i];
		if (migrationControllerPtr != nullptr){
			istd::CIntRange currentMigrationRange = migrationControllerPtr->GetMigrationRange();

			int startRevision = inputRange.GetMinValue();
			int stopRevision = inputRange.GetMaxValue();

			while (startRevision != stopRevision){
				istd::CIntRange checkRange(startRevision, stopRevision);

				if (currentMigrationRange.Contains(checkRange)){
					MigrationStep step;
					step.from = checkRange.GetMinValue();
					step.to = checkRange.GetMaxValue();
					step.migrationControllerPtr = migrationControllerPtr;

					steps.push_back(step);

					inputRange.SetMinValue(checkRange.GetMaxValue() + 1);

					break;
				}

				stopRevision--;
			}
		}
	}

	m_databaseEngineCompPtr->BeginTransaction();

	int maxStepTo = -1;
	for (const MigrationStep& step : steps){
		if (!step.migrationControllerPtr->DoMigration(istd::CIntRange(step.from, step.to))){
			m_databaseEngineCompPtr->CancelTransaction();
			return false;
		}

		// Calc max revision
		if (maxStepTo < step.to){
			maxStepTo = step.to;
		}
	}

	m_databaseEngineCompPtr->FinishTransaction();

	// Set max revision to database
	if (maxStepTo > 0){
		QDir folder(m_migrationFolderPathCompPtr->GetPath());

		QSqlError sqlError;
		QVariantMap valuesRevision;
		valuesRevision.insert(":Revision", maxStepTo);
		m_databaseEngineCompPtr->ExecSqlQueryFromFile(folder.filePath("SetRevision.sql"), valuesRevision, &sqlError);
		if (sqlError.type() != QSqlError::NoError){
			SendErrorMessage(0, QString("Execution of SetRevision.sql failed: '%1'").arg(sqlError.text()), "CCompositeMigrationControllerComp");
			m_databaseEngineCompPtr->CancelTransaction();

			return false;
		}
	}

	return true;
}


} // namespace imtdb


