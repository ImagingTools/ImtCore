// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgeo/CAddressControllerComp.h>

// Qt includes
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

// ImtCore includes
#include <imtgeo/CAddressTypeInfo.h>
#include <imtgeo/CAddressElementInfo.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection.h>


namespace imtgeo
{


bool CAddressControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const QByteArray& /*objectTypeId*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QJsonObject& dataObj,
			QString& errorMessage) const
{
	const IAddressElementInfo* addressInfoPtr = dynamic_cast<const IAddressElementInfo*>(&data);
	const CPositionIdentifiable* addressPosition = dynamic_cast<const CPositionIdentifiable*>(&data);
	if (!addressInfoPtr || !addressPosition){
		errorMessage = QObject::tr("Unable to get an address info");

		return false;
	}

	QByteArray parentId;
	const QByteArrayList parents = addressInfoPtr->GetParentIds();
	if (!parents.isEmpty()){
		parentId = parents.last();
	}

	QString typeName;
	QString typeShortName;
	imtbase::IObjectCollection::DataPtr dataTypePtr;
	if (m_addressTypeCollectionPtr.IsValid() && m_addressTypeCollectionPtr->GetObjectData(addressInfoPtr->GetAddressTypeId(), dataTypePtr)){
		if (const auto* addressTypeInfoPtr = dataTypePtr.GetPtr<const IAddressTypeInfo>()){
			typeName = addressTypeInfoPtr->GetName();
			typeShortName = addressTypeInfoPtr->GetShortName();
		}
	}

	QString address = addressInfoPtr->GetAddress();
	if (address.isEmpty() && m_addressTypeCollectionPtr.IsValid()){
		for (const QByteArray& elemId : parents){
			imtbase::IObjectCollection::DataPtr dataElementPtr;
			if (!m_objectCollectionCompPtr->GetObjectData(elemId, dataElementPtr)){
				continue;
			}

			const IAddressElementInfo* parentAddressInfoPtr = dataElementPtr.GetPtr<const IAddressElementInfo>();
			if (!parentAddressInfoPtr){
				continue;
			}

			imtbase::IObjectCollection::DataPtr parentDataTypePtr;
			if (!m_addressTypeCollectionPtr->GetObjectData(parentAddressInfoPtr->GetAddressTypeId(), parentDataTypePtr)){
				continue;
			}

			if (const auto* typeInfoPtr = parentDataTypePtr.GetPtr<const IAddressTypeInfo>()){
				address += typeInfoPtr->GetShortName() + QStringLiteral(" ") + parentAddressInfoPtr->GetName() + QStringLiteral(",");
			}
		}
		address += typeShortName + QStringLiteral(" ") + addressInfoPtr->GetName();
	}

	dataObj.insert(QStringLiteral("AddressId"), QString::fromUtf8(addressPosition->GetObjectUuid()));
	dataObj.insert(QStringLiteral("ParentId"), QString::fromUtf8(parentId));
	dataObj.insert(QStringLiteral("TypeId"), QString::fromUtf8(addressInfoPtr->GetAddressTypeId()));
	dataObj.insert(QStringLiteral("TypeName"), typeName);
	dataObj.insert(QStringLiteral("Name"), addressInfoPtr->GetName());
	dataObj.insert(QStringLiteral("Address"), address);
	dataObj.insert(QStringLiteral("Description"), addressInfoPtr->GetDescription());
	dataObj.insert(QStringLiteral("Latitude"), addressInfoPtr->GetLatitude());
	dataObj.insert(QStringLiteral("Longitude"), addressInfoPtr->GetLongitude());

	return true;
}


istd::IChangeableUniquePtr CAddressControllerComp::CreateObjectFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			QByteArray& objectId,
			QString& errorMessage) const
{
	if (!m_addressInfoFactCompPtr.IsValid()){
		errorMessage = QObject::tr("Can not create Address: %1").arg(QString::fromUtf8(objectId));

		return nullptr;
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject(QByteArrayLiteral("input"));
	if (!inputParamPtr){
		errorMessage = QStringLiteral("Unable to create address object. Error: GraphQL input params is invalid.");
		SendErrorMessage(0, errorMessage, __func__);

		return nullptr;
	}

	objectId = GetObjectIdFromInputParams(*inputParamPtr);
	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	QByteArray itemData = inputParamPtr->GetParamArgumentValue(QByteArrayLiteral("item")).toByteArray();
	if (itemData.isEmpty()){
		errorMessage = QObject::tr("Can not create address: %1").arg(QString::fromUtf8(objectId));

		return nullptr;
	}

	istd::TDelPtr<CAddressElementInfo> addressInfoPtr = new CAddressElementInfo();
	if (!addressInfoPtr.IsValid()){
		errorMessage = QObject::tr("Unable to get an address info!");

		return nullptr;
	}

	QJsonDocument jsonDoc = QJsonDocument::fromJson(itemData);
	if (!jsonDoc.isObject()){
		errorMessage = QObject::tr("Invalid JSON input!");

		return nullptr;
	}

	QJsonObject itemObj = jsonDoc.object();
	addressInfoPtr->SetObjectUuid(objectId);

	if (itemObj.contains(QStringLiteral("parentId"))){
		QByteArray parentId = itemObj.value(QStringLiteral("parentId")).toVariant().toByteArray();

		imtbase::IObjectCollection::DataPtr adrDataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(parentId, adrDataPtr)){
			if (const auto* adrInfoPtr = adrDataPtr.GetPtr<const IAddressElementInfo>()){
				QByteArrayList parentIds = adrInfoPtr->GetParentIds();
				parentIds.append(parentId);
				addressInfoPtr->SetParentIds(parentIds);
			}
		}
	}

	if (itemObj.contains(QStringLiteral("typeId"))){
		addressInfoPtr->SetAddressTypeId(itemObj.value(QStringLiteral("typeId")).toVariant().toByteArray());
	}

	if (itemObj.contains(QStringLiteral("name"))){
		addressInfoPtr->SetName(itemObj.value(QStringLiteral("name")).toString());
	}

	if (itemObj.contains(QStringLiteral("description"))){
		addressInfoPtr->SetDescription(itemObj.value(QStringLiteral("description")).toString());
	}

	if (itemObj.contains(QStringLiteral("latitude"))){
		addressInfoPtr->SetLatitude(itemObj.value(QStringLiteral("latitude")).toDouble());
	}

	if (itemObj.contains(QStringLiteral("longitude"))){
		addressInfoPtr->SetLongitude(itemObj.value(QStringLiteral("longitude")).toDouble());
	}

	return addressInfoPtr.PopPtr();
}


bool CAddressControllerComp::UpdateObjectFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	QByteArray dummyId;
	istd::IChangeableUniquePtr tempObject = CreateObjectFromRequest(gqlRequest, dummyId, errorMessage);
	if (!tempObject.IsValid()){
		return false;
	}

	object.CopyFrom(*tempObject);

	return true;
}


} // namespace imtgeo


