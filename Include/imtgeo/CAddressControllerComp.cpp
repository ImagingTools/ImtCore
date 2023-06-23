#include <imtgeo/CAddressControllerComp.h>


// ImtCore includes
#include <imtgeo/CAddressTypeInfo.h>
#include <imtgeo/CAddressElementInfo.h>
#include <imtbase/CCollectionFilter.h>


namespace imtgeo
{


imtbase::CTreeItemModel* CAddressControllerComp::GetObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();

	QByteArray addressId = GetObjectIdFromInputParams(gqlRequest.GetParams());

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(addressId, dataPtr)){
		const IAddressElementInfo* addressInfoPtr = dynamic_cast<const IAddressElementInfo*>(dataPtr.GetPtr());

		if (addressInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an address info");
			return nullptr;
		}

		QByteArray id = addressInfoPtr->GetId();
		QByteArray parentId;
		QList<QByteArray> parents = addressInfoPtr->GetParentIds();
		if (!parents.isEmpty()){
			parentId = *(parents.end());
		}
		QByteArray typeId = addressInfoPtr->GetAddressTypeId();
		QString typeName;
		QString typeShortName;
		imtbase::IObjectCollection::DataPtr dataTypePtr;
		if (m_addressTypeCollectionPtr->GetObjectData(typeId, dataTypePtr)){
			const IAddressTypeInfo* addressTypeInfoPtr =
				dynamic_cast<const IAddressTypeInfo*>(dataTypePtr.GetPtr());
			QString typeName = addressTypeInfoPtr->GetName();
			QString typeShortName = addressTypeInfoPtr->GetShortName();
		}
		QString name = addressInfoPtr->GetName();
		QString address = addressInfoPtr->GetAddress();
		if (address == QString(""))
		{
			for (QByteArray elemId : parents)
			{
				imtbase::IObjectCollection::DataPtr dataElementPtr;
				if (m_objectCollectionCompPtr->GetObjectData(elemId, dataElementPtr)){
					const IAddressElementInfo* addressInfoPtr =
						dynamic_cast<const IAddressElementInfo*>(dataElementPtr.GetPtr());
					imtbase::IObjectCollection::DataPtr dataTypePtr;
					if (m_addressTypeCollectionPtr->GetObjectData(addressInfoPtr->GetAddressTypeId(), dataTypePtr)){
						const IAddressTypeInfo* typeInfoPtr =
							dynamic_cast<const IAddressTypeInfo*>(dataTypePtr.GetPtr());
						QString shortName = typeInfoPtr->GetShortName();
						QString name = addressInfoPtr->GetName();
						address += shortName + " " + name + ",";
					}
				}
			}
			address += typeShortName + " " + name;
		}

		QString description = addressInfoPtr->GetDescription();
		double lat = addressInfoPtr->GetLatitude();
		double lon = addressInfoPtr->GetLongitude();

		dataModel->SetData("AddressId", id);
		dataModel->SetData("ParentId", parentId);
		dataModel->SetData("TypeId", typeId);
		dataModel->SetData("TypeName", typeName);
		dataModel->SetData("Name", name);
		dataModel->SetData("Address", address);
		dataModel->SetData("Description", description);
		dataModel->SetData("Latitude", lat);
		dataModel->SetData("Longitude", lon);

	}

	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CAddressControllerComp::InsertObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* notificationModel = nullptr;
	QByteArray newObjectId, newFullAddressObjectId;
	QString name, description;

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}
	else{
		QByteArray objectId;
		const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

		istd::IChangeable* newObject = CreateObject(inputParams, objectId, name, description, errorMessage);

		if (newObject != nullptr){
			newObjectId = m_objectCollectionCompPtr->InsertNewObject("", name, description, newObject, objectId);
			imtbase::IObjectCollection::DataPtr dataPtr;
			m_objectCollectionCompPtr->GetObjectData(newObjectId, dataPtr);
			CAddressElementInfo addressElementInfo;
			addressElementInfo.CopyFrom(*dataPtr);
		}

		if (errorMessage.isEmpty() && (newObjectId.isEmpty() || newFullAddressObjectId.isEmpty())){
			errorMessage = QObject::tr("Can not insert object: %1").arg(qPrintable(objectId));
		}
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsModel = rootModelPtr->AddTreeModel("errors");
		errorsModel->SetData("message", errorMessage);
	}
	else{
		imtbase::IObjectCollection::DataPtr dataPtr;
		dataModel = new imtbase::CTreeItemModel();
		notificationModel = new imtbase::CTreeItemModel();
		notificationModel->SetData("Id", newObjectId);
		notificationModel->SetData("Name", name);
		if (m_objectCollectionCompPtr->GetObjectData(newObjectId, dataPtr)){
			const IAddressElementInfo* addressInfoPtr = dynamic_cast<const IAddressElementInfo*>(dataPtr.GetPtr());
		}
		dataModel->SetExternTreeModel("addedNotification", notificationModel);
	}

	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
}


istd::IChangeable* CAddressControllerComp::CreateObject(
			const QList<imtgql::CGqlObject>& inputParams,
			QByteArray& objectId,
			QString& name,
			QString& description,
			QString& errorMessage) const
{
	if (!m_addressInfoFactCompPtr.IsValid()){
		errorMessage = QObject::tr("Can not create Address: %1").arg(QString(objectId));
		return nullptr;
	}

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		istd::TDelPtr<CAddressElementInfo> addressInfoPtr = new CAddressElementInfo();
		if (addressInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an address info!");
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.CreateFromJson(itemData);

		if (itemModel.ContainsKey("AddressId")){
			QByteArray id = itemModel.GetData("AddressId").toByteArray();

			if (id.isEmpty()){
				errorMessage = QT_TR_NOOP("AddressId can't be empty!");
				return nullptr;
			}

			addressInfoPtr->SetId(id);
		}

		QByteArray parentId = nullptr;
		if (itemModel.ContainsKey("ParentId")){
			parentId = itemModel.GetData("ParentId").toByteArray();
			imtbase::IObjectCollection::DataPtr adrDataPtr;
			if (m_objectCollectionCompPtr->GetObjectData(parentId, adrDataPtr)){
				const IAddressElementInfo* adrInfoPtr = dynamic_cast<const IAddressElementInfo*>(adrDataPtr.GetPtr());
				QList<QByteArray> parentIds = adrInfoPtr->GetParentIds();
				parentIds.append((parentId));
				addressInfoPtr->SetParentIds(parentIds);
			}
		}

		if (itemModel.ContainsKey("TypeId")){
			QByteArray typeId = itemModel.GetData("TypeId").toByteArray();
			addressInfoPtr->SetAddressTypeId(typeId);
		}

		if (itemModel.ContainsKey("Name")){
			name = itemModel.GetData("Name").toString();
			addressInfoPtr->SetName(name);
		}

		if (itemModel.ContainsKey("Description")){
			description = itemModel.GetData("Description").toString();
			addressInfoPtr->SetDescription(description);
		}

		if (itemModel.ContainsKey("Latitude")){
			double lat = itemModel.GetData("Latitude").toDouble();
			addressInfoPtr->SetLatitude(lat);
		}

		if (itemModel.ContainsKey("Longitude")){
			double lon = itemModel.GetData("Longitude").toDouble();
			addressInfoPtr->SetLongitude(lon);
		}

		return addressInfoPtr.PopPtr();
	}

	errorMessage = QObject::tr("Can not create address: %1").arg(QString(objectId));

	return nullptr;
}



imtbase::CTreeItemModel* CAddressControllerComp::UpdateObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* notificationModel = nullptr;

	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

	QByteArray oldObjectId = inputParams.at(0).GetFieldArgumentValue("Id").toByteArray();
	QByteArray newObjectId;
	QString name, description;

	QString splitObjectId;
	if (m_separatorObjectIdAttrPtr.IsValid()){
		splitObjectId = oldObjectId;

		QStringList splitData = splitObjectId.split(*m_separatorObjectIdAttrPtr);
		splitObjectId = splitData[0].toUtf8();
	}

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}
	else{
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(oldObjectId, dataPtr)){
			const IAddressElementInfo* addressInfoPtr = dynamic_cast<const IAddressElementInfo*>(dataPtr.GetPtr());
			QString oldName = addressInfoPtr->GetName();

			imtbase::CCollectionFilter filterOnSerialId;
			QByteArrayList filteringOnSerialIdInfoIds;
			filteringOnSerialIdInfoIds << "Indexes";
			filterOnSerialId.SetFilteringInfoIds(filteringOnSerialIdInfoIds);

			istd::IChangeable* savedObject = CreateObject(inputParams, newObjectId, name, description, errorMessage);
			if (savedObject != nullptr){
				if (m_objectCollectionCompPtr->SetObjectData(oldObjectId, *savedObject) == false){
					errorMessage = QObject::tr("Can not update object: %1").arg(splitObjectId);
				}
				else{
					imtbase::IObjectCollection::DataPtr newDataPtr;
					m_objectCollectionCompPtr->GetObjectData(oldObjectId, newDataPtr);
					const IAddressElementInfo* newAddressInfoPtr = dynamic_cast<const IAddressElementInfo*>(newDataPtr.GetPtr());
					QString newName = newAddressInfoPtr->GetName();

				}
			}
			else{
				if (errorMessage.isEmpty()){
					errorMessage = QObject::tr("Can not create object for update: %1").arg(splitObjectId);
				}
			}
		}
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsModel = rootModelPtr->AddTreeModel("errors");
		errorsModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		notificationModel = new imtbase::CTreeItemModel();
		notificationModel->SetData("Id", newObjectId);
		notificationModel->SetData("Name", name);
		dataModel->SetExternTreeModel("updatedNotification", notificationModel);
	}
	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CAddressControllerComp::DeleteObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = "No collection component was set";

		return nullptr;
	}

	QByteArray objectId = GetObjectIdFromInputParams(gqlRequest.GetParams());
	if (objectId.isEmpty()){
		errorMessage = QObject::tr("No object-ID could not be extracted from the request");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* notificationModel = nullptr;

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsModel = rootModelPtr->AddTreeModel("errors");
		errorsModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		notificationModel = new imtbase::CTreeItemModel();
		notificationModel->SetData("Id", objectId);
		dataModel->SetExternTreeModel("removedNotification", notificationModel);
	}
	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
}


} // namespace imtgeo


