#include <imtclientgql/CGqlObjectCollectionDelegateComp.h>


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imod/TModelWrap.h>
#include <iprm/IIdParam.h>
#include <iser/CMemoryWriteArchive.h>
#include <iser/CMemoryReadArchive.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtgql/CGqlRequest.h>
#include <imtclientgql/CGqlObjectCollectionResponse.h>
#include <imtbase/CFilterCollectionProxy.h>
#include <imtbase/COperationContext.h>


namespace imtclientgql
{


// public methods

imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetObjectInfoRequest(const QByteArray& objectId) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "Info";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	imtgql::CGqlObject input("input");
	input.InsertField("Id", QVariant(objectId));
	requestPtr->AddParam(input);

	imtgql::CGqlObject query("info");
	requestPtr->AddField(query);

	return requestPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetObjectMetaInfoRequest(const QByteArray& objectId) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "MetaInfo";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	imtgql::CGqlObject input("input");
	input.InsertField("Id", QVariant(objectId));
	requestPtr->AddParam(input);

	imtgql::CGqlObject query("metaInfo");
	requestPtr->AddField(query);

	return requestPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetObjectDataMetaInfoRequest(const QByteArray& objectId) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "DataMetaInfo";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	imtgql::CGqlObject input("input");
	input.InsertField("Id", QVariant(objectId));
	requestPtr->AddParam(input);

	imtgql::CGqlObject query("dataMetaInfo");
	requestPtr->AddField(query);

	return requestPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateInsertObjectRequest(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			const istd::IChangeable* objectPtr,
			const QString& uploadUrl,
			const QByteArray& proposedObjectId,
			const QByteArray& nodeId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr,
			const imtbase::IOperationContext* operationContextPtr) const
{
	QByteArray data;
	QByteArray commandId = *m_collectionIdAttrPtr + "Add";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_MUTATION, commandId);
	imtgql::CGqlObject input("input");
	input.InsertField("typeId", QVariant(typeId));
	input.InsertField("name", QVariant(name));
	input.InsertField("description", QVariant(description));
	SerializeObject(objectPtr,data);
	input.InsertField("item", QVariant(data.toBase64()));
	input.InsertField("uploadUrl", QVariant(uploadUrl));
	input.InsertField("Id", QVariant(proposedObjectId));
	input.InsertField("nodeId", QVariant(nodeId));
	SerializeObject(dataMetaInfoPtr,data);
	input.InsertField("dataMetaInfo", QVariant(data.toBase64()));
	SerializeObject(collectionItemMetaInfoPtr,data);
	input.InsertField("collectionItemMetaInfo", QVariant(data.toBase64()));
	SerializeObject(operationContextPtr,data);
	input.InsertField("operationContext", QVariant(data.toBase64()));

	requestPtr->AddParam(input);

	imtgql::CGqlObject query("addedNotification");
	requestPtr->AddField(query);

	return requestPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateSetObjectNameRequest(
			const QByteArray& objectId,
			const QString& name,
			int clientVersion,
			const imtbase::IOperationContext* operationContextPtr) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "SetName";
	QByteArray data;
	imtgql::CGqlRequest* queryPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_MUTATION, commandId);

	imtgql::CGqlObject input("input");
	input.InsertField("id", QVariant(objectId));
	input.InsertField("newName", QVariant(name));
	input.InsertField("version", QVariant(clientVersion));
	SerializeObject(operationContextPtr, data);
	input.InsertField("operationContext", QVariant(data.toBase64()));
	queryPtr->AddParam(input);

	imtgql::CGqlObject query("rename");
	queryPtr->AddField(query);

	return queryPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateSetObjectDescriptionRequest(
			const QByteArray& objectId,
			const QString& description,
			int clientVersion,
			const imtbase::IOperationContext* operationContextPtr) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "SetDescription";
	QByteArray data;
	imtgql::CGqlRequest* queryPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_MUTATION, commandId);

	imtgql::CGqlObject input("input");
	input.InsertField("id", QVariant(objectId));
	input.InsertField("description", QVariant(description));
	input.InsertField("version", QVariant(clientVersion));
	SerializeObject(operationContextPtr, data);
	input.InsertField("operationContext", QVariant(data.toBase64()));
	queryPtr->AddParam(input);

	imtgql::CGqlObject query("setDescription");
	queryPtr->AddField(query);

	return queryPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateSetObjectMetaInfoRequest(
			const QByteArray& objectId,
			const idoc::IDocumentMetaInfo& metaInfo,
			int clientVersion,
			const imtbase::IOperationContext* operationContextPtr) const
{
	return nullptr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetObjectRequest(const QByteArray& objectId) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "View";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	imtgql::CGqlObject input("input");
	input.InsertField("Id", QVariant(objectId));
	requestPtr->AddParam(input);

	imtgql::CGqlObject query("item");
	requestPtr->AddField(query);

	return requestPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateSetObjectRequest(
			const QByteArray& objectId,
			const istd::IChangeable* objectPtr,
			const QString& uploadUrl,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr,
			int clientVersion,
			const imtbase::IOperationContext* operationContextPtr) const
{
	QByteArray data;

	QByteArray commandId = *m_collectionIdAttrPtr + "Update";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_MUTATION, commandId);
	imtgql::CGqlObject input("input");
	input.InsertField("Id", QVariant(objectId));
	SerializeObject(objectPtr,data);
	input.InsertField("item", QVariant(data.toBase64()));
	input.InsertField("uploadUrl", QVariant(uploadUrl));
	SerializeObject(dataMetaInfoPtr,data);
	input.InsertField("dataMetaInfo", QVariant(data.toBase64()));
	SerializeObject(collectionItemMetaInfoPtr,data);
	input.InsertField("collectionItemMetaInfo", QVariant(data.toBase64()));
	input.InsertField("clientVersion", QVariant(clientVersion));
	SerializeObject(operationContextPtr,data);
	input.InsertField("operationContext", QVariant(data.toBase64()));

	requestPtr->AddParam(input);

	imtgql::CGqlObject query("updatedNotification");
	requestPtr->AddField(query);

	return requestPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateRemoveObjectRequest(
			const QByteArray& objectId,
			int clientElementVersion,
			const imtbase::IOperationContext* operationContextPtr) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "Remove";
	QByteArray data;
	imtgql::CGqlRequest* queryPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_MUTATION, commandId);
	
	imtgql::CGqlObject input("input");
	input.InsertField("Id", QVariant(objectId));
	input.InsertField("version", QVariant(clientElementVersion));
	SerializeObject(operationContextPtr,data);
	input.InsertField("operationContext", QVariant(data.toBase64()));
	queryPtr->AddParam(input);

	imtgql::CGqlObject query("removedNotification");
	queryPtr->AddField(query);

	return queryPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetElementCountRequest(const iprm::IParamsSet* /*selectionParamsPtr*/) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "Count";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	imtgql::CGqlObject input("input");
	requestPtr->AddParam(input);

	imtgql::CGqlObject query("itemsCount");
	requestPtr->AddField(query);

	return requestPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetElementListRequest(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "Ids";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	imtgql::CGqlObject input("input");
	input.InsertField("offset", QVariant(offset));
	input.InsertField("count", QVariant(count));
	QByteArray data;
	if (selectionParamsPtr != nullptr){
		SerializeObject(selectionParamsPtr,data);
	}
	input.InsertField("selectionParams", QVariant(data.toBase64()));
	requestPtr->AddParam(input);

	imtgql::CGqlObject query("itemIds");
	requestPtr->AddField(query);

	return requestPtr;
}


imtgql::IGqlRequest* CGqlObjectCollectionDelegateComp::CreateGetSubCollectionRequest(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr) const
{
	QByteArray commandId = *m_collectionIdAttrPtr + "SubCollection";
	imtgql::CGqlRequest* requestPtr = new imtgql::CGqlRequest(imtgql::IGqlRequest::RT_QUERY, commandId);
	imtgql::CGqlObject input("input");

	imtgql::CGqlObject viewParams("viewParams");
	viewParams.InsertField("offset", QVariant(offset));
	viewParams.InsertField("count", QVariant(count));
	QByteArray data;
	SerializeObject(selectionParamsPtr,data);
	viewParams.InsertField("selectionParams", QVariant(data.toBase64()));
	input.InsertField("viewParams", viewParams);

	requestPtr->AddParam(input);

	imtgql::CGqlObject query("items");
	query.InsertField("info");
	query.InsertField("metaInfo");
	query.InsertField("dataMetaInfo");
	query.InsertField("operationContext");
	requestPtr->AddField(query);

	return requestPtr;
}


imtgql::IGqlResponse* CGqlObjectCollectionDelegateComp::CreateResponse(const imtgql::IGqlRequest& request) const
{
	const imtgql::CGqlRequest* requestPtr = dynamic_cast<const imtgql::CGqlRequest*>(&request);
	if (requestPtr != nullptr){
		return new CGqlObjectCollectionResponse(*requestPtr);
	}

	return nullptr;
}


// private methods

bool CGqlObjectCollectionDelegateComp::SerializeObject(const istd::IPolymorphic* object, QByteArray& objectData) const
{
	objectData.clear();

	if (object == nullptr){
		return false;
	}

	const iser::ISerializable* objectConst = dynamic_cast<const iser::ISerializable*>(object);
	iser::ISerializable* serializableObject = dynamic_cast<iser::ISerializable*>(const_cast<iser::ISerializable*>(objectConst));

	if (serializableObject == nullptr){
		QByteArray errorMessage = QObject::tr("Object data metainfo is not Serializable").toUtf8();
		SendErrorMessage(0, errorMessage);

		return false;
	}
	iser::CMemoryWriteArchive archive;
	if (!serializableObject->Serialize(archive)){
		QByteArray errorMessage = QObject::tr("Error when serializing an object").toUtf8();
		SendErrorMessage(0, errorMessage);

		return false;
	}
	else{
		objectData = QByteArray((char*)archive.GetBuffer(), archive.GetBufferSize());
	}

	return true;
}


bool CGqlObjectCollectionDelegateComp::DeSerializeObject(istd::IPolymorphic* object, const QByteArray& objectData) const
{
	if (object == nullptr){
		return false;
	}

	iser::ISerializable* serializableObject = dynamic_cast<iser::ISerializable*>(object);
	if (serializableObject == nullptr){
		QByteArray errorMessage = QString("Object data metainfo is not serializable").toUtf8();
		SendErrorMessage(0, errorMessage);

		return false;
	}

	iser::CMemoryReadArchive archive(objectData.data(), objectData.count());
	if (!serializableObject->Serialize(archive)){
		QByteArray errorMessage = QString("Error when serializing an object").toUtf8();
		SendErrorMessage(0, errorMessage);

		return false;
	}

	return true;
}



} // namespace imtclientgql


