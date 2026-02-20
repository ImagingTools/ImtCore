// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CSerializableObjectCollectionControllerComp.h>


// ACF includes
#include <iser/CMemoryWriteArchive.h>
#include <iser/CMemoryReadArchive.h>

// ImtCore includes
#include <imtbase/imtbase.h>


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

	objectData = QByteArray((char*)archivePtr->GetBuffer(), archivePtr->GetBufferSize());

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

	const int bufferSize = imtbase::narrow_cast<int>(objectData.length());
	iser::CMemoryReadArchive archive(objectData.data(), bufferSize);
	if (!serializableObject->Serialize(archive)){
		QByteArray errorMessage = QString("Unable to deserialize object '%1'").arg(qPrintable(objectData)).toUtf8();
		SendErrorMessage(0, errorMessage);

		return false;
	}

	return true;
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

	const imtgql::CGqlParamObject& inputParams = gqlRequest.GetParams();

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

	const imtgql::CGqlParamObject& inputParams = gqlRequest.GetParams();

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

	const imtgql::CGqlParamObject& inputParams = gqlRequest.GetParams();

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

	const imtgql::CGqlParamObject& inputParams = gqlRequest.GetParams();

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


istd::IChangeableUniquePtr CSerializableObjectCollectionControllerComp::CreateObjectFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			QByteArray& /*objectId*/,
			QString& errorMessage) const
{
	if (!m_objectFactCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectFactory' was not set", "CSerializableObjectCollectionControllerComp");
		return nullptr;
	}

	const imtgql::CGqlParamObject* inputObjectPtr = gqlRequest.GetParamObject("input");
	if (inputObjectPtr == nullptr){
		errorMessage = QString("Unable to create object from request. Error: GraphQL input parameters is invalid");
		SendErrorMessage(0, errorMessage);

		return nullptr;
	}

	QByteArray typeId = inputObjectPtr->GetParamArgumentValue("typeId").toByteArray();
	QByteArray objectData64 = inputObjectPtr->GetParamArgumentValue("item").toByteArray();
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

	istd::IChangeableUniquePtr objectPtr = m_objectFactCompPtr.CreateInstance(index);
	if (!objectPtr.IsValid()){
		errorMessage = QString("Unable to create object from request. Error: Object is invalid");
		SendErrorMessage(0, errorMessage);

		return nullptr;
	}

	if (DeSerializeObject(objectPtr.GetPtr(), objectData)){
		return objectPtr;
	}

	return nullptr;
}


bool CSerializableObjectCollectionControllerComp::UpdateObjectFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	const imtgql::CGqlParamObject* inputObjectPtr = gqlRequest.GetParamObject("input");
	if (inputObjectPtr == nullptr){
		errorMessage = QString("Unable to create object from request. Error: GraphQL input parameters is invalid");
		SendErrorMessage(0, errorMessage);

		return false;
	}

	QByteArray objectData64 = inputObjectPtr->GetParamArgumentValue("item").toByteArray();
	QByteArray objectData = QByteArray::fromBase64(objectData64);
	if (objectData.isEmpty()){
		errorMessage = QString("Unable to create object from request. Error: 'item' from input params is empty");
		SendErrorMessage(0, errorMessage);

		return false;
	}

	return DeSerializeObject(&object, objectData);
}


imtbase::CTreeItemModel* CSerializableObjectCollectionControllerComp::ListObjects(
			const imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'm_objectCollectionCompPtr' was not set", "CSerializableObjectCollectionControllerComp");
		return nullptr;
	}

	const imtgql::CGqlParamObject& inputParams = gqlRequest.GetParams();

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");
	imtbase::CTreeItemModel* itemsModel = dataModel->AddTreeModel("items");
	itemsModel->SetIsArray(true);

	const imtgql::CGqlParamObject* viewParamsGql = nullptr;
	const imtgql::CGqlParamObject* inputObject = inputParams.GetParamArgumentObjectPtr("input");
	if (inputObject != nullptr){
		viewParamsGql = inputObject->GetParamArgumentObjectPtr("viewParams");
	}

	int offset = 0;
	int count = -1;
	QByteArray data;
	if (viewParamsGql != nullptr){
		offset = viewParamsGql->GetParamArgumentValue("offset").toInt();
		count = viewParamsGql->GetParamArgumentValue("count").toInt();
		data = viewParamsGql->GetParamArgumentValue("selectionParams").toByteArray();
	}

	iprm::IParamsSetUniquePtr filterParamsPtr;
	if (filterParamsPtr.IsValid() && !data.isEmpty()){
		if (!DeSerializeObject(filterParamsPtr.GetPtr(), QByteArray::fromBase64(data))){
			SendErrorMessage(0, "Unable to deserialize collection filter");

			return nullptr;
		}
	}

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

		QByteArray serializedMetaIndoData;
		idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(id);
		if (SerializeObject(metaInfo.GetPtr(), serializedMetaIndoData)){
			itemsModel->SetData("metaInfo", serializedMetaIndoData.toBase64(), itemIndex);
		}
		else{
			SendWarningMessage(0, QString("Unable to set meta info for element '%1'. Error: Meta info serialization failed").arg(qPrintable(id)), "CSerializableObjectCollectionControllerComp");
		}

		serializedMetaIndoData.clear();
		idoc::MetaInfoPtr dataMetaInfo = m_objectCollectionCompPtr->GetDataMetaInfo(id);

		if (SerializeObject(dataMetaInfo.GetPtr(), serializedMetaIndoData)){
			itemsModel->SetData("dataMetaInfo", serializedMetaIndoData.toBase64(), itemIndex);
		}
		else{
			SendWarningMessage(0, QString("Unable to set data meta info for element '%1'. Error: Data meta info serialization failed").arg(qPrintable(id)), "CSerializableObjectCollectionControllerComp");
		}
	}

	return rootModelPtr.PopPtr();
}



} // namespace imtservergql


