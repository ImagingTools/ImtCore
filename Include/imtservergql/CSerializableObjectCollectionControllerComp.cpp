#include <imtservergql/CSerializableObjectCollectionControllerComp.h>


// ACF includes
#include <iser/CMemoryWriteArchive.h>
#include <iser/CMemoryReadArchive.h>


namespace imtservergql
{


// protected methods

void CSerializableObjectCollectionControllerComp::CustomProcessObject(const imtgql::CGqlRequest& /*gqlRequest*/, iser::ISerializable& /*object*/) const
{
}


bool CSerializableObjectCollectionControllerComp::SerializeObject(
	const istd::IPolymorphic* object,
	QByteArray& objectData) const
{
	objectData.clear();

	const iser::ISerializable* objectConst = dynamic_cast<const iser::ISerializable*>(object);
	iser::ISerializable* serializableObject = dynamic_cast<iser::ISerializable*>(const_cast<iser::ISerializable*>(objectConst));
	if (serializableObject == nullptr){
		QByteArray errorMessage = QString("Object data metainfo is not Serializable").toUtf8();
		SendErrorMessage(0, errorMessage);

		return false;
	}

	istd::TDelPtr<iser::CMemoryWriteArchive> archivePtr;
	if (m_versionInfoCompPtr.IsValid()){
		archivePtr.SetPtr(new iser::CMemoryWriteArchive(m_versionInfoCompPtr.GetPtr()));
	}
	else{
		archivePtr.SetPtr(new iser::CMemoryWriteArchive());
	}

	if (!serializableObject->Serialize(*archivePtr.GetPtr())){
		QByteArray errorMessage = QString("Error when serializing an object").toUtf8();
		SendErrorMessage(0, errorMessage);

		return false;
	}
	else{
		objectData = QByteArray((char*)archivePtr->GetBuffer(), archivePtr->GetBufferSize());
	}

	return true;
}


bool CSerializableObjectCollectionControllerComp::DeSerializeObject(
	istd::IPolymorphic* object,
	const QByteArray& objectData) const
{
	iser::ISerializable* serializableObject = dynamic_cast<iser::ISerializable*>(object);
	if (serializableObject == nullptr){
		QByteArray errorMessage = QString("Unable to deserialize object. Error: Object is not ISerializable").toUtf8();
		SendErrorMessage(0, errorMessage);

		return false;
	}

	iser::CMemoryReadArchive archive(objectData.data(), objectData.length());
	if (!serializableObject->Serialize(archive)){
		QByteArray errorMessage = QString("Unable to deserialize object '%1'").arg(qPrintable(objectData)).toUtf8();
		SendErrorMessage(0, errorMessage);

		return false;
	}

	return true;
}


// reimplemented (imtgql::IGqlRequestHandler)

bool CSerializableObjectCollectionControllerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	if (m_collectionIdAttrPtr.IsValid() && !m_collectionIdAttrPtr->GetValue().isEmpty()){
		QByteArray externCommandId = gqlRequest.GetCommandId();

		for (const QByteArray& commandId : m_baseCommandIds){
			if (*m_collectionIdAttrPtr + commandId == externCommandId){
				return true;
			}
		}
	}

	return BaseClass::IsRequestSupported(gqlRequest);
}


// reimplemented (imtservergql::CObjectCollectionControllerCompBase)

imtbase::CTreeItemModel* CSerializableObjectCollectionControllerComp::GetMetaInfo(
	const imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CSerializableObjectCollectionControllerComp");
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");

	imtgql::CGqlObject inputParams = gqlRequest.GetParams();

	QByteArray objectId = GetObjectIdFromInputParams(inputParams);
	dataModel->SetData("id", objectId);

	QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
	dataModel->SetData("typeId", typeId);

	idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(objectId);
	QByteArray data;

	if (!SerializeObject(metaInfo.GetPtr(), data)){
		errorMessage = QString("Unable to get meta info for object with ID '%1'. Error: Serialization failed").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CSerializableObjectCollectionControllerComp");

		return nullptr;
	}

	dataModel->SetData("metaInfo", data.toBase64());

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CSerializableObjectCollectionControllerComp::GetInfo(
			const imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CSerializableObjectCollectionControllerComp");
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");
	imtbase::CTreeItemModel* infoModelPtr = dataModel->AddTreeModel("info");

	const imtgql::CGqlObject inputParams = gqlRequest.GetParams();

	QByteArray objectId = GetObjectIdFromInputParams(inputParams);
	infoModelPtr->SetData("id", objectId);

	QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
	infoModelPtr->SetData("typeId", typeId);

	QByteArray elementInfo = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toByteArray();
	infoModelPtr->SetData("name", elementInfo);

	elementInfo = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toByteArray();
	infoModelPtr->SetData("description", elementInfo);

	elementInfo = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_ENABLED).toByteArray();
	infoModelPtr->SetData("enabled", elementInfo);

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CSerializableObjectCollectionControllerComp::GetDataMetaInfo(
	const imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CSerializableObjectCollectionControllerComp");
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");

	const imtgql::CGqlObject inputParams = gqlRequest.GetParams();

	QByteArray objectId = GetObjectIdFromInputParams(inputParams);
	dataModel->SetData("id", objectId);

	QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
	dataModel->SetData("typeId", typeId);

	QByteArray data;
	idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);

	if (!SerializeObject(metaInfo.GetPtr(), data)){
		errorMessage = QString("Unable to get a data meta info for object with ID '%1'. Error: Serialization failed").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CSerializableObjectCollectionControllerComp");
		return nullptr;
	}

	dataModel->SetData("dataMetaInfo", data.toBase64());

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CSerializableObjectCollectionControllerComp::GetObject(
	const imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'm_objectCollectionCompPtr' was not set", "CSerializableObjectCollectionControllerComp");
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	const imtgql::CGqlObject inputParams = gqlRequest.GetParams();

	QByteArray objectId = GetObjectIdFromInputParams(inputParams);

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		iser::ISerializable* object = dynamic_cast<iser::ISerializable*>(dataPtr.GetPtr());
		if (object == nullptr){
			errorMessage =  QString("Unable to get object for command-ID '%1'. Error: Object with ID '%2' is invalid")
						.arg(qPrintable(gqlRequest.GetCommandId()))
						.arg(qPrintable(objectId)).toUtf8();
			SendErrorMessage(0, errorMessage);

			return nullptr;
		}

		CustomProcessObject(gqlRequest, *object);

		istd::TDelPtr<iser::CMemoryWriteArchive> archivePtr;
		if (m_versionInfoCompPtr.IsValid()){
			archivePtr.SetPtr(new iser::CMemoryWriteArchive(m_versionInfoCompPtr.GetPtr()));
		}
		else{
			archivePtr.SetPtr(new iser::CMemoryWriteArchive());
		}

		if (archivePtr.IsValid()){
			if (!object->Serialize(*archivePtr.GetPtr())){
				errorMessage = QString("Unable to get object for command-ID '%1'. Error: Object with ID '%2' cannot be serialized")
							.arg(qPrintable(gqlRequest.GetCommandId()))
							.arg(qPrintable(objectId)).toUtf8();
				SendErrorMessage(0, errorMessage);

				return nullptr;
			}

			QByteArray data((char*)archivePtr->GetBuffer(), archivePtr->GetBufferSize());
			dataModelPtr->SetData("objectData", data.toBase64());
		}
	}
	else{
		errorMessage = QString("Unable to get object for command-ID '%1'. Error: Object with ID '%2' not found")
				.arg(qPrintable(gqlRequest.GetCommandId()))
				.arg(qPrintable(objectId)).toUtf8();

		return nullptr;
	}

	return rootModelPtr.PopPtr();
}


istd::IChangeable* CSerializableObjectCollectionControllerComp::CreateObjectFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			QByteArray& /*objectId*/,
			QString& /*name*/,
			QString& /*description*/,
			QString& errorMessage) const
{
	if (!m_objectFactCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectFactory' was not set", "CSerializableObjectCollectionControllerComp");
		return nullptr;
	}

	const imtgql::CGqlObject* inputObjectPtr = gqlRequest.GetParamObject("input");
	if (inputObjectPtr == nullptr){
		errorMessage = QString("Unable to create object from request. Error: GraphQL input parameters is invalid");
		SendErrorMessage(0, errorMessage);

		return nullptr;
	}

	QByteArray typeId = inputObjectPtr->GetFieldArgumentValue("typeId").toByteArray();
	QByteArray objectData64 = inputObjectPtr->GetFieldArgumentValue("item").toByteArray();
	QByteArray objectData = QByteArray::fromBase64(objectData64);
	if (objectData.isEmpty()){
		errorMessage = QString("Unable to create object from request. Error: 'item' from input params is empty");
		SendErrorMessage(0, errorMessage);

		return nullptr;
	}

	int index = GetObjectTypeIdIndex(typeId);
	if (index < 0){
		Q_ASSERT_X(false, "Type-ID is not supported", "CSerializableObjectCollectionControllerComp");
		return nullptr;
	}

	istd::TDelPtr<istd::IChangeable> objectPtr(m_objectFactCompPtr.CreateInstance(index));
	if (objectPtr == nullptr){
		errorMessage = QString("Unable to create object from request. Error: Object is invalid");
		SendErrorMessage(0, errorMessage);

		return nullptr;
	}

	if (DeSerializeObject(objectPtr.GetPtr(), objectData)){
		return objectPtr.PopPtr();
	}

	return nullptr;
}


imtbase::CTreeItemModel* CSerializableObjectCollectionControllerComp::ListObjects(
			const imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'm_objectCollectionCompPtr' was not set", "CSerializableObjectCollectionControllerComp");
		return nullptr;
	}

	const imtgql::CGqlObject inputParams = gqlRequest.GetParams();

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");
	imtbase::CTreeItemModel* itemsModel = dataModel->AddTreeModel("items");

	const imtgql::CGqlObject* viewParamsGql = nullptr;
	const imtgql::CGqlObject* inputObject = inputParams.GetFieldArgumentObjectPtr("input");
	if (inputObject != nullptr){
		viewParamsGql = inputObject->GetFieldArgumentObjectPtr("viewParams");
	}

	int offset = 0, count = -1;

	if (viewParamsGql != nullptr){
		offset = viewParamsGql->GetFieldArgumentValue("offset").toInt();
		count = viewParamsGql->GetFieldArgumentValue("count").toInt();
	}

	QByteArray data = inputObject->GetFieldArgumentValue("selectionParams").toByteArray();

	istd::TDelPtr<iprm::IParamsSet> filterParamsPtr;
	if (m_paramsSetFactCompPtr.IsValid()){
		filterParamsPtr.SetPtr(m_paramsSetFactCompPtr.CreateInstance());
	}
	else{
		filterParamsPtr.SetPtr(new iprm::CParamsSet);
	}

	DeSerializeObject(filterParamsPtr.GetPtr(), QByteArray::fromBase64(data));

	imtbase::ICollectionInfo::Ids ids = m_objectCollectionCompPtr->GetElementIds(offset, count, filterParamsPtr.GetPtr());
	for (const imtbase::ICollectionInfo::Id& id: ids){
		int itemIndex = itemsModel->InsertNewItem();

		imtbase::CTreeItemModel* infoModelPtr = itemsModel->AddTreeModel("info", itemIndex);

		infoModelPtr->SetData("id", id);

		QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(id);
		infoModelPtr->SetData("typeId", typeId);

		QByteArray name = m_objectCollectionCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_NAME).toByteArray();
		infoModelPtr->SetData("name", name);

		QByteArray description = m_objectCollectionCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_DESCRIPTION).toByteArray();
		infoModelPtr->SetData("description", description);

		QByteArray enabled = m_objectCollectionCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_ENABLED).toByteArray();
		infoModelPtr->SetData("enabled", enabled);

		QByteArray data;
		idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(id);
		if (SerializeObject(metaInfo.GetPtr(), data)){
			itemsModel->SetData("metaInfo", data.toBase64(), itemIndex);
		}
		else{
			SendWarningMessage(0, QString("Unable to set meta info for element '%1'. Error: Meta info serialization failed").arg(qPrintable(id)), "CSerializableObjectCollectionControllerComp");
		}

		data.clear();
		idoc::MetaInfoPtr dataMetaInfo = m_objectCollectionCompPtr->GetDataMetaInfo(id);

		if (SerializeObject(dataMetaInfo.GetPtr(), data)){
			itemsModel->SetData("dataMetaInfo", data.toBase64(), itemIndex);
		}
		else{
			SendWarningMessage(0, QString("Unable to set data meta info for element '%1'. Error: Data meta info serialization failed").arg(qPrintable(id)), "CSerializableObjectCollectionControllerComp");
		}
	}

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CSerializableObjectCollectionControllerComp::RenameObject(
	const imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'm_objectCollectionCompPtr' was not set", "CSerializableObjectCollectionControllerComp");
		return nullptr;
	}

	const imtgql::CGqlObject* inputObjectPtr = gqlRequest.GetParamObject("input");
	if (inputObjectPtr == nullptr){
		errorMessage = QString("Unable to rename an object. Error: GraphQL input parameters is invalid");
		SendErrorMessage(0, errorMessage);

		return nullptr;
	}

	QByteArray objectId = inputObjectPtr->GetFieldArgumentValue("id").toByteArray();
	QString newName = inputObjectPtr->GetFieldArgumentValue("newName").toString();;

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");
	imtbase::CTreeItemModel* notifyModel = dataModel->AddTreeModel("renameNotification");

	if (!objectId.isEmpty()){
		notifyModel->SetData("id", objectId);
		notifyModel->SetData("name", newName);

		m_objectCollectionCompPtr->SetElementName(objectId, newName);
	}

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CSerializableObjectCollectionControllerComp::SetObjectDescription(
	const imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'm_objectCollectionCompPtr' was not set", "CSerializableObjectCollectionControllerComp");
		return nullptr;
	}

	const imtgql::CGqlObject* inputObjectPtr = gqlRequest.GetParamObject("input");
	if (inputObjectPtr == nullptr){
		errorMessage = QString("Unable to set description for object. Error: GraphQL input parameters is invalid");
		SendErrorMessage(0, errorMessage);

		return nullptr;
	}

	QByteArray objectId = inputObjectPtr->GetFieldArgumentValue("id").toByteArray();
	QString description = inputObjectPtr->GetFieldArgumentValue("description").toString();

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	imtbase::CTreeItemModel* notifyModel = dataModelPtr->AddTreeModel("setDescriptionNotification");

	if (!objectId.isEmpty()){
		notifyModel->SetData("id", objectId);
		notifyModel->SetData("description", description);
		m_objectCollectionCompPtr->SetElementDescription(objectId, description);
	}

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CSerializableObjectCollectionControllerComp::GetElementIds(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'm_objectCollectionCompPtr' was not set", "CSerializableObjectCollectionControllerComp");
		return nullptr;
	}

	const imtgql::CGqlObject* inputObjectPtr = gqlRequest.GetParamObject("input");
	if (inputObjectPtr == nullptr){
		errorMessage = QString("Unable to get elements ids. Error: GraphQL input parameters is invalid");
		SendErrorMessage(0, errorMessage);

		return nullptr;
	}

	int offset = inputObjectPtr->GetFieldArgumentValue("offset").toInt();
	int count = inputObjectPtr->GetFieldArgumentValue("count").toInt();

	QByteArray data = inputObjectPtr->GetFieldArgumentValue("selectionParams").toByteArray();

	istd::TDelPtr<iprm::IParamsSet> filterParamsPtr;
	if (m_paramsSetFactCompPtr.IsValid()){
		filterParamsPtr.SetPtr(m_paramsSetFactCompPtr.CreateInstance());
	}
	else{
		filterParamsPtr.SetPtr(new iprm::CParamsSet);
	}

	DeSerializeObject(filterParamsPtr.GetPtr(), QByteArray::fromBase64(data));

	imtbase::ICollectionInfo::Ids elementIds = m_objectCollectionCompPtr->GetElementIds(offset, count, filterParamsPtr.GetPtr());

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	dataModelPtr->SetData("itemIds", elementIds.toList().join(';'));

	return rootModelPtr.PopPtr();
}


// reimplemented (icomp::CComponentBase)

void CSerializableObjectCollectionControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_baseCommandIds << QByteArray("Info");
	m_baseCommandIds << QByteArray("DataMetaInfo");
	m_baseCommandIds << QByteArray("MetaInfo");
	m_baseCommandIds << QByteArray("List");
	m_baseCommandIds << QByteArray("View");
	m_baseCommandIds << QByteArray("Remove");
	m_baseCommandIds << QByteArray("SetDescription");
	m_baseCommandIds << QByteArray("Add");
	m_baseCommandIds << QByteArray("Update");
	m_baseCommandIds << QByteArray("Item");
	m_baseCommandIds << QByteArray("Count");
	m_baseCommandIds << QByteArray("Ids");
	m_baseCommandIds << QByteArray("SubCollection");
	m_baseCommandIds << QByteArray("TypeId");
}


} // namespace imtservergql


