// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CSerializableObjectCollectionControllerComp.h>

// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>

// ACF includes
#include <iser/CMemoryWriteArchive.h>
#include <iser/CMemoryReadArchive.h>

// ImtCore includes
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection.h>
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
		QByteArray errorMessage = QStringLiteral("Object data metainfo is not Serializable").toUtf8();
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
		QByteArray errorMessage = QStringLiteral("Error when serializing an object").toUtf8();
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
		QByteArray errorMessage = QStringLiteral("Unable to deserialize object. Error: Object is not ISerializable").toUtf8();
		SendErrorMessage(0, errorMessage);

		return false;
	}

	const int bufferSize = imtbase::narrow_cast<int>(objectData.length());
	iser::CMemoryReadArchive archive(objectData.data(), bufferSize);
	if (!serializableObject->Serialize(archive)){
		QByteArray errorMessage = QStringLiteral("Unable to deserialize object '%1'").arg(objectData).toUtf8();
		SendErrorMessage(0, errorMessage);

		return false;
	}

	return true;
}


// reimplemented (imtservergql::CObjectCollectionControllerCompBase)

QJsonObject CSerializableObjectCollectionControllerComp::GetMetaInfo(
	const imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CSerializableObjectCollectionControllerComp");
		return QJsonObject();
	}

	QJsonObject rootObj;
	QJsonObject dataObj;

	const imtgql::CGqlParamObject& inputParams = gqlRequest.GetParams();

	QByteArray objectId = GetObjectIdFromInputParams(inputParams);
	dataObj.insert(QStringLiteral("id"), QJsonValue::fromVariant(objectId));

	QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
	dataObj.insert(QStringLiteral("typeId"), QJsonValue::fromVariant(typeId));

	idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(objectId);
	QByteArray data;

	if (!SerializeObject(metaInfo.GetPtr(), data)){
		errorMessage = QStringLiteral("Unable to get meta info for object with ID '%1'. Error: Serialization failed").arg(objectId);
		SendErrorMessage(0, errorMessage, "CSerializableObjectCollectionControllerComp");

		return QJsonObject();
	}

	dataObj.insert(QStringLiteral("metaInfo"), QJsonValue::fromVariant(data.toBase64()));

	rootObj.insert(QStringLiteral("data"), dataObj);
	return rootObj;
}


QJsonObject CSerializableObjectCollectionControllerComp::GetInfo(
			const imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CSerializableObjectCollectionControllerComp");
		return QJsonObject();
	}

	QJsonObject rootObj;
	QJsonObject dataObj;
	QJsonObject infoObj;

	const imtgql::CGqlParamObject& inputParams = gqlRequest.GetParams();

	QByteArray objectId = GetObjectIdFromInputParams(inputParams);
	infoObj.insert(QStringLiteral("id"), QJsonValue::fromVariant(objectId));

	QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
	infoObj.insert(QStringLiteral("typeId"), QJsonValue::fromVariant(typeId));

	QByteArray elementInfo = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toByteArray();
	infoObj.insert(QStringLiteral("name"), QJsonValue::fromVariant(elementInfo));

	elementInfo = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toByteArray();
	infoObj.insert(QStringLiteral("description"), QJsonValue::fromVariant(elementInfo));

	elementInfo = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_ENABLED).toByteArray();
	infoObj.insert(QStringLiteral("enabled"), QJsonValue::fromVariant(elementInfo));

	dataObj.insert(QStringLiteral("info"), infoObj);
	rootObj.insert(QStringLiteral("data"), dataObj);
	return rootObj;
}


QJsonObject CSerializableObjectCollectionControllerComp::GetDataMetaInfo(
	const imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CSerializableObjectCollectionControllerComp");
		return QJsonObject();
	}

	QJsonObject rootObj;
	QJsonObject dataObj;

	const imtgql::CGqlParamObject& inputParams = gqlRequest.GetParams();

	QByteArray objectId = GetObjectIdFromInputParams(inputParams);
	dataObj.insert(QStringLiteral("id"), QJsonValue::fromVariant(objectId));

	QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
	dataObj.insert(QStringLiteral("typeId"), QJsonValue::fromVariant(typeId));

	QByteArray data;
	idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);

	if (!SerializeObject(metaInfo.GetPtr(), data)){
		errorMessage = QStringLiteral("Unable to get a data meta info for object with ID '%1'. Error: Serialization failed").arg(objectId);
		SendErrorMessage(0, errorMessage, "CSerializableObjectCollectionControllerComp");
		return QJsonObject();
	}

	dataObj.insert(QStringLiteral("dataMetaInfo"), QJsonValue::fromVariant(data.toBase64()));

	rootObj.insert(QStringLiteral("data"), dataObj);
	return rootObj;
}


QJsonObject CSerializableObjectCollectionControllerComp::GetObjectFromRequest(
	const imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'm_objectCollectionCompPtr' was not set", "CSerializableObjectCollectionControllerComp");
		return QJsonObject();
	}

	QJsonObject rootObj;
	QJsonObject dataObj;

	const imtgql::CGqlParamObject& inputParams = gqlRequest.GetParams();

	QByteArray objectId = GetObjectIdFromInputParams(inputParams);

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		iser::ISerializable* object = dynamic_cast<iser::ISerializable*>(dataPtr.GetPtr());
		if (object == nullptr){
			errorMessage =  QStringLiteral("Unable to get object for command-ID '%1'. Error: Object with ID '%2' is invalid")
						.arg(gqlRequest.GetCommandId())
						.arg(objectId).toUtf8();
			SendErrorMessage(0, errorMessage);

			return QJsonObject();
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
				errorMessage = QStringLiteral("Unable to get object for command-ID '%1'. Error: Object with ID '%2' cannot be serialized")
							.arg(gqlRequest.GetCommandId())
							.arg(objectId).toUtf8();
				SendErrorMessage(0, errorMessage);

				return QJsonObject();
			}

			QByteArray data((char*)archivePtr->GetBuffer(), archivePtr->GetBufferSize());
			dataObj.insert(QStringLiteral("objectData"), QJsonValue::fromVariant(data.toBase64()));
		}
	}
	else{
		errorMessage = QStringLiteral("Unable to get object for command-ID '%1'. Error: Object with ID '%2' not found")
				.arg(gqlRequest.GetCommandId())
				.arg(objectId).toUtf8();

		return QJsonObject();
	}

	rootObj.insert(QStringLiteral("data"), dataObj);
	return rootObj;
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
		errorMessage = QStringLiteral("Unable to create object from request. Error: GraphQL input parameters is invalid");
		SendErrorMessage(0, errorMessage);

		return nullptr;
	}

	QByteArray typeId = inputObjectPtr->GetParamArgumentValue("typeId").toByteArray();
	QByteArray objectData64 = inputObjectPtr->GetParamArgumentValue("item").toByteArray();
	QByteArray objectData = QByteArray::fromBase64(objectData64);
	if (objectData.isEmpty()){
		errorMessage = QStringLiteral("Unable to create object from request. Error: 'item' from input params is empty");
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
		errorMessage = QStringLiteral("Unable to create object from request. Error: Object is invalid");
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
		errorMessage = QStringLiteral("Unable to create object from request. Error: GraphQL input parameters is invalid");
		SendErrorMessage(0, errorMessage);

		return false;
	}

	QByteArray objectData64 = inputObjectPtr->GetParamArgumentValue("item").toByteArray();
	QByteArray objectData = QByteArray::fromBase64(objectData64);
	if (objectData.isEmpty()){
		errorMessage = QStringLiteral("Unable to create object from request. Error: 'item' from input params is empty");
		SendErrorMessage(0, errorMessage);

		return false;
	}

	return DeSerializeObject(&object, objectData);
}


QJsonObject CSerializableObjectCollectionControllerComp::GetObjectListFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'm_objectCollectionCompPtr' was not set", "CSerializableObjectCollectionControllerComp");
		return QJsonObject();
	}

	const imtgql::CGqlParamObject& inputParams = gqlRequest.GetParams();

	QJsonObject rootObj;
	QJsonObject dataObj;
	QJsonArray itemsArray;

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

			return QJsonObject();
		}
	}

	imtbase::ICollectionInfo::Ids ids = m_objectCollectionCompPtr->GetElementIds(offset, count, filterParamsPtr.GetPtr());
	for (const imtbase::ICollectionInfo::Id& id: ids){
		QJsonObject itemObj;
		QJsonObject infoObj;

		infoObj.insert(QStringLiteral("id"), QJsonValue::fromVariant(id));

		QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(id);
		infoObj.insert(QStringLiteral("typeId"), QJsonValue::fromVariant(typeId));

		QByteArray name = m_objectCollectionCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_NAME).toByteArray();
		infoObj.insert(QStringLiteral("name"), QJsonValue::fromVariant(name));

		QByteArray description = m_objectCollectionCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_DESCRIPTION).toByteArray();
		infoObj.insert(QStringLiteral("description"), QJsonValue::fromVariant(description));

		QByteArray enabled = m_objectCollectionCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_ENABLED).toByteArray();
		infoObj.insert(QStringLiteral("enabled"), QJsonValue::fromVariant(enabled));

		itemObj.insert(QStringLiteral("info"), infoObj);

		QByteArray serializedMetaIndoData;
		idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(id);
		if (SerializeObject(metaInfo.GetPtr(), serializedMetaIndoData)){
			itemObj.insert(QStringLiteral("metaInfo"), QJsonValue::fromVariant(serializedMetaIndoData.toBase64()));
		}
		else{
			SendWarningMessage(0, QStringLiteral("Unable to set meta info for element '%1'. Error: Meta info serialization failed").arg(id), "CSerializableObjectCollectionControllerComp");
		}

		serializedMetaIndoData.clear();
		idoc::MetaInfoPtr dataMetaInfo = m_objectCollectionCompPtr->GetDataMetaInfo(id);

		if (SerializeObject(dataMetaInfo.GetPtr(), serializedMetaIndoData)){
			itemObj.insert(QStringLiteral("dataMetaInfo"), QJsonValue::fromVariant(serializedMetaIndoData.toBase64()));
		}
		else{
			SendWarningMessage(0, QStringLiteral("Unable to set data meta info for element '%1'. Error: Data meta info serialization failed").arg(id), "CSerializableObjectCollectionControllerComp");
		}

		itemsArray.append(itemObj);
	}

	dataObj.insert(QStringLiteral("items"), itemsArray);
	rootObj.insert(QStringLiteral("data"), dataObj);
	return rootObj;
}



} // namespace imtservergql


