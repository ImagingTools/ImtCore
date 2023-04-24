#include <imtmongo/CMongoDatabaseObjectCollectionComp.h>

// mongocxx includes
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

// Qt includes
//#include <QtSql/QSqlQuery>
#include <QtCore/QSharedPointer>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/CParamsSetJoiner.h>
//#include <imtdb/CDatabaseEngineComp.h>
#include <imtbase/CObjectCollection.h>
//#include <imtdb/CSqlDatabaseObjectCollectionIterator.h>


namespace imtmongo
{


// public methods

CMongoDatabaseObjectCollectionComp::CMongoDatabaseObjectCollectionComp()
	:m_filterParamsObserver(*this),
	m_databaseAccessObserver(*this),
	m_isInitialized(false)
{
}


// reimplemented (imtbase::IObjectCollection)

const imtbase::IRevisionController* CMongoDatabaseObjectCollectionComp::GetRevisionController() const
{
	return nullptr;
}


const imtbase::ICollectionDataController* CMongoDatabaseObjectCollectionComp::GetDataController() const
{
	return m_collectionDataControllerCompPtr.GetPtr();
}


int CMongoDatabaseObjectCollectionComp::GetOperationFlags(const QByteArray& /*objectId*/) const
{
	return OF_ALL;
}


imtbase::ICollectionInfo::Id CMongoDatabaseObjectCollectionComp::InsertNewBranch(
			const Id& /*parentId*/,
			const QString& /*name*/,
			const QString& /*description*/,
			const Id& /*proposedElementId*/,
			const idoc::IDocumentMetaInfo* /*elementMetaInfoPtr*/)
{
	return Id();
}


QByteArray CMongoDatabaseObjectCollectionComp::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			DataPtr defaultValuePtr,
			const QByteArray& proposedObjectId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr,
			const Id& /*parentId*/)
{


	QByteArray objectId = proposedObjectId;
	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}



//	else {
//		changeNotifier.Abort();
//	}

	return QByteArray();
}


bool CMongoDatabaseObjectCollectionComp::RemoveElement(const Id& elementId)
{



//	if (ExecuteTransaction(query)){
//		return true;
//	}
//	else{
//		changeNotifier.Abort();
//	}

	return false;
}


const istd::IChangeable* CMongoDatabaseObjectCollectionComp::GetObjectPtr(const QByteArray& /*objectId*/) const
{
	return nullptr;
}


bool CMongoDatabaseObjectCollectionComp::GetObjectData(const QByteArray& objectId, DataPtr& dataPtr) const
{


	if (objectId.isEmpty()){
		return false;
	}


//	istd::IChangeable* dataObjPtr = m_objectDelegateCompPtr->CreateObjectFromRecord(sqlQuery.record());
//	dataPtr = DataPtr(DataPtr::RootObjectPtr(dataObjPtr), [dataObjPtr](){
//		return dataObjPtr;
//	});

	return dataPtr.IsValid();
}


bool CMongoDatabaseObjectCollectionComp::SetObjectData(
			const Id& objectId,
			const istd::IChangeable& object,
			CompatibilityMode /*mode*/)
{



//	if (ExecuteTransaction(query)){
//		return true;
//	}
//	else{
//		changeNotifier.Abort();
//	}

	return false;
}


imtbase::IObjectCollection* CMongoDatabaseObjectCollectionComp::CreateSubCollection(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			const Id& /*parentId*/,
			int /*iterationFlags*/) const
{
	imtbase::IObjectCollection* collectionPtr = m_objectCollectionFactoryCompPtr.CreateInstance();
	imtbase::CParamsSetJoiner filterParams(selectionParamsPtr, m_filterParamsCompPtr.GetPtr());

//	if (m_objectDelegateCompPtr.IsValid()) {
//		QByteArray objectSelectionQuery = m_objectDelegateCompPtr->GetSelectionQuery(QByteArray(), offset, count, &filterParams);
//		if (objectSelectionQuery.isEmpty()) {
//			return nullptr;
//		}

//		QSqlError sqlError;
//		QSqlQuery sqlQuery = m_dbEngineCompPtr->ExecSqlQuery(objectSelectionQuery, &sqlError, true);

//		while (sqlQuery.next()) {
//			istd::IChangeable* dataObjPtr = m_objectDelegateCompPtr->CreateObjectFromRecord(sqlQuery.record());
//			DataPtr dataPtr = DataPtr(DataPtr::RootObjectPtr(dataObjPtr), [dataObjPtr]() {
//				return dataObjPtr;
//				});

//			QByteArray objectId = m_objectDelegateCompPtr->GetObjectIdFromRecord(sqlQuery.record());

//			QByteArray typeId = m_objectDelegateCompPtr->GetObjectTypeId(objectId);

//			collectionPtr->InsertNewObject(typeId, "", "", dataPtr, objectId);
//		}
//	}
	return collectionPtr;
}




// reimplemented (IObjectCollectionInfo)

const iprm::IOptionsList* CMongoDatabaseObjectCollectionComp::GetObjectTypesInfo() const
{


	return nullptr;
}


imtbase::ICollectionInfo::Id CMongoDatabaseObjectCollectionComp::GetObjectTypeId(const QByteArray& objectId) const
{

	return QByteArray();
}


idoc::MetaInfoPtr CMongoDatabaseObjectCollectionComp::GetDataMetaInfo(const Id& objectId) const
{


	return idoc::MetaInfoPtr();
}


// reimplemented (ICollectionInfo)

int CMongoDatabaseObjectCollectionComp::GetElementsCount(
			const iprm::IParamsSet* selectionParamPtr,
			const Id& /*parentId*/,
			int /*iterationFlags*/) const
{
	imtbase::CParamsSetJoiner filterParams(selectionParamPtr, m_filterParamsCompPtr.GetPtr());



	return 0;
}


imtbase::ICollectionInfo::Ids CMongoDatabaseObjectCollectionComp::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			const Id& /*parentId*/,
			int /*iterationFlags*/) const
{
	Ids retVal;

	// Create an instance.
//	mongocxx::instance inst{};
//	mongocxx::client client{mongocxx::uri{"mongodb://localhost:27017"}};
//	mongocxx::database db = client["agisbil"];

	mongocxx::database db = m_mongoDatabaseEngineCompPtr->GetDatabase();
	mongocxx::collection coll = db["address-type"];
	mongocxx::cursor cursor = coll.find({});

	// Iterate the cursor into bsoncxx::document::view objects.
	for (const bsoncxx::document::view& doc : cursor) {
		bsoncxx::document::element id_ele = doc["_id"];

		if (id_ele.type() == bsoncxx::type::k_oid) {

			const char* oid = id_ele.get_oid().value.bytes();

			retVal.push_back(oid);

				std::cout << "OID: " << oid << std::endl;
			} else {
				std::cout << "Error: _id was not an object ID." << std::endl;
			}
	}

	return retVal;
}


bool CMongoDatabaseObjectCollectionComp::GetSubsetInfo(
			imtbase::ICollectionInfo& /*subsetInfo*/,
			int /*offset*/,
			int /*count*/,
			const iprm::IParamsSet* /*selectionParamsPtr*/,
			const Id& /*parentId*/,
			int /*iterationFlags*/) const
{
	return false;
}


imtbase::IObjectCollectionIterator *CMongoDatabaseObjectCollectionComp::CreateObjectCollectionIterator(
			int offset, 
			int count, 
			const iprm::IParamsSet *selectionParamsPtr, 
			const Id &parentId, 
			int iterationFlags) const
{
	imtbase::CParamsSetJoiner filterParams(selectionParamsPtr, m_filterParamsCompPtr.GetPtr());



	return nullptr;
}


imtbase::ICollectionInfo::Id CMongoDatabaseObjectCollectionComp::GetParentId(const Id& /*elementId*/) const
{
	return Id();
}


imtbase::ICollectionInfo::Ids CMongoDatabaseObjectCollectionComp::GetElementPath(const Id& /*elementId*/) const
{
	return Ids();
}


bool CMongoDatabaseObjectCollectionComp::IsBranch(const Id& /*elementId*/) const
{
	return false;
}


QVariant CMongoDatabaseObjectCollectionComp::GetElementInfo(const QByteArray& elementId, int infoType) const
{


	return QVariant();
}


idoc::MetaInfoPtr CMongoDatabaseObjectCollectionComp::GetElementMetaInfo(const Id& elementId) const
{


	return idoc::MetaInfoPtr();
}


bool CMongoDatabaseObjectCollectionComp::SetElementName(const Id& elementId, const QString& name)
{


	return true;
}


bool CMongoDatabaseObjectCollectionComp::SetElementDescription(const Id& elementId, const QString& description)
{


	return true;
}


bool CMongoDatabaseObjectCollectionComp::SetElementEnabled(const Id& /*objectId*/, bool /*isEnabled*/)
{
	return false;
}


// reimplemented (istd::IChangeable)

bool CMongoDatabaseObjectCollectionComp::ResetData(CompatibilityMode /*mode*/)
{

	return false;
}


// protected methods



QSqlRecord CMongoDatabaseObjectCollectionComp::GetObjectRecord(const QByteArray& objectId) const
{


	return QSqlRecord();
}


void CMongoDatabaseObjectCollectionComp::OnFilterParamsChanged(const istd::IChangeable::ChangeSet& /*changeSet*/, const iprm::IParamsSet* /*filterParamsPtr*/)
{
	istd::CChangeNotifier changeNotifier(this);
}


void CMongoDatabaseObjectCollectionComp::OnDatabaseAccessChanged(const istd::IChangeable::ChangeSet& /*changeSet*/, const imtdb::IDatabaseLoginSettings* /*databaseAccessSettingsPtr*/)
{
	istd::CChangeNotifier changeNotifier(this);
}


// reimplemented (icomp::CComponentBase)

void CMongoDatabaseObjectCollectionComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_isInitialized = true;
}


void CMongoDatabaseObjectCollectionComp::OnComponentDestroyed()
{
	m_filterParamsObserver.UnregisterAllObjects();
	m_databaseAccessObserver.UnregisterAllObjects();

    BaseClass::OnComponentDestroyed();
}


} // namespace imtmongo


