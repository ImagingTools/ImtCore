// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgeo/CAddressCollectionControllerComp.h>


// Qt includes
#include <QJsonArray>
#include <QJsonValue>

// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <idoc/IDocumentMetaInfo.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/CCollectionFilter.h>
#include <imtgeo/CAddressElementInfo.h>
#include <imtgeo/CAddressTypeInfo.h>
#include <imtdb/CSqlDatabaseObjectCollectionComp.h>
#include <imtbase/CCollectionInfo.h>


namespace imtgeo
{


QVariant CAddressCollectionControllerComp::GetObjectInformation(
			const QByteArray& informationId,
			const QByteArray& objectId) const
{
	idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(objectId);
	if (metaInfo.IsValid()){
		if (informationId == QByteArray("Added")){
			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME);
		}

		if (informationId == QByteArray("LastModified")){
			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME);
		}
	}

	return QVariant();
}


QJsonObject CAddressCollectionControllerComp::GetMetaInfo(
	const imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return QJsonObject();
}


QJsonObject CAddressCollectionControllerComp::ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	QJsonObject rootObj;
	QJsonObject dataObj;

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		QJsonObject errorsObj;
		errorsObj.insert(QStringLiteral("message"), QJsonValue::fromVariant(errorMessage));
		rootObj.insert(QStringLiteral("errors"), errorsObj);
	}
	else {
		QJsonObject notificationObj;

		const imtgql::CGqlParamObject* viewParamsGql = nullptr;
		const imtgql::CGqlParamObject& inputParams = gqlRequest.GetParams();
		const imtgql::CGqlParamObject* inputObject = inputParams.GetParamArgumentObjectPtr("input");
		if (inputObject != nullptr){
			viewParamsGql = inputObject->GetParamArgumentObjectPtr("viewParams");
		}

		imtbase::CCollectionFilter filter;
		//imtbase::CCollectionFilter filterOnParentId;
		imtbase::CCollectionFilter filterOnParentIds;
		imtbase::CCollectionFilter filterOnTypeId;
		int offset = 0;
		int count = -1;
		QString parentIds = "";
		QString filterText = "";
		QString typeId = "";
		if (viewParamsGql != nullptr){
			offset = viewParamsGql->GetParamArgumentValue("offset").toInt();
			count = viewParamsGql->GetParamArgumentValue("count").toInt();

			QByteArray filterBA = viewParamsGql->GetParamArgumentValue("FilterModel").toByteArray();
			if (!filterBA.isEmpty()){
				imtbase::CTreeItemModel generalModel;
				generalModel.CreateFromJson(filterBA);

				parentIds = generalModel.GetData("ParentIds").toString();
				if (!parentIds.isEmpty()){
					QStringList parentIdList = parentIds.split(",");
					QJsonArray jsonArray;
					QJsonDocument json_document;
					for (int i = 0; i < parentIdList.count(); i++){
						jsonArray.append(parentIdList.at(i));
					}
					json_document = QJsonDocument(jsonArray);

					//qDebug() << "jsonParents::: " << qPrintable(QJsonDocument(jsonArray).toJson(QJsonDocument::Compact));

					QByteArrayList filteringOnParentIdsInfoIds;
					filteringOnParentIdsInfoIds << "ParentIds";
					filterOnParentIds.SetFilteringInfoIds(filteringOnParentIdsInfoIds);
					filterOnParentIds.SetTextFilter(qPrintable(QJsonDocument(jsonArray).toJson(QJsonDocument::Compact)));
				}

				//                if (!parentId.isEmpty()){
				//                    QByteArrayList filteringOnParentIdInfoIds;
				//                    filteringOnParentIdInfoIds << "ParentId";
				//                    filterOnParentId.SetFilteringInfoIds(filteringOnParentIdInfoIds);
				//                    filterOnParentId.SetTextFilter(parentId);
				//                }


				typeId = generalModel.GetData("TypeId").toString();

				if (!typeId.isEmpty()){
					QByteArrayList filteringOnTypeIdInfoIds;
					filteringOnTypeIdInfoIds << "TypeId";
					filterOnTypeId.SetFilteringInfoIds(filteringOnTypeIdInfoIds);
					filterOnTypeId.SetTextFilter(typeId);
				}

				imtbase::CTreeItemModel* filterModel = generalModel.GetTreeItemModel("FilterIds");
				if (filterModel != nullptr){
					QByteArrayList filteringInfoIds;
					for (int i = 0; i < filterModel->GetItemsCount(); i++){
						QByteArray headerId = filterModel->GetData("id", i).toByteArray();
						if (!headerId.isEmpty()){
							filteringInfoIds << headerId;
						}
					}
					filter.SetFilteringInfoIds(filteringInfoIds);
				}

				filterText = generalModel.GetData("TextFilter").toString();
				if (!filterText.isEmpty()){

					filterText = filterText.trimmed();
					if (filterText.startsWith(",")){
						filterText = filterText.replace(0, 1, "");
						filterText = filterText.trimmed();
					}
					if (filterText.endsWith(",")){
						filterText = filterText.replace(filterText.length() - 1, 1, "");
						filterText = filterText.trimmed();
					}

					//qDebug() << "filter::: Text::" << filterText;
					filter.SetTextFilter(filterText);
				}
				imtbase::CTreeItemModel* sortModel = generalModel.GetTreeItemModel("Sort");
				if (sortModel != nullptr){
					QByteArray headerId = sortModel->GetData("HeaderId").toByteArray();
					QByteArray sortOrder = sortModel->GetData("SortOrder").toByteArray();
					if (!headerId.isEmpty() && !sortOrder.isEmpty()){
						filter.SetSortingOrder(sortOrder == "ASC" ? imtbase::ICollectionFilter::SO_ASC : imtbase::ICollectionFilter::SO_DESC);
						filter.SetSortingInfoIds(QByteArrayList() << headerId);
					}
				}
			}
		}
		iprm::CParamsSet filterParams;
		filterParams.SetEditableParameter("Filter", &filter);
		filterParams.SetEditableParameter("ParentIds", &filterOnParentIds);
		filterParams.SetEditableParameter("TypeId", &filterOnTypeId);

		int pagesCount = std::ceil(m_objectCollectionCompPtr->GetElementsCount(&filterParams) / (double)count);
		if (pagesCount < 0){
			pagesCount = 1;
		}

		notificationObj.insert(QStringLiteral("PagesCount"), QJsonValue::fromVariant(pagesCount));


		imtdb::CSqlDatabaseObjectCollectionComp* objectCollectionCompPtr = dynamic_cast<imtdb::CSqlDatabaseObjectCollectionComp*>(m_objectCollectionCompPtr.GetPtr());
		if (objectCollectionCompPtr == nullptr){
			return QJsonObject();

		}
		//        int countElement = objectCollectionCompPtr->GetElementsCount(&filterParams);
		//        if (!countElement){
		//            return nullptr;
		//        }

		istd::TDelPtr<imtbase::IObjectCollectionIterator> objectCollectionIterator(
					objectCollectionCompPtr->CreateObjectCollectionIterator(QByteArray(), offset, count, &filterParams));

		QJsonArray itemsArray;

		if (objectCollectionIterator != nullptr){
			while (objectCollectionIterator->Next()){
				imtbase::IObjectCollection::DataPtr objectDataPtr;
				if (objectCollectionIterator->GetObjectData(objectDataPtr)){
					QJsonObject itemObj;

					const imtgeo::IAddressElementInfo* addressElementInfoPtr = nullptr;
					const imtgeo::CPositionIdentifiable* addressPosition = nullptr;
					addressElementInfoPtr = dynamic_cast<const imtgeo::IAddressElementInfo*>(objectDataPtr.GetPtr());
					addressPosition = dynamic_cast<const imtgeo::CPositionIdentifiable*>(objectDataPtr.GetPtr());

					QString fullAddress = addressElementInfoPtr->GetAddress();
					QString name = addressElementInfoPtr->GetName();
					QByteArray addressId = addressPosition->GetObjectUuid();
					QByteArray typeAddressId = addressElementInfoPtr->GetAddressTypeId();
					QList<QByteArray> parentsList = addressElementInfoPtr->GetParentIds();
					QByteArray addressParentId = parentsList.isEmpty() ? QByteArray() : parentsList.last();
					QString parentsStr = QString();
					double lat = addressElementInfoPtr->GetLatitude();
					double lon = addressElementInfoPtr->GetLongitude();
					for (int i = 0; i < parentsList.count(); i++){
						parentsStr.append(QString(parentsList.at(i)));
						if (i < parentsList.count() - 1){
							parentsStr.append(",");
						}
					}

					itemObj.insert(QStringLiteral("FullAddress"), QJsonValue::fromVariant(fullAddress));
					itemObj.insert(QStringLiteral("Id"), QJsonValue::fromVariant(QVariant(addressId)));
					itemObj.insert(QStringLiteral("Name"), QJsonValue::fromVariant(name));
					itemObj.insert(QStringLiteral("TypeId"), QJsonValue::fromVariant(QVariant(typeAddressId)));
					itemObj.insert(QStringLiteral("ParentId"), QJsonValue::fromVariant(QVariant(addressParentId)));
					itemObj.insert(QStringLiteral("ParentIds"), QJsonValue::fromVariant(parentsStr));
					itemObj.insert(QStringLiteral("Latitude"), QJsonValue::fromVariant(lat));
					itemObj.insert(QStringLiteral("Longitude"), QJsonValue::fromVariant(lon));

					itemsArray.append(itemObj);
				}
			}//while
		}//ITERATOR

		if ((itemsArray.count() == 0) && !parentIds.isEmpty() && filterText.isEmpty()){
			notificationObj.insert(QStringLiteral("Close"), QJsonValue::fromVariant(true));
		}

		dataObj.insert(QStringLiteral("items"), itemsArray);
		dataObj.insert(QStringLiteral("notification"), notificationObj);
	}
	rootObj.insert(QStringLiteral("data"), dataObj);

	return rootObj;
}


} // namespace imtgeo



