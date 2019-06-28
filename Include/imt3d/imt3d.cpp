#include <imt3d/imt3d.h>


// ACF includes
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imt3d/IGridInfo.h>
#include <imt3d/IPointCloud3d.h>
#include <imt3d/IDepthBitmap.h>


namespace imt3d
{


bool ConvertDepthImageToCloud(const imt3d::IDepthBitmap& bitmap, IPointCloud3d& pointCloud)
{
	istd::CIndex2d bitmapSize = bitmap.GetImageSize();

	// since points count is unknown we have to store points in temporaray dynamic array and then copy them to to the point cloud
	std::vector<IPointCloud3d::PointXyz32> points;
	points.reserve(bitmapSize.GetX() * bitmapSize.GetY());

	bool isOrganized = true;

	for (int y = 0; y < bitmapSize.GetY(); ++y){
		const float* linePtr = (const float*)bitmap.GetLinePtr(y);

		for (int x = 0; x < bitmapSize.GetX(); ++x){
			if (qIsNaN(linePtr[x])){
				isOrganized = false;
				continue;
			}

			IPointCloud3d::PointXyz32 point;
			point.data[0] = static_cast<float>(x);
			point.data[1] = static_cast<float>(y);
			point.data[2] = linePtr[x];

			points.push_back(point);
		}
	}

	if (points.empty()){
		return false;
	}

	int pointsCount = static_cast<int>(points.size());

	// copy points to the point cloud buffer
	IPointCloud3d::PointXyz32* pointsDataPtr = new IPointCloud3d::PointXyz32[pointsCount];
	memcpy(pointsDataPtr, points.data(), pointsCount * sizeof(IPointCloud3d::PointXyz32));

	istd::CIndex2d* gridSizePtr = isOrganized ? &bitmapSize : nullptr;

	return pointCloud.CreateCloud(IPointCloud3d::PF_XYZ_32, pointsCount, pointsDataPtr, true, gridSizePtr);
}


bool ConvertPointCloudToDepthBitmap(const IPointCloud3d& pointCloud, imt3d::IDepthBitmap& bitmap)
{
	switch (pointCloud.GetPointFormat()){
		case imt3d::IPointCloud3d::PF_XYZ_32:
			return ConvertPointCloudToDepthBitmap<imt3d::IPointCloud3d::PointXyz32>(pointCloud, bitmap);
		case imt3d::IPointCloud3d::PF_XYZ_64:
			return ConvertPointCloudToDepthBitmap<imt3d::IPointCloud3d::PointXyz64>(pointCloud, bitmap);
		case imt3d::IPointCloud3d::PF_XYZW_32:
			return ConvertPointCloudToDepthBitmap<imt3d::IPointCloud3d::PointXyzw32>(pointCloud, bitmap);
		case imt3d::IPointCloud3d::PF_XYZ_ABC_32:
			return ConvertPointCloudToDepthBitmap<imt3d::IPointCloud3d::PointXyzAbc32>(pointCloud, bitmap);
		default:
			return false;
	}
}


template <typename PointType>
bool ConvertPointCloudToDepthBitmap(const IPointCloud3d& pointCloud, imt3d::IDepthBitmap& bitmap)
{
	const imt3d::IGridInfo* gridInfoPtr = dynamic_cast<const imt3d::IGridInfo*>(&pointCloud);
	if (!gridInfoPtr){
		return false;
	}

	istd::CIndex2d gridSize = gridInfoPtr->GetGridSize();
	if (gridSize.GetProductVolume() <= 0){
		return false;
	}

	istd::CRange depthRange(pointCloud.GetBoundingCuboid().GetNear(), pointCloud.GetBoundingCuboid().GetFar());
	depthRange.Validate();

	istd::CChangeNotifier bitmapChangeNotifier(&bitmap);

	if (!bitmap.CreateDepthBitmap(depthRange, gridSize)){
		return false;
	}

	for (int pointIndex = 0; pointIndex < pointCloud.GetPointsCount(); ++pointIndex){
		const PointType* pointDataPtr = static_cast<const PointType*>(pointCloud.GetPointData(pointIndex));
		Q_ASSERT(pointDataPtr != nullptr);

		istd::CIndex2d imagePos = gridInfoPtr->GetGridPosition(pointIndex);

		float* imageLinePtr = (float*)bitmap.GetLinePtr(imagePos.GetY());
		imageLinePtr[imagePos.GetX()] = pointDataPtr->data[2];
	}

	return true;
}


} // namespace imt3d


