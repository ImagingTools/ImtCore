#include <imtdb/CDatabaseObjectCollectionComp.h>

// ImtCore includes
#include <imtdb/CDatabaseEngineComp.h>


namespace imtdb
{


void CDatabaseObjectCollectionComp::Refresh()
{
	ResetData();

	QSqlQuery sqlQuery= this->ExecSelectSqlQuery();
	while (sqlQuery.next()){
		istd::IChangeable* objectPtr = this->CreateObjectFromSqlRecord(sqlQuery.record());
		if (objectPtr != nullptr){
			BaseClass2::InsertNewObject("", "", "", objectPtr);
		}
	}
}


QSqlQuery CDatabaseObjectCollectionComp::ExecSelectSqlQuery(const QVariantMap& bindValues, QSqlError* sqlError) const
{
	return m_dbEngineCompPtr->ExecSqlQuery(this->GetQueryStringFromFile(*m_selectSqlQueryPathAttrPtr), bindValues, sqlError);
}


QSqlQuery CDatabaseObjectCollectionComp::ExecUpdateSqlQuery(const QVariantMap& bindValues, QSqlError* sqlError) const
{
	QByteArray queryString = this->GetQueryStringFromFile(*m_updateSqlQueryPathAttrPtr);
	for(auto value = bindValues.cbegin(); value != bindValues.cend(); ++value){
		if(!value->isValid() || value->isNull()){
			CDatabaseEngineComp::DrectBindValueUpdateDefault(&queryString, value.key().toUtf8());
		}
	}

	return m_dbEngineCompPtr->ExecSqlQuery(queryString, bindValues, sqlError);
}


QSqlQuery CDatabaseObjectCollectionComp::ExecInsertSqlQuery(const QVariantMap& bindValues, QSqlError* sqlError) const
{
	QByteArray queryString = this->GetQueryStringFromFile(*m_insertSqlQueryPathAttrPtr);
	for(auto value = bindValues.cbegin(); value != bindValues.cend(); ++value){
		if(!value->isValid() || value->isNull())
			CDatabaseEngineComp::DrectBindValueInsertDefault(&queryString, value.key().toUtf8());
	}

	return m_dbEngineCompPtr->ExecSqlQuery(queryString, bindValues, sqlError);
}


QSqlQuery CDatabaseObjectCollectionComp::ExecDeleteSqlQuery(const QVariantMap& bindValues, QSqlError* sqlError) const
{
	return m_dbEngineCompPtr->ExecSqlQuery(this->GetQueryStringFromFile(*m_deleteSqlQueryPathAttrPtr), bindValues, sqlError);
}


const IDatabaseChangeable* const CDatabaseObjectCollectionComp::GetObjectPtrById(const QByteArray& id) const
{
	const IDatabaseChangeable* retVal = nullptr;
	retVal = dynamic_cast<const IDatabaseChangeable*>(CObjectCollectionBase::GetObjectPtr(id));
	return retVal;
}


istd::IChangeable* CDatabaseObjectCollectionComp::CreateObjectFromSqlRecord(const QSqlRecord& record) const
{
	Q_ASSERT_X(0, Q_FUNC_INFO, "Not implemented method");

	return nullptr;
}


// reimplemented (imtbase::IObjectCollection)

QByteArray CDatabaseObjectCollectionComp::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			const istd::IChangeable* defaultValuePtr,
			const QByteArray& proposedObjectId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr)
{
	Q_ASSERT_X(0, Q_FUNC_INFO, "Not implemented method");

	return nullptr;
}


bool CDatabaseObjectCollectionComp::RemoveObject(const QByteArray& objectId)
{
	Q_ASSERT_X(0, Q_FUNC_INFO, "Not implemented method");

	return false;
}


bool CDatabaseObjectCollectionComp::SetObjectData(const QByteArray& objectId, const istd::IChangeable& object, CompatibilityMode mode)
{
	Q_ASSERT_X(0, Q_FUNC_INFO, "Not implemented method");

	return false;
}


istd::IChangeable* CDatabaseObjectCollectionComp::CreateObjectInstance(const QByteArray& typeId) const
{
	Q_ASSERT_X(0, Q_FUNC_INFO, "Not implemented method");

	return nullptr;
}


QByteArray CDatabaseObjectCollectionComp::GetQueryStringFromFile(const QByteArray& filePath) const
{
	QByteArray retVal;
	QFile queryFile(filePath);
	if(!queryFile.open(QFile::ReadOnly)){
		qCritical() << __FILE__ << __LINE__
					<< "\n\t| what(): Unable to open file"
					<< "\n\t| fileName(): " << queryFile.fileName()
					<< "\n\t| path(): " << filePath
					   ;
	}
	else {
		retVal = queryFile.readAll();

		queryFile.close();
	}

	return retVal;
}




} // namespace imod
