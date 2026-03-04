// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
		const CPositionIdentifiable* addressPosition = dynamic_cast<const CPositionIdentifiable*>(dataPtr.GetPtr());

		if (addressInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an address info");
			return nullptr;
		}

		QByteArray id = addressPosition->GetObjectUuid();
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
			typeName = addressTypeInfoPtr->GetName();
			typeShortName = addressTypeInfoPtr->GetShortName();
		}
		QString name = addressInfoPtr->GetName();
		QString address = addressInfoPtr->GetAddress();
		if (address == QString(""))
		{
			for (const QByteArray& elemId: parents)
			{
				imtbase::IObjectCollection::DataPtr dataElementPtr;
				if (m_objectCollectionCompPtr->GetObjectData(elemId, dataElementPtr)){
					const IAddressElementInfo* parentAddressInfoPtr =
						dynamic_cast<const IAddressElementInfo*>(dataElementPtr.GetPtr());
					imtbase::IObjectCollection::DataPtr parentDataTypePtr;
					if (m_addressTypeCollectionPtr->GetObjectData(parentAddressInfoPtr->GetAddressTypeId(), parentDataTypePtr)){
						const IAddressTypeInfo* typeInfoPtr =
							dynamic_cast<const IAddressTypeInfo*>(parentDataTypePtr.GetPtr());
						QString shortName = typeInfoPtr->GetShortName();
						QString addressName = parentAddressInfoPtr->GetName();
						address += shortName + " " + addressName + ",";
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
	QByteArray newObjectId;
	QString name;
	QString description;

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}
	else{
		QByteArray objectId;

		istd::IChangeableUniquePtr newObject = CreateObjectFromRequest(gqlRequest, objectId, errorMessage);
		if (newObject.IsValid()){
			newObjectId = m_objectCollectionCompPtr->InsertNewObject("", name, description, newObject.GetPtr(), objectId);
			imtbase::IObjectCollection::DataPtr dataPtr;
			m_objectCollectionCompPtr->GetObjectData(newObjectId, dataPtr);
			CAddressElementInfo addressElementInfo;
			addressElementInfo.CopyFrom(*dataPtr);
		}

		if (!errorMessage.isEmpty() || newObjectId.isEmpty()){
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
		dataModel->SetExternTreeModel("addedNotification", notificationModel);
	}

	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
}


istd::IChangeableUniquePtr CAddressControllerComp::CreateObjectFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			QByteArray& objectId,
			QString& errorMessage) const
{
	if (!m_addressInfoFactCompPtr.IsValid()){
		errorMessage = QObject::tr("Can not create Address: %1").arg(QString(objectId));
		return nullptr;
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject(QByteArrayLiteral("input"));
	if (inputParamPtr == nullptr){
		errorMessage = QStringLiteral("Unable to create address object. Error: GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, __FILE__);

		return nullptr;
	}

	objectId = GetObjectIdFromInputParams(*inputParamPtr);

	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	QByteArray itemData = inputParamPtr->GetParamArgumentValue(QByteArrayLiteral("Item")).toByteArray();
	if (!itemData.isEmpty()){
		istd::TDelPtr<CAddressElementInfo> addressInfoPtr = new CAddressElementInfo();
		if (addressInfoPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an address info!");
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.CreateFromJson(itemData);
		addressInfoPtr->SetObjectUuid(objectId);

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
			QString name = itemModel.GetData("Name").toString();
			addressInfoPtr->SetName(name);
		}

		if (itemModel.ContainsKey("Description")){
			QString description = itemModel.GetData("Description").toString();
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

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject(QByteArrayLiteral("input"));
	if (inputParamPtr == nullptr){
		errorMessage = QStringLiteral("Unable to update address object. Error: GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, __FILE__);

		return nullptr;
	}

	QByteArray oldObjectId = GetObjectIdFromInputParams(*inputParamPtr);
	QByteArray newObjectId;
	QString name;
	QString description;

	QString splitObjectId = oldObjectId;

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

			istd::IChangeableUniquePtr savedObject = CreateObjectFromRequest(gqlRequest, newObjectId, errorMessage);
			if (savedObject.IsValid()){
				if (!m_objectCollectionCompPtr->SetObjectData(oldObjectId, *savedObject)){
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


} // namespace imtgeo


