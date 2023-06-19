#include <imtgeo/CDeviceMapClusterCollectionControllerComp.h>

#include <cmath>

// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <idoc/IDocumentMetaInfo.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/CCollectionFilter.h>
#include <imtgeo/CAddressElementInfo.h>


namespace imtgeo
{


QVariant CDeviceMapClusterCollectionControllerComp::GetObjectInformation(
        const QByteArray &informationId,
        const QByteArray &objectId) const
{
    idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(objectId);
    if (metaInfo.IsValid()){
        if (informationId == QByteArray("Added")){
            return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME);
        }
        else if (informationId == QByteArray("LastModified")){
            return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME);
        }
    }

    return QVariant();
}


imtbase::CTreeItemModel* CDeviceMapClusterCollectionControllerComp::GetMetaInfo(
        const imtgql::CGqlRequest& gqlRequest,
        QString& errorMessage) const
{
    istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
    return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CDeviceMapClusterCollectionControllerComp::ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
    istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
    imtbase::CTreeItemModel* dataModel = nullptr;
    imtbase::CTreeItemModel* itemsModel = nullptr;
    imtbase::CTreeItemModel* notificationModel = nullptr;



    //qDebug() << "ТЕСТ ТАЙЛОВ " << long2tilex(73.35,10) << "," << lat2tiley(55,10);


    //qDebug() << "ЗАПРОС КЛАСТЕРОВ";

    //	if (!m_objectCollectionCompPtr.IsValid()){
    //		errorMessage = QObject::tr("Internal error").toUtf8();
    //	}

    if (!errorMessage.isEmpty()){
        imtbase::CTreeItemModel* errorsItemModel = rootModelPtr->AddTreeModel("errors");
        errorsItemModel->SetData("message", errorMessage);
    }
    else{
        dataModel = new imtbase::CTreeItemModel();
        itemsModel = new imtbase::CTreeItemModel();
        notificationModel = new imtbase::CTreeItemModel();

        const imtgql::CGqlObject* viewParamsGql = nullptr;
		const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();
		if (inputParams.size() > 0){
			viewParamsGql = inputParams.at(0).GetFieldArgumentObjectPtr("viewParams");
        }

        //		imtbase::CCollectionFilter filter;
        //		imtbase::CCollectionFilter filterOnParentId;
        //		imtbase::CCollectionFilter filterOnTypeId;
        //		int offset = 0, count = -1;

        double zoomLevel = 0;
        double leftTopLat = 0;
        double leftTopLon = 0;
        double rightBottomLat = 0;
        double rightBottomLon = 0;

        QList<QPair<QPair<int, int>, int>> tileList;
        //		QString parentId = "";
        //		QString typeId = "";

        if (viewParamsGql != nullptr){

            //			offset = viewParamsGql->GetFieldArgumentValue("Offset").toInt();
            //			count = viewParamsGql->GetFieldArgumentValue("Count").toInt();

            QByteArray filterBA = viewParamsGql->GetFieldArgumentValue("FilterModel").toByteArray();
            if (!filterBA.isEmpty()){
                imtbase::CTreeItemModel generalModel;
                generalModel.CreateFromJson(filterBA);

                zoomLevel = generalModel.GetData("ZoomLevel").toDouble();
                leftTopLat = generalModel.GetData("LeftTopLat").toDouble();
                leftTopLon = generalModel.GetData("LeftTopLon").toDouble();
                rightBottomLat = generalModel.GetData("RightBottomLat").toDouble();
                rightBottomLon = generalModel.GetData("RightBottomLon").toDouble();

                tileList = getTileSet(QGeoCoordinate(leftTopLat, leftTopLon), QGeoCoordinate(rightBottomLat, rightBottomLon), zoomLevel+1);

                //qDebug() << "ZoomLevel_ " << zoomLevel << "leftTopLat_ " << leftTopLat << "leftTopLon_ " << leftTopLon << "rigthBottomLat_ " << rigthBottomLat << "rigthBottomLon_ " << rigthBottomLon;


                //				parentId = generalModel.GetData("ParentId").toString();
                //				typeId = generalModel.GetData("TypeId").toString();
                //				if (!parentId.isEmpty()){
                //					QByteArrayList filteringOnParentIdInfoIds;
                //					filteringOnParentIdInfoIds << "ParentId";
                //					filterOnParentId.SetFilteringInfoIds(filteringOnParentIdInfoIds);
                //					filterOnParentId.SetTextFilter(parentId);
                //				}
                //				if (!typeId.isEmpty()){
                //					QByteArrayList filteringOnTypeIdInfoIds;
                //					filteringOnTypeIdInfoIds << "Type";
                //					filterOnTypeId.SetFilteringInfoIds(filteringOnTypeIdInfoIds);
                //					filterOnTypeId.SetTextFilter(typeId);
                //				}

                //				imtbase::CTreeItemModel* filterModel = generalModel.GetTreeItemModel("FilterIds");
                //				if (filterModel != nullptr){
                //					QByteArrayList filteringInfoIds;
                //					for (int i = 0; i < filterModel->GetItemsCount(); i++){
                //						QByteArray headerId = filterModel->GetData("Id", i).toByteArray();
                //						if (!headerId.isEmpty()){
                //							filteringInfoIds << headerId;
                //						}
                //					}
                //					filter.SetFilteringInfoIds(filteringInfoIds);
                //				}

                //				QString filterText = generalModel.GetData("TextFilter").toString();
                //				if (!filterText.isEmpty()){
                //					filter.SetTextFilter(filterText);
                //				}
                //				imtbase::CTreeItemModel* sortModel = generalModel.GetTreeItemModel("Sort");
                //				if (sortModel != nullptr){
                //					QByteArray headerId = sortModel->GetData("HeaderId").toByteArray();
                //					QByteArray sortOrder = sortModel->GetData("SortOrder").toByteArray();
                //					if (!headerId.isEmpty() && !sortOrder.isEmpty()){
                //						filter.SetSortingOrder(sortOrder == "ASC" ? imtbase::ICollectionFilter::SO_ASC : imtbase::ICollectionFilter::SO_DESC);
                //						filter.SetSortingInfoIds(QByteArrayList() << headerId);
                //					}
                //				}



            }
        }


        //		iprm::CParamsSet filterParams;
        //		filterParams.SetEditableParameter("Filter", &filter);
        //		filterParams.SetEditableParameter("ParentId", &filterOnParentId);
        //		filterParams.SetEditableParameter("Type", &filterOnTypeId);

        //		int pagesCount = std::ceil(m_objectCollectionCompPtr->GetElementsCount(&filterParams) / (double)count);
        //		if (pagesCount < 0){
        //			pagesCount = 1;
        //		}

        //		notificationModel->SetData("PagesCount", pagesCount);

        //		imtbase::IObjectCollection* subCollection = m_objectCollectionCompPtr->CreateSubCollection(offset, count, &filterParams);
        //		imtbase::ICollectionInfo::Ids collectionIds = subCollection->GetElementIds(0, -1);

        //        for (const QByteArray& collectionId : collectionIds){
        //			int itemIndex = itemsModel->InsertNewItem();

        //            const imtgeo::IAddressElementInfo* addressElementInfoPtr = dynamic_cast<const imtgeo::IAddressElementInfo*>(subCollection->GetObjectPtr(collectionId));

        //            QByteArray addressId = addressElementInfoPtr->GetId();
        //			QString name = addressElementInfoPtr->GetName();
        //            QString address = addressElementInfoPtr->GetFullAddress();
        //			int typeAddressId = addressElementInfoPtr->GetType();
        //			QByteArray parentId = addressElementInfoPtr->GetParentId();
        //			itemsModel->SetData("Id", addressId, itemIndex);
        //			itemsModel->SetData("Name", name, itemIndex);
        //            itemsModel->SetData("FullAddress", address, itemIndex);
        //			itemsModel->SetData("TypeId", typeAddressId, itemIndex);
        //			itemsModel->SetData("ParentId", parentId, itemIndex);
        //		}


        //        {//test
        //            int itemIndex = 0;

        //            itemIndex = itemsModel->InsertNewItem();
        //            itemsModel->SetData("Id", itemIndex, itemIndex);
        //            itemsModel->SetData("Positive", 1000, itemIndex);
        //            itemsModel->SetData("Negative", 0, itemIndex);
        //            itemsModel->SetData("Latitude", 55, itemIndex);
        //            itemsModel->SetData("Longitude", 73.30, itemIndex);

        //            itemIndex = itemsModel->InsertNewItem();
        //            itemsModel->SetData("Id", itemIndex, itemIndex);
        //            itemsModel->SetData("Positive", 500, itemIndex);
        //            itemsModel->SetData("Negative", 40, itemIndex);
        //            itemsModel->SetData("Latitude", 55, itemIndex);
        //            itemsModel->SetData("Longitude", 73.35, itemIndex);

        //            itemIndex = itemsModel->InsertNewItem();
        //            itemsModel->SetData("Id", itemIndex, itemIndex);
        //            itemsModel->SetData("Positive", 0, itemIndex);
        //            itemsModel->SetData("Negative", 8, itemIndex);
        //            itemsModel->SetData("Latitude", 55, itemIndex);
        //            itemsModel->SetData("Longitude", 73.40, itemIndex);

        //        }//test

        //        {//test
        //            int itemIndex = 0;
        //            double lat = leftTopLat;
        //            double lon = leftTopLon;
        //            int counter_lat = 0;
        //            int counter_lon = 0;
        //            double latIncr = (rightBottomLat - leftTopLat)/5;
        //            double lonIncr = (rightBottomLon - leftTopLon)/5;
        //            while(counter_lat < 5){
        //                lat += latIncr;
        //                counter_lon = 0;
        //                lon = leftTopLon;
        //                while(counter_lon < 5){
        //                    lon += lonIncr;
        //                    itemIndex = itemsModel->InsertNewItem();
        //                    itemsModel->SetData("Id", itemIndex, itemIndex);
        //                    itemsModel->SetData("Positive", lat, itemIndex);
        //                    itemsModel->SetData("Negative", lon, itemIndex);
        //                    itemsModel->SetData("Latitude", lat, itemIndex);
        //                    itemsModel->SetData("Longitude", lon, itemIndex);

        //                    counter_lon++;
        //                }
        //                counter_lat++;
        //            }


        //        }//test

        {//test
            if(tileList.count()){
                int itemIndex = 0;
                int z = tileList.at(0).second;
                for(int i = 0; i < tileList.size(); i++){

                    double lat = 0;
                    double lon = 0;

                    lat = tiley2lat(tileList.at(i).first.second, z);
                    lon = tilex2long(tileList.at(i).first.first, z);

                    itemIndex = itemsModel->InsertNewItem();
                    itemsModel->SetData("Id", itemIndex, itemIndex);
                    itemsModel->SetData("Positive", 100000, itemIndex);
                    itemsModel->SetData("Negative", 100000, itemIndex);
                    itemsModel->SetData("Latitude", lat, itemIndex);
                    itemsModel->SetData("Longitude", lon, itemIndex);

                }
            }

        }//test


        itemsModel->SetIsArray(true);
        dataModel->SetExternTreeModel("items", itemsModel);
        dataModel->SetExternTreeModel("notification", notificationModel);
    }
    rootModelPtr->SetExternTreeModel("data", dataModel);

    return rootModelPtr.PopPtr();


}

int CDeviceMapClusterCollectionControllerComp::long2tilex(double lon, int z) const
{
    return (int)(floor((lon + 180.0) / 360.0 * (1 << z)));
}

int CDeviceMapClusterCollectionControllerComp::lat2tiley(double lat, int z) const
{
    const double pi = acos(-1.0);

    double latrad = lat * pi/180.0;
    return (int)(floor((1.0 - asinh(tan(latrad)) / pi) / 2.0 * (1 << z)));
}

double CDeviceMapClusterCollectionControllerComp::tilex2long(int x, int z) const
{
    return x / (double)(1 << z) * 360.0 - 180;
}

double CDeviceMapClusterCollectionControllerComp::tiley2lat(int y, int z) const
{
    const double pi = acos(-1.0);

    double n = pi - 2.0 * pi * y / (double)(1 << z);
    return 180.0 / pi * atan(0.5 * (exp(n) - exp(-n)));
}

QList<QPair<QPair<int, int>, int> > CDeviceMapClusterCollectionControllerComp::getTileSet(QGeoCoordinate coordLeftTop, QGeoCoordinate coordRightBottom, int z) const
{
    qDebug() << "НАБОР ТАЙЛОВ";

    QList<QPair<QPair<int, int>, int>> tileList;

    int minX = long2tilex(coordLeftTop.longitude(),z);
    int minY = lat2tiley(coordLeftTop.latitude(),z);
    int maxX = long2tilex(coordRightBottom.longitude(),z);
    int maxY = lat2tiley(coordRightBottom.latitude(),z);

    //qDebug() << "Тайлы Границы " << minX << " " << minY << " " << maxX << " " << maxY;

    for(int x = minX; x <= maxX; x++){
        for(int y = minY; y <= maxY; y++){
            QPair<int, int> pairX_Y = qMakePair(x,y);
            QPair<QPair<int,int>,int> pairXY_Z = qMakePair(pairX_Y, z);
            tileList.append(pairXY_Z);
        }

    }

    //    for(int i = 0; i < tileList.size(); i++){
    //        qDebug() << "Тайлы " << tileList.at(i).first.first << " " << tileList.at(i).first.second << " " << tileList.at(i).second;

    //    }


    return tileList;

}

QPair<QPair<int, int>, int> CDeviceMapClusterCollectionControllerComp::getTile(QGeoCoordinate coord, int z) const
{
    QPair<QPair<int, int>, int> tileData;

    int x = long2tilex(coord.longitude(),z);
    int y = lat2tiley(coord.latitude(),z);
    QPair<int, int> coordPair = qMakePair(x,y);
    tileData = qMakePair(coordPair,z);

    return tileData;
}


} // namespace imtgeo



