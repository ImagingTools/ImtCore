#include <imtdb/CDatabaseObjectCollectionComp.h>


// Qt includes
#include <QtSql/QSqlQuery>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtdb/CDatabaseEngineComp.h>


namespace imtdb
{


// public methods

// reimplemented (IObjectCollectionInfo)

const iprm::IOptionsList* CDatabaseObjectCollectionComp::GetObjectTypesInfo() const
{
	return &m_typesInfo;
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
	if (!m_objectDelegateCompPtr.IsValid()){
		return nullptr;
	}

	QByteArray objectId = proposedObjectId;
	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}

	QByteArray query = m_objectDelegateCompPtr->CreateNewObjectQuery(typeId, objectId, name, description, defaultValuePtr);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return nullptr;
	}

	if (ExecuteTransaction(query)){
		istd::CChangeNotifier changeNotifier(this);

		idoc::CStandardDocumentMetaInfo collectionMetaInfo;

		if (collectionItemMetaInfoPtr == nullptr){
			collectionMetaInfo.SetMetaInfo(IObjectCollection::MIT_INSERTION_TIME, QDateTime::currentDateTime());
			collectionMetaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME, QDateTime::currentDateTime());

			collectionItemMetaInfoPtr = &collectionMetaInfo;
		}

		imtbase::IMetaInfoCreator::MetaInfoPtr metaInfoPtr;

		if (dataMetaInfoPtr == nullptr){
			if (m_metaInfoCreatorCompPtr.IsValid() && (defaultValuePtr != nullptr)){
				if (m_metaInfoCreatorCompPtr->CreateMetaInfo(defaultValuePtr, *m_typeIdAttrPtr, metaInfoPtr)){
					dataMetaInfoPtr = metaInfoPtr.GetPtr();
				}
			}
		}

		QByteArray internalObjectId = BaseClass2::InsertNewObject(typeId, name, description, defaultValuePtr, objectId, dataMetaInfoPtr, collectionItemMetaInfoPtr);
		Q_ASSERT(objectId == internalObjectId);

		return objectId;
	}

	return QByteArray();
}


bool CDatabaseObjectCollectionComp::RemoveObject(const QByteArray& objectId)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		return false;
	}

	QByteArray query = m_objectDelegateCompPtr->CreateDeleteObjectQuery(*this, objectId);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return false;
	}

	if (ExecuteTransaction(query)){
		istd::CChangeNotifier changeNotifier(this);

		BaseClass2::RemoveObject(objectId);

		return true;
	}

	return false;
}


bool CDatabaseObjectCollectionComp::SetObjectData(
			const QByteArray& objectId,
			const istd::IChangeable& object,
			CompatibilityMode mode)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		return false;
	}

	QByteArray query = m_objectDelegateCompPtr->CreateUpdateObjectQuery(*this, objectId, object);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return false;
	}

	if (ExecuteTransaction(query)){
		istd::CChangeNotifier changeNotifier(this);

		BaseClass2::SetObjectData(objectId, object, mode);

		return true;
	}

	return false;
}


void CDatabaseObjectCollectionComp::SetObjectName(const QByteArray& objectId, const QString& objectName)
{
	if (!m_objectDelegateCompPtr.IsValid()){
		return;
	}

	QByteArray query = m_objectDelegateCompPtr->CreateRenameObjectQuery(*this, objectId, objectName);
	if (query.isEmpty()){
		SendErrorMessage(0, "Database query could not be created", "Database collection");

		return;
	}

	if (ExecuteTransaction(query)){
		istd::CChangeNotifier changeNotifier(this);

		BaseClass2::SetObjectName(objectId, objectName);
	}
}


  // protected methods

bool CDatabaseObjectCollectionComp::ExecuteTransaction(const QByteArray& sqlQuery) const
{
	QStringList queryList = QString(qPrintable(sqlQuery)).split(";");

	m_dbEngineCompPtr->BeginTransaction();

	for (const QString& singleQuery: queryList){
		if (!singleQuery.isEmpty()){
			QSqlError error;
			m_dbEngineCompPtr->ExecSqlQuery(singleQuery.toLocal8Bit(), &error);
			if (error.type() != QSqlError::NoError){
				SendErrorMessage(0, error.text(), "Database collection");

				m_dbEngineCompPtr->CancelTransaction();

				return false;
			}
		}
	}

	m_dbEngineCompPtr->FinishTransaction();

	return true;
}


void CDatabaseObjectCollectionComp::CreateCollectionFromDatabase()
{
	istd::CChangeGroup changeGroup(this);

	ResetData();

	QSqlQuery sqlQuery = ExecSelectSqlQuery();
	while (sqlQuery.next()){
		QString name;
		QString description;
		QDateTime added;
		QDateTime lastModified;

		istd::IChangeable* objectPtr = CreateObjectFromSqlRecord(sqlQuery.record(), name, description, lastModified, added);
		if (objectPtr != nullptr){
			idoc::CStandardDocumentMetaInfo collectionMetaInfo;
			collectionMetaInfo.SetMetaInfo(IObjectCollection::MIT_INSERTION_TIME, added);
			collectionMetaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME, lastModified);

			imtbase::IMetaInfoCreator::MetaInfoPtr metaInfoPtr;
			if (m_metaInfoCreatorCompPtr.IsValid()){
				m_metaInfoCreatorCompPtr->CreateMetaInfo(objectPtr, *m_typeIdAttrPtr, metaInfoPtr);
			}

			BaseClass2::InsertNewObject(*m_typeIdAttrPtr, name, description, objectPtr, "", metaInfoPtr.GetPtr(), &collectionMetaInfo);
		}
	}
}


istd::IChangeable* CDatabaseObjectCollectionComp::CreateObjectFromSqlRecord(
			const QSqlRecord& record,
			QString& objectName,
			QString& objectDescription,
			QDateTime& lastModified,
			QDateTime& added) const
{
	if (m_objectDelegateCompPtr.IsValid()){
		return m_objectDelegateCompPtr->CreateObjectFromRecord(*m_typeIdAttrPtr, record, objectName, objectDescription, lastModified, added);
	}

	return nullptr;
}


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


// reimplemented (imtbase::CObjectCollectionBase)

istd::IChangeable* CDatabaseObjectCollectionComp::CreateObjectInstance(const QByteArray& /*typeId*/) const
{
	if (m_objectFactoryCompPtr.IsValid()){
		return m_objectFactoryCompPtr.CreateInstance();
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

	m_typesInfo.InsertOption(*m_typeNameAttrPtr, *m_typeIdAttrPtr);

	CreateCollectionFromDatabase();
}


} // namespace imtdb


