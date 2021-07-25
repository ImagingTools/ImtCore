#include <imtdb/CDatabaseObjectCollectionComp.h>


// ImtCore includes
#include <imtdb/CDatabaseEngineComp.h>


namespace imtdb
{


QSqlQuery CDatabaseObjectCollectionComp::ExecSelectSqlQuery(const QVariantMap& bindValues, QSqlError* sqlError) const
{
	if (m_dbEngineCompPtr.IsValid()){
		return m_dbEngineCompPtr->ExecSqlQuery(GetQueryStringFromFile(*m_selectSqlQueryPathAttrPtr), bindValues, sqlError);
	}

	return QSqlQuery();
}


QSqlQuery CDatabaseObjectCollectionComp::ExecUpdateSqlQuery(const QVariantMap& bindValues, QSqlError* sqlError) const
{
	QByteArray queryString = GetQueryStringFromFile(*m_updateSqlQueryPathAttrPtr);

	for(auto value = bindValues.cbegin(); value != bindValues.cend(); ++value){
		if(!value->isValid() || value->isNull()){
			CDatabaseEngineComp::DrectBindValueUpdateDefault(&queryString, value.key().toUtf8());
		}
	}

	return m_dbEngineCompPtr->ExecSqlQuery(queryString, bindValues, sqlError);
}


QSqlQuery CDatabaseObjectCollectionComp::ExecInsertSqlQuery(const QVariantMap& bindValues, QSqlError* sqlError) const
{
	QByteArray queryString = GetQueryStringFromFile(*m_insertSqlQueryPathAttrPtr);

	for(auto value = bindValues.cbegin(); value != bindValues.cend(); ++value){
		if(!value->isValid() || value->isNull()){
			CDatabaseEngineComp::DrectBindValueInsertDefault(&queryString, value.key().toUtf8());
		}
	}

	return m_dbEngineCompPtr->ExecSqlQuery(queryString, bindValues, sqlError);
}


QSqlQuery CDatabaseObjectCollectionComp::ExecDeleteSqlQuery(const QVariantMap& bindValues, QSqlError* sqlError) const
{
	return m_dbEngineCompPtr->ExecSqlQuery(GetQueryStringFromFile(*m_deleteSqlQueryPathAttrPtr), bindValues, sqlError);
}


// protected methods

void CDatabaseObjectCollectionComp::SyncWithDatabase()
{
	ResetData();

	QSqlQuery sqlQuery = ExecSelectSqlQuery();
	while (sqlQuery.next()){
		istd::IChangeable* objectPtr = CreateObjectFromSqlRecord(sqlQuery.record());
		if (objectPtr != nullptr){
			BaseClass2::InsertNewObject("", "", "", objectPtr);
		}
	}
}


istd::IChangeable* CDatabaseObjectCollectionComp::CreateObjectFromSqlRecord(const QSqlRecord& record) const
{
	if (m_objectDelegateCompPtr.IsValid()){
		return m_objectDelegateCompPtr->CreateObjectFromRecord("", record);
	}

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


// reimplemented (CObjectCollectionBase)

istd::IChangeable* CDatabaseObjectCollectionComp::CreateObjectInstance(const QByteArray& typeId) const
{
	int factoryIndex = m_typeIdsAttrPtr.FindValue(typeId);
	if (factoryIndex >= 0){
		if (factoryIndex < m_objectFactoriesCompPtr.GetCount()){
			return m_objectFactoriesCompPtr.CreateInstance(factoryIndex);
		}
	}

	return nullptr;
}


void CDatabaseObjectCollectionComp::DestroyObjectInstance(istd::IChangeable* objectPtr) const
{
	if (objectPtr != nullptr){
		icomp::IComponent* componentPtr = dynamic_cast<icomp::IComponent*>(objectPtr);
		if (componentPtr != nullptr){
			const icomp::ICompositeComponent* parentComponentPtr = nullptr;
			while ((parentComponentPtr = componentPtr->GetParentComponent(true)) != nullptr){
				componentPtr = const_cast<icomp::ICompositeComponent*>(parentComponentPtr);
			}

			if (componentPtr != nullptr){
				delete componentPtr;
			}
		}
		else{
			delete objectPtr;
		}
	}
}



// reimplemented (icomp::CComponentBase)

void CDatabaseObjectCollectionComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	SyncWithDatabase();
}


} // namespace imtdb


