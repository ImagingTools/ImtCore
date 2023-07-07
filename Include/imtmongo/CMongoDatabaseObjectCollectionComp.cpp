#include <imtmongo/CMongoDatabaseObjectCollectionComp.h>

// mongocxx includes
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/string/to_string.hpp>
#include <bsoncxx/oid.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

// Qt includes
#include <QtCore/QtDebug>
#include <QtCore/QSharedPointer>

// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/CParamsSetJoiner.h>
#include <imtmongo/CMongoDatabaseObjectCollectionIterator.h>


using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

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


QByteArray CMongoDatabaseObjectCollectionComp::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			DataPtr defaultValuePtr,
			const QByteArray& proposedObjectId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr,
			const imtbase::IOperationContext* operationContextPtr)
{

	QByteArray objectId = proposedObjectId;
	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
	}

	mongocxx::database* db = m_mongoDatabaseEngineCompPtr->GetDatabase();
	mongocxx::collection coll = (*db)[GetCollectionName().toStdString()];
	coll.insert_one(make_document(kvp("_oid", objectId.toStdString())));

	return QByteArray();
}


bool CMongoDatabaseObjectCollectionComp::RemoveElement(const Id& elementId,
													   const imtbase::IOperationContext* operationContextPtr)
{
	mongocxx::database* db = m_mongoDatabaseEngineCompPtr->GetDatabase();
	mongocxx::collection coll = (*db)[GetCollectionName().toStdString()];
	bsoncxx::document::view_or_value doc = bsoncxx::builder::basic::make_document(kvp("_id", elementId.toStdString()));
	coll.delete_one(doc);

	return true;
}


const istd::IChangeable* CMongoDatabaseObjectCollectionComp::GetObjectPtr(const QByteArray& objectId) const
{
	mongocxx::database* db = m_mongoDatabaseEngineCompPtr->GetDatabase();
	mongocxx::collection coll = (*db)[GetCollectionName().toStdString()];

	bsoncxx::oid id(objectId.data(),12);

	bsoncxx::document::view_or_value filtr = bsoncxx::builder::basic::make_document(kvp("_id",id));
	bsoncxx::stdx::optional<bsoncxx::document::value> fs = coll.find_one(filtr);
	if(fs)
	{
		bsoncxx::document::view doc = fs->view();
		istd::IChangeable* dataObjPtr = m_objectDelegateCompPtr->CreateObjectFromRecord(doc);
		return dataObjPtr;
	}

	return nullptr;
}


bool CMongoDatabaseObjectCollectionComp::GetObjectData(const QByteArray& objectId, DataPtr& dataPtr) const
{
	if (objectId.isEmpty()){
		return false;
	}

	mongocxx::database* db = m_mongoDatabaseEngineCompPtr->GetDatabase();
	mongocxx::collection coll =(*db)[GetCollectionName().toStdString()];
	bsoncxx::oid id(objectId.data(),12);
	bsoncxx::document::view_or_value filtr = bsoncxx::builder::basic::make_document(kvp("_id",id));
	bsoncxx::stdx::optional<bsoncxx::document::value> fs = coll.find_one(filtr);
	if(fs)
	{
		bsoncxx::document::view doc = fs->view();
		istd::IChangeable* dataObjPtr = m_objectDelegateCompPtr->CreateObjectFromRecord(doc);
		dataPtr = DataPtr(DataPtr::RootObjectPtr(dataObjPtr), [dataObjPtr](){
			return dataObjPtr;
		});

		return dataPtr.IsValid();
	}

	return false;
}


bool CMongoDatabaseObjectCollectionComp::SetObjectData(
			const Id& objectId,
			const istd::IChangeable& object,
			CompatibilityMode /*mode*/,
			const imtbase::IOperationContext* operationContextPtr)
{
	mongocxx::database* db = m_mongoDatabaseEngineCompPtr->GetDatabase();
	mongocxx::collection coll = (*db)[GetCollectionName().toStdString()];
	bsoncxx::document::view_or_value oid = bsoncxx::builder::basic::make_document(kvp("_id", objectId.toStdString()));
	bsoncxx::document::view_or_value doc = m_objectDelegateCompPtr->CreateUpdateObjectQuery(objectId, object);
	coll.update_one(oid, doc);

	return true;
}


imtbase::IObjectCollection* CMongoDatabaseObjectCollectionComp::CreateSubCollection(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	imtbase::IObjectCollection* collectionPtr = m_objectCollectionFactoryCompPtr.CreateInstance();
	imtbase::CParamsSetJoiner filterParams(selectionParamsPtr, m_filterParamsCompPtr.GetPtr());

	if (m_objectDelegateCompPtr.IsValid()) {

		mongocxx::database* db = m_mongoDatabaseEngineCompPtr->GetDatabase();
		mongocxx::collection coll = (*db)[GetCollectionName().toStdString()];
		bsoncxx::document::view_or_value find_params ;//= m_objectDelegateCompPtr->GetSelectionQuery(QByteArray(),offset,count,selectionParamsPtr)
		mongocxx::options::find findOption;
		findOption = findOption.limit(count);
		findOption = findOption.skip(offset);
		mongocxx::cursor cursor = coll.find(find_params, findOption);

		for (bsoncxx::document::view doc : cursor) {
			istd::IChangeable* dataObjPtr = m_objectDelegateCompPtr->CreateObjectFromRecord(doc);
			DataPtr dataPtr = DataPtr(DataPtr::RootObjectPtr(dataObjPtr), [dataObjPtr]() {
				return dataObjPtr;
				});

			QByteArray objectId = m_objectDelegateCompPtr->GetObjectIdFromRecord(doc);

			QByteArray typeId = m_objectDelegateCompPtr->GetObjectTypeId(objectId);

			collectionPtr->InsertNewObject(typeId, "", "", dataPtr, objectId);
		}
	}
	return collectionPtr;
}




// reimplemented (IObjectCollectionInfo)

const iprm::IOptionsList* CMongoDatabaseObjectCollectionComp::GetObjectTypesInfo() const
{
	if (m_objectDelegateCompPtr.IsValid()) {

		return m_objectDelegateCompPtr->GetObjectTypeInfos();
	}

	return nullptr;
}


imtbase::ICollectionInfo::Id CMongoDatabaseObjectCollectionComp::GetObjectTypeId(const QByteArray& objectId) const
{
	if (m_objectDelegateCompPtr.IsValid()) {

		return m_objectDelegateCompPtr->GetObjectTypeId(objectId);
	}

	return QByteArray();
}


idoc::MetaInfoPtr CMongoDatabaseObjectCollectionComp::GetDataMetaInfo(const Id& objectId) const
{


	return idoc::MetaInfoPtr();
}


// reimplemented (ICollectionInfo)

int CMongoDatabaseObjectCollectionComp::GetElementsCount(
			const iprm::IParamsSet* selectionParamPtr) const
{
	imtbase::CParamsSetJoiner filterParams(selectionParamPtr, m_filterParamsCompPtr.GetPtr());

	mongocxx::database* db = m_mongoDatabaseEngineCompPtr->GetDatabase();
	mongocxx::collection coll = (*db)[GetCollectionName().toStdString()];
	bsoncxx::document::view_or_value find_params;//= m_objectDelegateCompPtr->GetSelectionQuery(QByteArray(),offset,count,selectionParamsPtr)
	int count = coll.count_documents(find_params);

	return count;
}


imtbase::ICollectionInfo::Ids CMongoDatabaseObjectCollectionComp::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	Ids retVal;

	mongocxx::database* db = m_mongoDatabaseEngineCompPtr->GetDatabase();

	QString collName = GetCollectionName();
	qDebug() << "collection name" << collName;

	mongocxx::collection coll = (*db)[collName.toStdString()];

	bsoncxx::document::view_or_value find_params ;//= m_objectDelegateCompPtr->GetSelectionQuery(QByteArray(),offset,count,selectionParamsPtr)
	mongocxx::options::find findOption;
	if(count > 0)
		findOption = findOption.limit(count);
	if(offset > 0)
		findOption = findOption.skip(offset);
	mongocxx::cursor cursor = coll.find(find_params, findOption);

	// Iterate the cursor into bsoncxx::document::view objects.
	for (const bsoncxx::document::view& doc : cursor) {
		bsoncxx::document::element id_ele = doc["_id"];

		if (id_ele.type() == bsoncxx::type::k_oid) {

			QByteArray Id(id_ele.get_oid().value.bytes(),12);
			retVal.push_back(Id);


			std::string oid = id_ele.get_oid().value.to_string();
			QString id = QString::fromStdString(oid);
			//retVal.push_back(id.toLocal8Bit());

			std::cout << "OID2: " << oid<< std::endl;
			qDebug() << id;

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
			const iprm::IParamsSet* /*selectionParamsPtr*/) const
{

	return false;
}


imtbase::IObjectCollectionIterator *CMongoDatabaseObjectCollectionComp::CreateObjectCollectionIterator(
			int offset, 
			int count, 
			const iprm::IParamsSet *selectionParamsPtr) const
{
	imtbase::CParamsSetJoiner filterParams(selectionParamsPtr, m_filterParamsCompPtr.GetPtr());

	//mongocxx::database db = m_mongoDatabaseEngineCompPtr->GetDatabase();
	mongocxx::collection coll = (*m_mongoDatabaseEngineCompPtr->GetDatabase())[GetCollectionName().toStdString()];
	bsoncxx::document::view_or_value findParams ;//= m_objectDelegateCompPtr->GetSelectionQuery(QByteArray(),offset,count,selectionParamsPtr)

	mongocxx::options::find findOption;
	findOption = findOption.limit(count);
	findOption = findOption.skip(offset);
	/*if(count > 0){
		if(offset > 0){
			findOption = findOption.limit(count);
			findOption = findOption.skip(offset);
		}
		else
			findOption = findOption.limit(count);
	}
	else{
		if(offset > 0)
			findOption = findOption.skip(offset);
	}*/

	mongocxx::cursor cursor = coll.find(findParams, findOption);

	CMongoDatabaseObjectCollectionIterator* iterator =
			new CMongoDatabaseObjectCollectionIterator(cursor, m_objectDelegateCompPtr.GetPtr());


	return iterator;
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

QByteArray CMongoDatabaseObjectCollectionComp::GetCollectionName() const
{
	return m_collectionNameAttrPtr->GetValue();
}


mongocxx::cursor CMongoDatabaseObjectCollectionComp::GetObjectRecord(const QByteArray& objectId) const
{
	//mongocxx::database db = m_mongoDatabaseEngineCompPtr->GetDatabase();
	mongocxx::collection coll = (*m_mongoDatabaseEngineCompPtr->GetDatabase())[GetCollectionName().toStdString()];
	bsoncxx::document::view_or_value doc = bsoncxx::builder::basic::make_document(kvp("_id", objectId.toStdString()));
	mongocxx::cursor cursor = coll.find(doc);
	return cursor;
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


