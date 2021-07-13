#include <imtdb/CDataBaseObjectCollection.h>


namespace imtdb
{


void CDataBaseObjectCollection::Refresh()
{
	QSqlQuery sqlQuery= this->ExecSelectSqlQuery();
	while (sqlQuery.next()){
		auto newObj = this->CreateObjectFromSqlRecord(sqlQuery.record());

		CDataBaseObjectCollection::CObjectCollectionBase::InsertNewObject("","","",newObj, newObj->GetID());
	}
}


QSqlQuery CDataBaseObjectCollection::ExecSelectSqlQuery(const QVariantMap& bindValues, QSqlError* sqlError) const
{
	return m_dbEngineCompPtr->ExecSqlQuery(this->GetQueryStringFromFile(*m_selectSqlQueryPathAttrPtr), bindValues, sqlError);
}


QSqlQuery CDataBaseObjectCollection::ExecUpdateSqlQuery(const QVariantMap& bindValues, QSqlError* sqlError) const
{
	QByteArray queryString = this->GetQueryStringFromFile(*m_updateSqlQueryPathAttrPtr);
	for(auto value = bindValues.cbegin(); value != bindValues.cend(); ++value){
		if(!value->isValid() || value->isNull()){
			CDataBaseEngineComp::DrectBindValueUpdateDefault(&queryString, value.key().toUtf8());
		}
	}

	return m_dbEngineCompPtr->ExecSqlQuery(queryString, bindValues, sqlError);
}


QSqlQuery CDataBaseObjectCollection::ExecInsertSqlQuery(const QVariantMap& bindValues, QSqlError* sqlError) const
{
	QByteArray queryString = this->GetQueryStringFromFile(*m_insertSqlQueryPathAttrPtr);
	for(auto value = bindValues.cbegin(); value != bindValues.cend(); ++value){
		if(!value->isValid() || value->isNull())
			CDataBaseEngineComp::DrectBindValueInsertDefault(&queryString, value.key().toUtf8());
	}

	return m_dbEngineCompPtr->ExecSqlQuery(queryString, bindValues, sqlError);
}


QSqlQuery CDataBaseObjectCollection::ExecDeleteSqlQuery(const QVariantMap& bindValues, QSqlError* sqlError) const
{
	return m_dbEngineCompPtr->ExecSqlQuery(this->GetQueryStringFromFile(*m_deleteSqlQueryPathAttrPtr), bindValues, sqlError);
}


const IDataBaseChangeable* const CDataBaseObjectCollection::GetObjectPtrById(const QByteArray& id) const
{
	const IDataBaseChangeable* retVal = nullptr;
	retVal = dynamic_cast<const IDataBaseChangeable*>(CObjectCollectionBase::GetObjectPtr(id));
	return retVal;
}


const IDataBaseChangeable* CDataBaseObjectCollection::CreateObjectFromSqlRecord(const QSqlRecord& record) const
{
	Q_ASSERT_X(0, Q_FUNC_INFO, "Not implemented method");

	return nullptr;
}


// reimplemented (imtbase::IObjectCollection)

QByteArray CDataBaseObjectCollection::InsertNewObject(
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


bool CDataBaseObjectCollection::RemoveObject(const QByteArray& objectId)
{
	Q_ASSERT_X(0, Q_FUNC_INFO, "Not implemented method");

	return false;
}


bool CDataBaseObjectCollection::SetObjectData(const QByteArray& objectId, const istd::IChangeable& object, CompatibilityMode mode)
{
	Q_ASSERT_X(0, Q_FUNC_INFO, "Not implemented method");

	return false;
}


istd::IChangeable* CDataBaseObjectCollection::CreateObjectInstance(const QByteArray& typeId) const
{
	Q_ASSERT_X(0, Q_FUNC_INFO, "Not implemented method");

	return nullptr;
}


QByteArray CDataBaseObjectCollection::GetQueryStringFromFile(const QByteArray& filePath) const
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
