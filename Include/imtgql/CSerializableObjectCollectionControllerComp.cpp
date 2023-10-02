#include <imtgql/CSerializableObjectCollectionControllerComp.h>

// ACF includes
#include <iser/CMemoryWriteArchive.h>
#include <iser/CMemoryReadArchive.h>


namespace imtgql
{


//QVariant CSerializableObjectCollectionControllerComp::GetObjectInformation(const QByteArray &informationId, const QByteArray &objectId) const
//{
//	idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);

//	if (metaInfo.IsValid()){
//		if (informationId == QByteArray("UserId")){
//			return metaInfo->GetMetaInfo(imtauth::IUserInfo::MIT_USERNAME);
//		}
//		else if (informationId == QByteArray("Email")){
//			return metaInfo->GetMetaInfo(imtauth::IUserInfo::MIT_EMAIL);
//		}
//		else if (informationId == QByteArray("Added")){
//			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME);
//		}
//		else if (informationId == QByteArray("LastModified")){
//			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME);
//		}
//	}

//	return QVariant();
//}


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
		//ToDo Serial add

		const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

		QByteArray Id = GetObjectIdFromInputParams(inputParams);
		dataModel->SetData("id", Id);
		QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(Id);
		dataModel->SetData("typeId", typeId);

		imtbase::CTreeItemModel* metaInfoModel = dataModel->AddTreeModel("metaInfo");

		idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(Id);
		idoc::IDocumentMetaInfo::MetaInfoTypes metainfoTypes = metaInfo->GetMetaInfoTypes();
		for (int metainfoType: metainfoTypes){
			QByteArray metainfoName = metaInfo->GetMetaInfoName(metainfoType).toUtf8();
			switch (metainfoType){
				case idoc::IDocumentMetaInfo::MIT_CREATION_TIME: case idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME:
				{
					QString timestr = metaInfo->GetMetaInfo(metainfoType).toString();
					metaInfoModel->SetData(QString::number(metainfoType).toLocal8Bit(), timestr);
					break;
				}
				default:
					QByteArray data = metaInfo->GetMetaInfo(metainfoType).toByteArray();
					metaInfoModel->SetData(QString::number(metainfoType).toLocal8Bit(), data);
			}
		}

//		elementInfo = metaInfo->GetMetaInfo(imtauth::IUserInfo::MIT_USERNAME).toByteArray();
//		dataModel->SetData("description", elementInfo);
//		elementInfo = metaInfo->GetMetaInfo(imtauth::IUserInfo::MIT_USERNAME).toByteArray();
//		dataModel->SetData("version", elementInfo);


//			if (informationId == QByteArray("Name")){
//				result.text = m_collectionPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
//				result.sortValue = result.text;
//			}

//			idoc::MetaInfoPtr metaInfoPtr = m_collectionPtr->GetDataMetaInfo(objectId);
//			if (metaInfoPtr.IsValid()){
//				if (informationId == QByteArray("ApplicationId")){
//					result.text = metaInfoPtr->GetMetaInfo(aculainsp::IInspectionApplicationDocument::MIT_APPLICATION_ID).toByteArray();
//					result.sortValue = result.text;
//				}
//				else if (informationId == QByteArray("Version")){
//					result.text = metaInfoPtr->GetMetaInfo(aculainsp::IInspectionApplicationDocument::MIT_VERSION).toByteArray();
//					result.sortValue = result.text;
//				}
//			}

//			idoc::MetaInfoPtr metaInfo = m_collectionPtr->GetElementMetaInfo(objectId);
//			if (metaInfo.IsValid()){
//				if (informationId == QByteArray("Added")){
//					result.sortValue = metaInfo->GetMetaInfo(imtbase::ICollectionInfo::MIT_INSERTION_TIME);
//					result.text = result.sortValue.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
//				}
//				else if (informationId == QByteArray("ModificationTime")){
//					result.sortValue = metaInfo->GetMetaInfo(imtbase::ICollectionInfo::MIT_LAST_OPERATION_TIME);
//					result.text = result.sortValue.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
//				}
//				else if (informationId == QByteArray("Revision")){
//					QVariant revision = metaInfo->GetMetaInfo(imtbase::ICollectionInfo::MIT_REVISION);
//					if (revision.isValid()){
//						result.text = QString::number(revision.toInt());
//					}

//					result.sortValue = result.text;
//				}




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
		//ToDo Serial add

		imtbase::CTreeItemModel* infoModel = dataModel->AddTreeModel("info");
		const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

		QByteArray Id = GetObjectIdFromInputParams(inputParams);

		infoModel->SetData("id", Id);
		QByteArray typeId = m_objectCollectionCompPtr->GetObjectTypeId(Id);
		infoModel->SetData("typeId", typeId);
//		imtbase::IObjectCollection::DataPtr dataPtr;
		QByteArray elementInfo = m_objectCollectionCompPtr->GetElementInfo(Id, imtbase::ICollectionInfo::EIT_NAME).toByteArray();
		infoModel->SetData("name", elementInfo);
		elementInfo = m_objectCollectionCompPtr->GetElementInfo(Id, imtbase::ICollectionInfo::EIT_DESCRIPTION).toByteArray();
		infoModel->SetData("description", elementInfo);
		elementInfo = m_objectCollectionCompPtr->GetElementInfo(Id, imtbase::ICollectionInfo::EIT_ENABLED).toByteArray();
		infoModel->SetData("enabled", elementInfo);
	}

	return rootModelPtr.PopPtr();

}



imtbase::CTreeItemModel* CSerializableObjectCollectionControllerComp::GetDataMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
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

		imtbase::CTreeItemModel* metaInfoModel = dataModel->AddTreeModel("dataMetaInfo");

		idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetDataMetaInfo(Id);
		idoc::IDocumentMetaInfo::MetaInfoTypes metainfoTypes = metaInfo->GetMetaInfoTypes();
		for (int metainfoType: metainfoTypes){
			QByteArray metainfoName = metaInfo->GetMetaInfoName(metainfoType).toUtf8();
			metaInfoModel->SetData(QString::number(metainfoType).toLocal8Bit(), metaInfo->GetMetaInfo(metainfoType));
		}

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
		errorMessage = QObject::tr("Can not create device: %1").arg(QString(objectId));
		SendErrorMessage(0, errorMessage);

		return nullptr;
	}

	iser::ISerializable *objectPtr = m_objectFactCompPtr.CreateInstance();
	if (objectPtr == nullptr){
		errorMessage = QT_TR_NOOP("Unable to get an Object!");
		SendErrorMessage(0, errorMessage);

		return nullptr;
	}

	iser::CMemoryReadArchive archive(objectData.data(), objectData.count());
	if (!objectPtr->Serialize(archive)){
		errorMessage = QObject::tr("Error when serializing an object").toUtf8();
		SendErrorMessage(0, errorMessage);

		return nullptr;
	}


	return objectPtr;
}


} // namespace imtgql






