#include <imt3d/imt3d.h>


// ACF includes
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imt3d/IGridInfo.h>
#include <imt3d/IPointCloud3d.h>
#include <imt3d/IDepthBitmap.h>


namespace imt3d
{


bool ConvertDepthImageToCloud(const IDepthBitmap& bitmap, IPointCloud3d& pointCloud)
{
	istd::CIndex2d bitmapSize = bitmap.GetImageSize();

	// since points count is unknown we have to store points in temporaray dynamic array and then copy them to to the point cloud
	std::vector<IPointsBasedObject::PointXyz32> points;
	points.reserve(bitmapSize.GetX() * bitmapSize.GetY());

	bool isOrganized = true;

	for (int y = 0; y < bitmapSize.GetY(); ++y){
		const float* linePtr = (const float*)bitmap.GetLinePtr(y);

		for (int x = 0; x < bitmapSize.GetX(); ++x){
			if (qIsNaN(linePtr[x])){
				isOrganized = false;
				continue;
			}

			IPointsBasedObject::PointXyz32 point;
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
	IPointsBasedObject::PointXyz32* pointsDataPtr = new IPointsBasedObject::PointXyz32[pointsCount];
	memcpy(pointsDataPtr, points.data(), pointsCount * sizeof(IPointsBasedObject::PointXyz32));

	istd::CIndex2d* gridSizePtr = isOrganized ? &bitmapSize : nullptr;

	return pointCloud.CreateCloud(IPointsBasedObject::PF_XYZ_32, pointsCount, pointsDataPtr, true, gridSizePtr);
}


bool ConvertPointCloudToDepthBitmap(const IPointCloud3d& pointCloud, IDepthBitmap& bitmap)
{
	switch (pointCloud.GetPointFormat()){
		case IPointsBasedObject::PF_XYZ_32:
			return ConvertPointCloudToDepthBitmap<IPointsBasedObject::PointXyz32>(pointCloud, bitmap);
		case IPointsBasedObject::PF_XYZ_64:
			return ConvertPointCloudToDepthBitmap<IPointsBasedObject::PointXyz64>(pointCloud, bitmap);
		case IPointsBasedObject::PF_XYZW_32:
			return ConvertPointCloudToDepthBitmap<IPointsBasedObject::PointXyzw32>(pointCloud, bitmap);
		case IPointsBasedObject::PF_XYZ_ABC_32:
			return ConvertPointCloudToDepthBitmap<IPointsBasedObject::PointXyzAbc32>(pointCloud, bitmap);
		case IPointsBasedObject::PF_XYZW_NORMAL_32:
			return ConvertPointCloudToDepthBitmap<IPointsBasedObject::PointXyzwNormal32>(pointCloud, bitmap);
		case IPointsBasedObject::PF_XYZW_RGBA_32:
			return ConvertPointCloudToDepthBitmap<IPointsBasedObject::PointXyzwRgba32>(pointCloud, bitmap);
		default:
			return false;
	}
}


template <typename PointType>
bool ConvertPointCloudToDepthBitmap(const IPointCloud3d& pointCloud, IDepthBitmap& bitmap)
{
	const IGridInfo* gridInfoPtr = dynamic_cast<const IGridInfo*>(&pointCloud);
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


