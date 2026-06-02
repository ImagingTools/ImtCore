// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgeo/CMapTileCollectionControllerComp.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection.h>


// Qt includes
#include <QJsonArray>
#include <QJsonValue>

// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <iprm/CParamsSet.h>


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

		if (informationId == QByteArray("LastModified")){
			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME);
		}
	}

	return QVariant();
}


QJsonObject CMapTileCollectionControllerComp::GetMetaInfo(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return QJsonObject();
}


QJsonObject CMapTileCollectionControllerComp::ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	QJsonObject rootObj;
	QJsonObject dataObj;

	if (!errorMessage.isEmpty()){
		QJsonObject errorsObj;
		errorsObj.insert(QStringLiteral("message"), QJsonValue::fromVariant(errorMessage));
		rootObj.insert(QStringLiteral("errors"), errorsObj);
	}
	else {
		QJsonObject notificationObj;

		const imtgql::CGqlParamObject* viewParamsGql = nullptr;
		QList<imtgql::CGqlParamObject> inputParams;
		inputParams.append(gqlRequest.GetParams());
		if (!inputParams.isEmpty()){
			viewParamsGql = inputParams.at(0).GetParamArgumentObjectPtr("viewParams");
		}

		double zoomLevel = 0;
		double leftTopLat = 0;
		double leftTopLon = 0;
		double rightBottomLat = 0;
		double rightBottomLon = 0;

		QList<QPair<QPair<qint64, qint64>, int>> tileList;

		if (viewParamsGql != nullptr){
			QByteArray filterBA = viewParamsGql->GetParamArgumentValue("FilterModel").toByteArray();
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

		QJsonArray itemsArray;

		{
			if (!tileList.isEmpty()){
				int z = tileList.at(0).second;
				for (int i = 0; i < tileList.size(); i++){
					QJsonObject itemObj;

					double lat = 0;
					double lon = 0;

					lat = tiley2lat(tileList.at(i).first.second, z);
					lon = tilex2long(tileList.at(i).first.first, z);

					itemObj.insert(QStringLiteral("Id"), QJsonValue::fromVariant(i));
					itemObj.insert(QStringLiteral("Latitude"), QJsonValue::fromVariant(lat));
					itemObj.insert(QStringLiteral("Longitude"), QJsonValue::fromVariant(lon));

					itemsArray.append(itemObj);
				}
			}
		}

		dataObj.insert(QStringLiteral("items"), itemsArray);
		dataObj.insert(QStringLiteral("notification"), notificationObj);
	}
	rootObj.insert(QStringLiteral("data"), dataObj);

	return rootObj;
}


} // namespace imtgeo



