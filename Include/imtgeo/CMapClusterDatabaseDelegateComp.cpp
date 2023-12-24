#include <imtgeo/CMapClusterDatabaseDelegateComp.h>


// ACF includes
#include <imod/TModelWrap.h>
#include <iprm/TParamsPtr.h>
#include <idoc/CStandardDocumentMetaInfo.h>


namespace imtgeo
{


// public methods

// reimplemented (imtdb::CSqlJsonDatabaseDelegateComp)

bool CMapClusterDatabaseDelegateComp::CreateFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
{
	QString objectFilterQuery;
	bool retVal = true;

	QString zoomLevelFilterQuery;
	iprm::TParamsPtr<imtbase::ICollectionFilter> zoomLevelFilterParamPtr(&filterParams, "ZoomLevel");

	QString leftTopLatFilterQuery;
	iprm::TParamsPtr<imtbase::ICollectionFilter> leftTopLatFilterParamPtr(&filterParams, "LeftTopLat");

	QString leftTopLonFilterQuery;
	iprm::TParamsPtr<imtbase::ICollectionFilter> leftTopLonFilterParamPtr(&filterParams, "LeftTopLon");

	QString rightBottomLatFilterQuery;
	iprm::TParamsPtr<imtbase::ICollectionFilter> rightBottomLatFilterParamPtr(&filterParams, "RightBottomLat");

	QString rightBottomLonFilterQuery;
	iprm::TParamsPtr<imtbase::ICollectionFilter> rightBottomLonFilterParamPtr(&filterParams, "RightBottomLon");


	if (zoomLevelFilterParamPtr.IsValid() && zoomLevelFilterParamPtr->GetTextFilter() != ""){
		zoomLevelFilterQuery = QString("(\"Document\"->>'ZoomLevel')::INTEGER = %1").arg(zoomLevelFilterParamPtr->GetTextFilter().toDouble());
	}

	//
	if (leftTopLatFilterParamPtr.IsValid() && leftTopLatFilterParamPtr->GetTextFilter() != ""){
		leftTopLatFilterQuery = QString("(\"Document\"->>'Latitude')::double precision < %1").arg(leftTopLatFilterParamPtr->GetTextFilter().toDouble());
	}
	if (leftTopLonFilterParamPtr.IsValid() && leftTopLonFilterParamPtr->GetTextFilter() != ""){
		leftTopLonFilterQuery = QString("(\"Document\"->>'Longitude')::double precision > %1").arg(leftTopLonFilterParamPtr->GetTextFilter().toDouble());
	}
	if (rightBottomLatFilterParamPtr.IsValid() && rightBottomLatFilterParamPtr->GetTextFilter() != ""){
		rightBottomLatFilterQuery = QString("(\"Document\"->>'Latitude')::double precision > %1").arg(rightBottomLatFilterParamPtr->GetTextFilter().toDouble());
	}
	if (rightBottomLonFilterParamPtr.IsValid() && rightBottomLonFilterParamPtr->GetTextFilter() != ""){
		rightBottomLonFilterQuery = QString("(\"Document\"->>'Longitude')::double precision < %1").arg(rightBottomLonFilterParamPtr->GetTextFilter().toDouble());
	}

	if (!zoomLevelFilterQuery.isEmpty()){
		filterQuery += " AND ";
		filterQuery += zoomLevelFilterQuery;
	}

	if (!leftTopLatFilterQuery.isEmpty()){
		filterQuery += " AND ";
		filterQuery += leftTopLatFilterQuery;
	}

	if (!leftTopLonFilterQuery.isEmpty()){
		filterQuery += " AND ";
		filterQuery += leftTopLonFilterQuery;
	}

	if (!rightBottomLatFilterQuery.isEmpty()){
		filterQuery += " AND ";
		filterQuery += rightBottomLatFilterQuery;
	}

	if (!rightBottomLonFilterQuery.isEmpty()){
		filterQuery += " AND ";
		filterQuery += rightBottomLonFilterQuery;
	}

	return true;
}


} // namespace imtgeo


