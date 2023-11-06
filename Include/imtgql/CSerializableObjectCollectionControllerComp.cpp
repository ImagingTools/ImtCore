#include <imtgql/CSerializableObjectCollectionControllerComp.h>


// ACF includes
#include <iser/CMemoryWriteArchive.h>
#include <iser/CMemoryReadArchive.h>


namespace imtgql
{


bool CSerializableObjectCollectionControllerComp::SerializeObject(const istd::IPolymorphic* object, QByteArray& objectData) const
{
	objectData.clear();

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


bool CSerializableObjectCollectionControllerComp::DeSerializeObject(istd::IPolymorphic* object, const QByteArray& objectData) const
{
	iser::ISerializable* serializableObject = dynamic_cast<iser::ISerializable*>(object);
	if (serializableObject == nullptr){
		QByteArray errorMessage = QObject::tr("Object data metainfo is not Serializable").toUtf8();
		SendErrorMessage(0, errorMessage);

		return false;
	}
	iser::CMemoryReadArchive archive(objectData.data(), objectData.count());
	if (!serializableObject->Serialize(archive)){
		QByteArray errorMessage = QObject::tr("Error when serializing an object").toUtf8();
		SendErrorMessage(0, errorMessage);

		return false;
	}

	return true;
}


imtbase::CTreeItemModel* CSerializableObjectCollectionControllerComp::GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModelPtr->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

		QByteArray objectId = GetObjectIdFromInputParams(inputParams);
		dataModel->SetData("id", objectId);

		QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
		dataModel->SetData("typeId", typeId);

		idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(objectId);
		QByteArray data;
		SerializeObject(metaInfo.GetPtr(), data);
		dataModel->SetData("metaInfo", data.toBase64());

	}

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CSerializableObjectCollectionControllerComp::GetInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModelPtr->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		imtbase::CTreeItemModel* infoModel = dataModel->AddTreeModel("info");
		const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

		QByteArray objectId = GetObjectIdFromInputParams(inputParams);

		infoModel->SetData("id", objectId);
		QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
		infoModel->SetData("typeId", typeId);
//		imtbase::IObjectCollection::DataPtr dataPtr;
		QByteArray elementInfo = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toByteArray();
		infoModel->SetData("name", elementInfo);
		elementInfo = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toByteArray();
		infoModel->SetData("description", elementInfo);
		elementInfo = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_ENABLED).toByteArray();
		infoModel->SetData("enabled", elementInfo);
	}

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CSerializableObjectCollectionControllerComp::GetDataMetaInfo(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModelPtr->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		//ToDo Serial add

		const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

		QByteArray Id = GetObjectIdFromInputParams(inputParams);
		dataModel->SetData("id", Id);
		QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(Id);
		dataModel->SetData("typeId", typeId);

		QByteArray data;
		idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetDataMetaInfo(Id);
		SerializeObject(metaInfo.GetPtr(), data);
		dataModel->SetData("dataMetaInfo", data.toBase64());


	}

	return rootModelPtr.PopPtr();
}

imtbase::CTreeItemModel* CSerializableObjectCollectionControllerComp::GetObject(
        const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
    imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
    imtbase::CTreeItemModel* dataModel = nullptr;

    if (!m_objectCollectionCompPtr.IsValid()){
        errorMessage = QObject::tr("Internal error").toUtf8();
    }

    if (!errorMessage.isEmpty()){
        imtbase::CTreeItemModel* errorsItemModel = rootModel->AddTreeModel("errors");
        errorsItemModel->SetData("message", errorMessage);
    }
    else{
        dataModel = new imtbase::CTreeItemModel();

        dataModel->SetData("Id", "");
        //ToDo Serial add

		const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

		QByteArray Id = GetObjectIdFromInputParams(inputParams);

        imtbase::IObjectCollection::DataPtr dataPtr;
        if (m_objectCollectionCompPtr->GetObjectData(Id, dataPtr)){
			iser::ISerializable* object = dynamic_cast<iser::ISerializable*>(dataPtr.GetPtr());
			if (object == nullptr){
				errorMessage = QObject::tr("Object is not Serializable").toUtf8();
				SendErrorMessage(0, errorMessage);

				return nullptr;
			}

			iser::CMemoryWriteArchive archive;
			if (!object->Serialize(archive)){
				errorMessage = QObject::tr("Error when serializing an object").toUtf8();
				SendErrorMessage(0, errorMessage);

				return nullptr;
			}

			QByteArray data((char*)archive.GetBuffer(), archive.GetBufferSize());
			dataModel->SetData("objectData", data.toBase64());
        }
    }
    rootModel->SetExternTreeModel("data", dataModel);

    return rootModel;
}


istd::IChangeable* CSerializableObjectCollectionControllerComp::CreateObject(
            const QList<imtgql::CGqlObject>& inputParams,
            QByteArray &objectId,
            QString &name,
            QString &description,
            QString& errorMessage) const
{
	if (!m_objectFactCompPtr.IsValid()){
		errorMessage = QObject::tr("Can not create Object: %1").arg(QString(objectId));
        return nullptr;
    }

	QByteArray objectData64 = inputParams.at(0).GetFieldArgumentValue("item").toByteArray();
	QByteArray objectData = QByteArray::fromBase64(objectData64);
	if (objectData.isEmpty()){
		errorMessage = QObject::tr("Can not create object: %1").arg(QString(objectId));
		SendErrorMessage(0, errorMessage);

		return nullptr;
	}

	istd::TDelPtr<iser::ISerializable> objectPtr(m_objectFactCompPtr.CreateInstance());
	if (objectPtr == nullptr){
		errorMessage = QT_TR_NOOP("Unable to get an Object!");
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
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* itemsModel = nullptr;
	imtbase::CTreeItemModel* notificationModel = nullptr;

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModelPtr->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		itemsModel = new imtbase::CTreeItemModel();
		notificationModel = new imtbase::CTreeItemModel();

		const imtgql::CGqlObject* viewParamsGql = nullptr;
		if (inputParams.size() > 0){
			viewParamsGql = inputParams.at(0).GetFieldArgumentObjectPtr("viewParams");
		}

		iprm::CParamsSet filterParams;

		int offset = 0, count = -1;

		if (viewParamsGql != nullptr){
			offset = viewParamsGql->GetFieldArgumentValue("offset").toInt();
			count = viewParamsGql->GetFieldArgumentValue("count").toInt();
		}

//		int elementsCount = m_objectCollectionCompPtr->GetElementsCount(&filterParams);

//		int pagesCount = std::ceil(elementsCount / (double)count);
//		if (pagesCount <= 0){
//			pagesCount = 1;
//		}

//		notificationModel->SetData("PagesCount", pagesCount);
//		notificationModel->SetData("TotalCount", elementsCount);

		QVector<QByteArray> ids = m_objectCollectionCompPtr->GetElementIds(offset, count, &filterParams);
		for (QByteArray id: ids){
			int itemIndex = itemsModel->InsertNewItem();
			imtbase::CTreeItemModel* infoModel = itemsModel->AddTreeModel("info", itemIndex);
			const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();


			infoModel->SetData("id", id);
			QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(id);
			infoModel->SetData("typeId", typeId);
			QByteArray elementInfo = m_objectCollectionCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_NAME).toByteArray();
			infoModel->SetData("name", elementInfo);
			elementInfo = m_objectCollectionCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_DESCRIPTION).toByteArray();
			infoModel->SetData("description", elementInfo);
			elementInfo = m_objectCollectionCompPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_ENABLED).toByteArray();
			infoModel->SetData("enabled", elementInfo);

			QByteArray data;
			idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(id);
			SerializeObject(metaInfo.GetPtr(), data);
			itemsModel->SetData("metaInfo", data.toBase64(), itemIndex);

			data.clear();
			idoc::MetaInfoPtr dataMetaInfo = m_objectCollectionCompPtr->GetDataMetaInfo(id);
			SerializeObject(dataMetaInfo.GetPtr(), data);
			itemsModel->SetData("dataMetaInfo", data.toBase64(), itemIndex);
		}

		itemsModel->SetIsArray(true);


		dataModel->SetExternTreeModel("items", itemsModel);
		dataModel->SetExternTreeModel("notification", notificationModel);
	}

	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CSerializableObjectCollectionControllerComp::RenameObject(
		const imtgql::CGqlRequest& gqlRequest,
		QString& /*errorMessage*/) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

	QByteArray objectId;
	QString newName;
	if (inputParams.size() > 0){
		objectId = inputParams.at(0).GetFieldArgumentValue("id").toByteArray();
		newName = inputParams.at(0).GetFieldArgumentValue("newName").toString();
	}

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
		QString& /*errorMessage*/) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

	QByteArray objectId;
	QString description;

	if (inputParams.size() > 0){
		objectId = inputParams.at(0).GetFieldArgumentValue("id").toByteArray();
		description = inputParams.at(0).GetFieldArgumentValue("description").toString();
	}

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



} // namespace imtgql






