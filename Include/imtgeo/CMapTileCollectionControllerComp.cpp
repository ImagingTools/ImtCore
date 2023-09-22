#include <imtgeo/CMapTileCollectionControllerComp.h>


// STL includes
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


QVariant CMapTileCollectionControllerComp::GetObjectInformation(
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


imtbase::CTreeItemModel* CMapTileCollectionControllerComp::GetMetaInfo(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CMapTileCollectionControllerComp::ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* itemsModel = nullptr;
	imtbase::CTreeItemModel* notificationModel = nullptr;

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModelPtr->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else {
		dataModel = new imtbase::CTreeItemModel();
		itemsModel = new imtbase::CTreeItemModel();
		notificationModel = new imtbase::CTreeItemModel();

		const imtgql::CGqlObject* viewParamsGql = nullptr;
		const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();
		if (inputParams.size() > 0){
			viewParamsGql = inputParams.at(0).GetFieldArgumentObjectPtr("viewParams");
		}

		double zoomLevel = 0;
		double leftTopLat = 0;
		double leftTopLon = 0;
		double rightBottomLat = 0;
		double rightBottomLon = 0;

		QList<QPair<QPair<int, int>, int>> tileList;

		if (viewParamsGql != nullptr){
			QByteArray filterBA = viewParamsGql->GetFieldArgumentValue("FilterModel").toByteArray();
			if (!filterBA.isEmpty()){
				imtbase::CTreeItemModel generalModel;
				generalModel.CreateFromJson(filterBA);

				zoomLevel = generalModel.GetData("ZoomLevel").toDouble();
				leftTopLat = generalModel.GetData("LeftTopLat").toDouble();
				leftTopLon = generalModel.GetData("LeftTopLon").toDouble();
				rightBottomLat = generalModel.GetData("RightBottomLat").toDouble();
				rightBottomLon = generalModel.GetData("RightBottomLon").toDouble();

				tileList = getTileSet(QGeoCoordinate(leftTopLat, leftTopLon), QGeoCoordinate(rightBottomLat, rightBottomLon), zoomLevel + 1);
			}
		}

        {
			if (tileList.count()){
				int itemIndex = 0;
				int z = tileList.at(0).second;
				for (int i = 0; i < tileList.size(); i++){

					double lat = 0;
					double lon = 0;

					lat = tiley2lat(tileList.at(i).first.second, z);
					lon = tilex2long(tileList.at(i).first.first, z);

					itemIndex = itemsModel->InsertNewItem();
					itemsModel->SetData("Id", itemIndex, itemIndex);
					itemsModel->SetData("Latitude", lat, itemIndex);
					itemsModel->SetData("Longitude", lon, itemIndex);

				}
			}

        }


		itemsModel->SetIsArray(true);
		dataModel->SetExternTreeModel("items", itemsModel);
		dataModel->SetExternTreeModel("notification", notificationModel);
	}
	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();


}


} // namespace imtgeo



