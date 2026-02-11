// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	istd::CIntRange availableRange;
	for (int i = 0; i < m_migrationControllersCompPtr.GetCount(); i++){
		const imtdb::IMigrationController* migrationControllerPtr = m_migrationControllersCompPtr[i];
		if (migrationControllerPtr != nullptr){
			istd::CIntRange range = migrationControllerPtr->GetMigrationRange();
			if (i == 0){
				availableRange = range;
			}
			else{
				availableRange.Unite(range);
			}
		}
	}

	return availableRange;
}


bool CCompositeMigrationControllerComp::DoMigration(int& resultRevision, const istd::CIntRange& subRange) const
{
	resultRevision = -1;

	istd::CIntRange composedRange = GetMigrationRange();

	istd::CIntRange inputRange = subRange;
	if (subRange.GetMinValue() < 0){
		inputRange.SetMinValue(composedRange.GetMinValue());
	}

	if (subRange.GetMaxValue() < 0){
		inputRange.SetMaxValue(composedRange.GetMaxValue());
	}

	if (!composedRange.IsValid() || !inputRange.IsValid()){
		return false;
	}

	if (!(	composedRange.GetMinValue() <= inputRange.GetMinValue() &&
			composedRange.GetMaxValue() >= inputRange.GetMaxValue())){
		return false;
	}

	MigrationSteps steps;

	for (int i = 0; i < m_migrationControllersCompPtr.GetCount(); i++){
		const imtdb::IMigrationController* migrationControllerPtr = m_migrationControllersCompPtr[i];
		if (migrationControllerPtr != nullptr){
			istd::CIntRange currentMigrationRange = migrationControllerPtr->GetMigrationRange();

			int startRevision = inputRange.GetMinValue();
			int stopRevision = inputRange.GetMaxValue();

			while (startRevision <= stopRevision){
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

	int currentRevision = -1;
	for (const MigrationStep& step : steps){
		if (!step.migrationControllerPtr->DoMigration(currentRevision, istd::CIntRange(step.from, step.to))){
			return false;
		}
	}

	resultRevision = currentRevision;

	return true;
}


} // namespace imtdb


