#include <imtgql/CSerializableObjectCollectionControllerComp.h>

// ACF includes
#include <iser/CJsonMemWriteArchive.h>
#include <iser/CJsonMemReadArchive.h>


namespace imtgql
{


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
			QByteArray json;
			{
				iser::ISerializable* object = dynamic_cast<iser::ISerializable*>(dataPtr.GetPtr());
				if (object == nullptr){
					errorMessage = QObject::tr("Object is not Serializable").toUtf8();
					SendErrorMessage(0, errorMessage);

					return nullptr;
				}
				iser::CJsonMemWriteArchive archive(json);
				if (!object->Serialize(archive)){
					errorMessage = QObject::tr("Error when serializing an object").toUtf8();
					SendErrorMessage(0, errorMessage);

					return nullptr;
				}
			}
			dataModel->SetData("objectData", json.toBase64());
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

	iser::CJsonMemReadArchive archive(objectData);
	if (!objectPtr->Serialize(archive)){
		errorMessage = QObject::tr("Error when serializing an object").toUtf8();
		SendErrorMessage(0, errorMessage);

		return nullptr;
	}


	return objectPtr;
}


} // namespace imtgql






