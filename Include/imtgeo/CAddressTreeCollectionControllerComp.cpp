// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgeo/CAddressTreeCollectionControllerComp.h>


// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/CCollectionFilter.h>
#include <imtgeo/CAddressElementInfo.h>
#include <imtdb/CSqlDatabaseObjectCollectionComp.h>
#include <imtbase/CCollectionInfo.h>


namespace imtgeo
{


QVariant CAddressTreeCollectionControllerComp::GetObjectInformation(
		const QByteArray &informationId,
		const QByteArray &objectId) const
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


imtbase::CTreeItemModel* CAddressTreeCollectionControllerComp::GetMetaInfo(
		const imtgql::CGqlRequest& /*gqlRequest*/,
		QString& /*errorMessage*/) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CAddressTreeCollectionControllerComp::ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* itemsModel = nullptr;
	imtbase::CTreeItemModel* notificationModel = nullptr;


	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModelPtr->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else {
		dataModel = new imtbase::CTreeItemModel();
		itemsModel = new imtbase::CTreeItemModel();
		notificationModel = new imtbase::CTreeItemModel();

		const imtgql::CGqlParamObject* viewParamsGql = nullptr;
		const imtgql::CGqlParamObject& inputParams = gqlRequest.GetParams();
		const imtgql::CGqlParamObject* inputObject = inputParams.GetParamArgumentObjectPtr("input");
		if (inputObject != nullptr){
			viewParamsGql = inputObject->GetParamArgumentObjectPtr("viewParams");
		}

		iprm::CParamsSet filterParams;
		imtbase::CCollectionFilter filter;
		imtbase::CCollectionFilter filterOnParentIds;
		imtbase::CCollectionFilter filterOnParentId;
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

				if (!parentIds.isEmpty()){
					QStringList parentIdList = parentIds.split(",");
					QJsonArray jsonArray;
					QJsonDocument json_document;
					for(int i = 0; i < parentIdList.count(); i++){
						jsonArray.append(parentIdList.at(i));
					}
					json_document = QJsonDocument(jsonArray);

					QByteArrayList filteringOnParentIdsInfoIds;
					filteringOnParentIdsInfoIds << "ParentIds";
					filterOnParentIds.SetFilteringInfoIds(filteringOnParentIdsInfoIds);
					filterOnParentIds.SetTextFilter(qPrintable(QJsonDocument(jsonArray).toJson(QJsonDocument::Compact)));
				}

				if(generalModel.ContainsKey("ParentId")){
					QString parentId = generalModel.GetData("ParentId").toString();
					QByteArrayList filteringOnParentIdInfoIds;
					filteringOnParentIdInfoIds << "ParentId";
					filterOnParentId.SetFilteringInfoIds(filteringOnParentIdInfoIds);
					filterOnParentId.SetTextFilter(parentId);
					filterParams.SetEditableParameter("ParentId", &filterOnParentId);
				}

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
						QByteArray headerId = filterModel->GetData("Id", i).toByteArray();
						if (!headerId.isEmpty()){
							filteringInfoIds << headerId;
						}
					}
					filter.SetFilteringInfoIds(filteringInfoIds);
				}

				filterText = generalModel.GetData("TextFilter").toString();
				if (!filterText.isEmpty()){

					filterText = filterText.trimmed();
					if(filterText.startsWith(",")){
						filterText = filterText.replace(0,1,"");
						filterText = filterText.trimmed();
					}
					if(filterText.endsWith(",")){
						filterText = filterText.replace(filterText.length() -1, 1,"");
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

		filterParams.SetEditableParameter("Filter", &filter);
		filterParams.SetEditableParameter("ParentIds", &filterOnParentIds);
		filterParams.SetEditableParameter("TypeId", &filterOnTypeId);

		int pagesCount = std::ceil(m_objectCollectionCompPtr->GetElementsCount(&filterParams) / (double)count);
		if (pagesCount < 0){
			pagesCount = 1;
		}

		notificationModel->SetData("PagesCount", pagesCount);


		imtdb::CSqlDatabaseObjectCollectionComp* objectCollectionCompPtr = dynamic_cast<imtdb::CSqlDatabaseObjectCollectionComp*>(m_objectCollectionCompPtr.GetPtr());
		if (objectCollectionCompPtr == nullptr){
			return nullptr;
		}

		istd::TDelPtr<imtbase::IObjectCollectionIterator> objectCollectionIterator(
					objectCollectionCompPtr->CreateObjectCollectionIterator(QByteArray(), offset, count, &filterParams));

		if (objectCollectionIterator != nullptr){
			while (objectCollectionIterator->Next()){
				imtbase::IObjectCollection::DataPtr objectDataPtr;
				if (objectCollectionIterator->GetObjectData(objectDataPtr)){
					int itemIndex = itemsModel->InsertNewItem();
					if (itemIndex >= 0){

						const IAddressElementInfo* addressElementInfoPtr = nullptr;
						const CPositionIdentifiable* addressPosition = nullptr;
						addressElementInfoPtr = dynamic_cast<const IAddressElementInfo*>(objectDataPtr.GetPtr());
						addressPosition = dynamic_cast<const CPositionIdentifiable*>(objectDataPtr.GetPtr());

						QString fullAddress = addressElementInfoPtr->GetAddress();
						QString name = addressElementInfoPtr->GetName();
						QByteArray addressId = addressPosition->GetObjectUuid();
						QByteArray typeAddressId = addressElementInfoPtr->GetAddressTypeId();
						QList<QByteArray> parentsList = addressElementInfoPtr->GetParentIds();
						QByteArray addressParentId = parentsList.isEmpty() ? QByteArray() : parentsList.last();
						QString parentsStr = QString();
						double lat = addressElementInfoPtr->GetLatitude();
						double lon = addressElementInfoPtr->GetLongitude();
						for(int i = 0; i < parentsList.count(); i++){
							parentsStr.append(QString(parentsList.at(i)));
							if(i < parentsList.count() -1){
								parentsStr.append(",");
							}
						}

						//qDebug() << "fullAddress::: "  << fullAddress;

						itemsModel->SetData("FullAddress", fullAddress, itemIndex);
						itemsModel->SetData("Id", addressId, itemIndex);
						itemsModel->SetData("Name", name, itemIndex);
						itemsModel->SetData("TypeId", typeAddressId, itemIndex);
						itemsModel->SetData("ParentId", addressParentId, itemIndex);
						itemsModel->SetData("ParentIds", parentsStr, itemIndex);
						itemsModel->SetData("Latitude", lat, itemIndex);
						itemsModel->SetData("Longitude", lon, itemIndex);

						bool isNode = false;
						isNode = addressElementInfoPtr->GetHasChildren();
						QString typeId_ = isNode ? "Node" : "Doc";
						itemsModel->SetData("TypeId__", typeId_, itemIndex);
						itemsModel->SetData("HasChildren__", isNode, itemIndex);

					}

				}
			}//while

		}//ITERATOR

		if((itemsModel->GetItemsCount() == 0) && !parentIds.isEmpty() && filterText.isEmpty()){
			notificationModel->SetData("Close",true);
		}

		itemsModel->SetIsArray(true);
		dataModel->SetExternTreeModel("items", itemsModel);
		dataModel->SetExternTreeModel("notification", notificationModel);
	}
	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
}

bool CAddressTreeCollectionControllerComp::checkHasChildren(const QString& id) const
{
	QString sql = QString("(SELECT * FROM public.\"AddressElements\" WHERE '[\"%1\"]' <@ (\"ParentIds\") LIMIT 1)")
					  .arg(id);

	QSqlQuery sqlQuery = m_engineCompChr3->ExecSqlQuery(sql.toUtf8());
	return (sqlQuery.size() > 0);
}

} // namespace imtgeo



