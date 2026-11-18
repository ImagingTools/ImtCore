// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CObjectCollectionImportMigrationControllerComp.h>


// Qt includes
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

// ACF includes
#include <ilog/CMessageContainer.h>


namespace imtdb
{


// protected methods

// reimplemented (imtdb::IMigrationController)

bool CObjectCollectionImportMigrationControllerComp::DoMigration(int& resultRevision, const istd::CIntRange& subRange) const
{
	resultRevision = -1;

	if (!m_databaseEngineCompPtr.IsValid() || !m_sourceCollectionCompPtr.IsValid() || !m_targetDelegateCompPtr.IsValid()){
		return false;
	}

	if (!subRange.IsValid() || !m_range.Contains(subRange)){
		return false;
	}

	const QString tableName = QString::fromUtf8(m_targetDelegateCompPtr->GetTableName());

	if (m_sourceEnablerCompPtr.IsValid() && !m_sourceEnablerCompPtr->IsEnabled()){
		SendInfoMessage(0, QStringLiteral("The import into '%1' is switched off, nothing is imported").arg(tableName));

		resultRevision = subRange.GetMaxValue();

		return true;
	}

	// An incomplete read must not pass for "nothing to import": the revision would be recorded and the objects never come
	ilog::CMessageContainer readLog;
	const imtbase::ICollectionInfo::Ids objectIds = m_sourceCollectionCompPtr->GetElementIds(0, -1, nullptr, &readLog);
	if (readLog.GetWorstCategory() >= istd::IInformationProvider::IC_ERROR){
		SendErrorMessage(0, QStringLiteral("The objects to import into '%1' cannot be read from the source collection").arg(tableName));

		return false;
	}

	int importedCount = 0;
	for (const QByteArray& objectId : objectIds){
		bool isImported = false;
		if (!ImportObject(objectId, isImported)){
			return false;
		}

		if (isImported){
			importedCount++;
		}
	}

	resultRevision = subRange.GetMaxValue();

	SendInfoMessage(
				0,
				QStringLiteral("%1 of %2 objects of the source collection imported into '%3'").arg(importedCount).arg(objectIds.count()).arg(tableName),
				"CObjectCollectionImportMigrationControllerComp");

	return true;
}


// private methods

bool CObjectCollectionImportMigrationControllerComp::IsInTarget(const QByteArray& objectId, bool& isInTarget) const
{
	isInTarget = false;

	QSqlError sqlError;
	QSqlQuery query = m_databaseEngineCompPtr->ExecSqlQuery(m_targetDelegateCompPtr->GetSelectionQuery(objectId), &sqlError, true);
	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, QStringLiteral("The object '%1' cannot be looked up in '%2': %3")
					.arg(QString::fromUtf8(objectId), QString::fromUtf8(m_targetDelegateCompPtr->GetTableName()), sqlError.text()));

		return false;
	}

	isInTarget = query.next();

	return true;
}


bool CObjectCollectionImportMigrationControllerComp::ImportObject(const QByteArray& objectId, bool& isImported) const
{
	isImported = false;

	const QString tableName = QString::fromUtf8(m_targetDelegateCompPtr->GetTableName());

	bool isInTarget = false;
	if (!IsInTarget(objectId, isInTarget)){
		return false;
	}

	if (isInTarget){
		return true;
	}

	imtbase::IObjectCollection::DataPtr sourceDataPtr;
	if (!m_sourceCollectionCompPtr->GetObjectData(objectId, sourceDataPtr) || !sourceDataPtr.IsValid()){
		SendErrorMessage(0, QStringLiteral("The object '%1' to import into '%2' cannot be read").arg(QString::fromUtf8(objectId), tableName));

		return false;
	}

	IObjectImportConverter::TargetObject targetObject;
	targetObject.typeId = m_sourceCollectionCompPtr->GetObjectTypeId(objectId);
	targetObject.name = m_sourceCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
	targetObject.description = m_sourceCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();

	const istd::IChangeable* targetDataPtr = sourceDataPtr.GetPtr();

	if (m_converterCompPtr.IsValid()){
		QString errorMessage;
		const IObjectImportConverter::ConversionResult result = m_converterCompPtr->ConvertObject(objectId, *sourceDataPtr.GetPtr(), targetObject, errorMessage);
		if (result == IObjectImportConverter::CR_SKIPPED){
			return true;
		}

		if (result == IObjectImportConverter::CR_FAILED){
			SendErrorMessage(0, QStringLiteral("The object '%1' cannot be converted for '%2': %3").arg(QString::fromUtf8(objectId), tableName, errorMessage));

			return false;
		}

		if (targetObject.objectPtr.IsValid()){
			targetDataPtr = targetObject.objectPtr.GetPtr();
		}
	}

	const IDatabaseObjectDelegate::NewObjectQuery insertQuery = m_targetDelegateCompPtr->CreateNewObjectQuery(
				targetObject.typeId,
				objectId,
				targetObject.name,
				targetObject.description,
				targetDataPtr,
				nullptr);
	if (insertQuery.query.isEmpty()){
		SendErrorMessage(0, QStringLiteral("The object '%1' cannot be written into '%2'").arg(QString::fromUtf8(objectId), tableName));

		return false;
	}

	QSqlError sqlError;
	if (insertQuery.bindValues.isEmpty()){
		m_databaseEngineCompPtr->ExecSqlQuery(insertQuery.query, &sqlError);
	}
	else{
		m_databaseEngineCompPtr->ExecSqlQuery(insertQuery.query, insertQuery.bindValues, &sqlError);
	}

	if (sqlError.type() != QSqlError::NoError){
		SendErrorMessage(0, QStringLiteral("The object '%1' cannot be written into '%2': %3").arg(QString::fromUtf8(objectId), tableName, sqlError.text()));

		return false;
	}

	isImported = true;

	return true;
}


} // namespace imtdb


