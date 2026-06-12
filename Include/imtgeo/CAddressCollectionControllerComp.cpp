// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgeo/CAddressCollectionControllerComp.h>


// Qt includes
#include <QJsonArray>
#include <QJsonObject>

// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtgeo/CAddressElementInfo.h>


namespace imtgeo
{


bool CAddressCollectionControllerComp::SetupGqlItem(
		const imtgql::CGqlRequest& gqlRequest,
		QJsonObject& itemObj,
		const imtbase::IObjectCollectionIterator* objectCollectionIterator,
		QString& errorMessage) const
{
	if (objectCollectionIterator == nullptr){
		return false;
	}

	imtbase::IObjectCollection::DataPtr objectDataPtr;
	if (!objectCollectionIterator->GetObjectData(objectDataPtr)){
		return false;
	}

	const imtgeo::IAddressElementInfo* addressElementInfoPtr = objectDataPtr.GetPtr<const imtgeo::IAddressElementInfo>();
	const imtgeo::CPositionIdentifiable* addressPosition = objectDataPtr.GetPtr<const imtgeo::CPositionIdentifiable>();

	if (addressElementInfoPtr == nullptr || addressPosition == nullptr){
		return false;
	}

	const QByteArrayList informationIds = GetInformationIds(gqlRequest, QByteArrayLiteral("items"));
	if (informationIds.isEmpty()){
		return false;
	}

	for (const QByteArray& informationId : informationIds){
		QVariant elementInformation;

		if (informationId == QByteArrayLiteral("id")){
			elementInformation = QString::fromUtf8(addressPosition->GetObjectUuid());
		}
		else if (informationId == QByteArrayLiteral("fullAddress")){
			elementInformation = addressElementInfoPtr->GetFullAddress();
		}
		else if (informationId == QByteArrayLiteral("name")){
			elementInformation = addressElementInfoPtr->GetName();
		}
		else if (informationId == QByteArrayLiteral("typeId")){
			elementInformation = QString::fromUtf8(addressElementInfoPtr->GetAddressTypeId());
		}
		else if (informationId == QByteArrayLiteral("parentId")){
			const QByteArrayList parentsList = addressElementInfoPtr->GetParentIds();
			elementInformation = parentsList.isEmpty() ? QString() : QString::fromUtf8(parentsList.last());
		}
		else if (informationId == QByteArrayLiteral("parentIds")){
			const QByteArrayList parentsList = addressElementInfoPtr->GetParentIds();
			QString parentsStr;
			for (int i = 0; i < parentsList.count(); ++i){
				parentsStr.append(QString::fromUtf8(parentsList.at(i)));
				if (i < parentsList.count() - 1){
					parentsStr.append(QStringLiteral(","));
				}
			}
			elementInformation = parentsStr;
		}
		else if (informationId == QByteArrayLiteral("latitude")){
			elementInformation = addressElementInfoPtr->GetLatitude();
		}
		else if (informationId == QByteArrayLiteral("longitude")){
			elementInformation = addressElementInfoPtr->GetLongitude();
		}
		else{
			idoc::MetaInfoPtr metaInfo = objectCollectionIterator->GetDataMetaInfo();
			if (metaInfo.IsValid()){
				if (informationId == QByteArrayLiteral("added")){
					elementInformation = metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME);
				}
				else if (informationId == QByteArrayLiteral("lastModified")){
					elementInformation = metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME);
				}
			}
		}

		if (elementInformation.isNull()){
			elementInformation = GetObjectInformation(informationId, addressPosition->GetObjectUuid());
		}

		itemObj.insert(QString::fromUtf8(informationId.constData()), QJsonValue::fromVariant(elementInformation));
	}

	return true;
}


} // namespace imtgeo



