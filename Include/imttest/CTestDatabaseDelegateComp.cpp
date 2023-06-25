#include <imttest/CTestDatabaseDelegateComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imttest/CTestMetaInfo.h>
#include <imtdb/CDatabaseEngineComp.h>


namespace imttest
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeable* CTestDatabaseDelegateComp::CreateObjectFromRecord(const QSqlRecord& record) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_testFactCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imttest::ITestInfo> testInfoPtr = m_testFactCompPtr.CreateInstance();
	if (!testInfoPtr.IsValid()){
		return nullptr;
	}

	if(record.contains("id")){
		QByteArray testId = record.value("Id").toByteArray();
		testInfoPtr->SetTestId(testId);
	}

	if (record.contains("Name")){
		QString testName = record.value("Name").toString();

		testInfoPtr->SetTestName(testName);
	}

	if (record.contains("Description")){
		QString testDescription = record.value("Description").toString();

		testInfoPtr->SetTestDescription(testDescription);
	}

	return testInfoPtr.PopPtr();
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CTestDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& typeId,
			const QByteArray& proposedObjectId,
			const QString& objectName,
			const QString& objectDescription,
			const istd::IChangeable* valuePtr,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	if (typeId.isEmpty() || typeId != "TestInfo"){
		return NewObjectQuery();
	}
	QByteArray testId = proposedObjectId;
	if (!proposedObjectId.isEmpty()){
		QByteArray checkExistQuery = GetSelectionQuery(proposedObjectId);
		QSqlError sqlError;
		QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(checkExistQuery, &sqlError);
		if (sqlError.type() == QSqlError::NoError && sqlQuery.size() == 0){
			testId = proposedObjectId;
		}
		else{
			testId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
		}
	}
	QString testName;
	QString testDescription;
	const imttest::ITestInfo* testInfoPtr = dynamic_cast<const imttest::ITestInfo*>(valuePtr);
	if (testInfoPtr != nullptr){
		testName = testInfoPtr->GetTestName();
		testDescription = testInfoPtr->GetTestDescription();

	}
	if (testName.isEmpty()){
		testName = objectName;
	}
	if (testDescription.isEmpty()){
		testDescription = objectDescription;
	}

	NewObjectQuery retVal;

	retVal.query = QString("INSERT INTO \"Tests\"(Id, Name, Description) VALUES('%1', '%2', '%3');")
			.arg(qPrintable(testId))
			.arg(testName)
			.arg(testDescription)
			.toLocal8Bit();

	if (testName.isEmpty()){
		if(testId.isEmpty()){
			return NewObjectQuery();
		}
		else{
			retVal.objectName = QString(testId);
		}
	}
	else{
		retVal.objectName = testName;
	}
	return retVal;
}


QByteArray CTestDatabaseDelegateComp::CreateDeleteObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		const imttest::ITestInfo* testInfoPtr = dynamic_cast<const imttest::ITestInfo*>(objectPtr.GetPtr());
		if (testInfoPtr == nullptr){
			return QByteArray();
		}

		QByteArray retVal = QString("DELETE FROM \"Tests\" WHERE Id = '%1';").arg(qPrintable(objectId)).toLocal8Bit();

		return retVal;
	}

	return QByteArray();
}


QByteArray CTestDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* /*operationContextPtr*/,
			bool /*useExternDelegate*/) const
{
	const imttest::ITestInfo* testInfoPtr = dynamic_cast<const imttest::ITestInfo*>(&object);
	if (testInfoPtr == nullptr){
		return QByteArray();
	}

	QString testName = testInfoPtr->GetTestName();

	if (testName.isEmpty()){
		return QByteArray();
	}
	QByteArray testId = testInfoPtr->GetTestId();
	QByteArray retVal = QString("UPDATE \"Tests\" SET Id = '%1' Name = '%2' WHERE Id ='%3';")
							.arg(qPrintable(testId))
							.arg(testName)
							.arg(qPrintable(objectId))
							.toLocal8Bit();

	return retVal;
}


QByteArray CTestDatabaseDelegateComp::CreateRenameObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const QString& newObjectName,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	const imttest::ITestInfo* testInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		testInfoPtr = dynamic_cast<const imttest::ITestInfo*>(objectPtr.GetPtr());
	}

	if (testInfoPtr == nullptr){
		return QByteArray();
	}

	if (objectId.isEmpty()){
		return QByteArray();
	}

	QByteArray retVal = QString("UPDATE \"Tests\" SET Name = '%1' WHERE Id = '%2';")
			.arg(newObjectName)
			.arg(qPrintable(objectId)).toLocal8Bit();

	return retVal;
}


QByteArray CTestDatabaseDelegateComp::CreateDescriptionObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const QString& description,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	const imttest::ITestInfo* testPtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		testPtr = dynamic_cast<const imttest::ITestInfo*>(objectPtr.GetPtr());
	}

	if (testPtr == nullptr){
		return QByteArray();
	}

	QByteArray testId = testPtr->GetTestId();
	if (testId.isEmpty()){
		return QByteArray();
	}

	QByteArray retVal = QString("UPDATE \"Tests\" SET Description = '%1' WHERE Id ='%2';")
			.arg(description)
			.arg(qPrintable(testId)).toLocal8Bit();

	return retVal;
}


// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)

idoc::MetaInfoPtr CTestDatabaseDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	return  idoc::MetaInfoPtr(new imttest::CTestMetaInfo());
}


bool CTestDatabaseDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("Id")){
		QString testId = record.value("Id").toString();

		metaInfo.SetMetaInfo(imttest::ITestInfo::MIT_TEST_ID, testId);
	}

	if (record.contains("Name")){
		QString testName = record.value("Name").toString();

		metaInfo.SetMetaInfo(imttest::ITestInfo::MIT_TEST_NAME, testName);
	}

	if (record.contains("Description")){
		QString testDescription = record.value("Description").toString();

		metaInfo.SetMetaInfo(imttest::ITestInfo::MIT_TEST_DESCRIPTION, testDescription);
	}

	return true;
}

QByteArray CTestDatabaseDelegateComp::CreateDataMetaInfoQuery(const imtbase::IObjectCollection &collection, const QByteArray &objectId, const idoc::IDocumentMetaInfo *dataMetaInfoPtr) const
{
	if(dataMetaInfoPtr != nullptr && !objectId.isEmpty()){
		QString name = dataMetaInfoPtr->GetMetaInfo(imttest::ITestInfo::MIT_TEST_NAME).toString();
		QString description = dataMetaInfoPtr->GetMetaInfo(imttest::ITestInfo::MIT_TEST_DESCRIPTION).toString();
		QDateTime added = dataMetaInfoPtr->GetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME).toDateTime();
		QString str_Added = added.isValid() ? added.toString("yyyy-MM-dd hh:mm:ss") : "null";
		QDateTime lastModificationTime = dataMetaInfoPtr->GetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME).toDateTime();
		QString str_lastModificationTime = lastModificationTime.isValid() ? lastModificationTime.toString("yyyy-MM-dd hh:mm:ss") : "null";
		QByteArray retVal = QString("UPDATE \"Tests\" SET Name = '%1', Description = '%2', Added = %3, LastModified = %4 WHERE Id ='%5';")
								.arg(name)
								.arg(description)
								.arg(str_Added)
								.arg(str_lastModificationTime)
								.arg(qPrintable(objectId)).toLocal8Bit();

		return retVal;
	}
	return QByteArray();
}

QByteArray CTestDatabaseDelegateComp::CreateCollectionItemMetaInfoQuery(const imtbase::IObjectCollection &collection, const QByteArray &objectId, const idoc::IDocumentMetaInfo *collectionItemMetaInfoPtr) const
{
	if(collectionItemMetaInfoPtr != nullptr && !objectId.isEmpty()){
		QString name = collectionItemMetaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_TITLE).toString();
		QString description = collectionItemMetaInfoPtr->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_DESCRIPTION).toString();
		QDateTime added = collectionItemMetaInfoPtr->GetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME).toDateTime();
		QString str_Added = added.isValid() ? added.toString("yyyy-MM-dd hh:mm:ss") : "null";
		QDateTime lastModificationTime = collectionItemMetaInfoPtr->GetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME).toDateTime();
		QString str_lastModificationTime = lastModificationTime.isValid() ? lastModificationTime.toString("yyyy-MM-dd hh:mm:ss") : "null";
		QByteArray retVal = QString("UPDATE \"Tests\" SET Name = '%1', Description = '%2', Added = %3, LastModified = %4 WHERE Id ='%5';")
								.arg(name)
								.arg(description)
								.arg(str_Added)
								.arg(str_lastModificationTime)
								.arg(qPrintable(objectId)).toLocal8Bit();

		return retVal;
	}
	return QByteArray();
}




} // namespace imttest


