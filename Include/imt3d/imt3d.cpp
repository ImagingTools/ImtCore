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
	imt3d::IPointCloud3d::CloudPoints points;

	for (int y = 0; y < bitmap.GetImageSize().GetY(); ++y){
		const float* linePtr = (const float*)bitmap.GetLinePtr(y);
		for (int x = 0; x < bitmap.GetImageSize().GetX(); ++x){
			IPointCloud3d::Point3d point;
			point[0] = x;
			point[1] = y;
			point[2] = linePtr[x];

			points.push_back(point);
		}
	}

	istd::CIndex2d gridSize = bitmap.GetImageSize();

	pointCloud.CreateCloud(points, &gridSize);

	return true;
}


bool imt3d::ConvertPointCloudToDepthBitmap(const IPointCloud3d& pointCloud, imt3d::IDepthBitmap& bitmap)
{
	const imt3d::IGridInfo* gridInfoPtr = dynamic_cast<const imt3d::IGridInfo*>(&pointCloud);
	if (gridInfoPtr != nullptr){
		istd::CIndex2d gridSize = gridInfoPtr->GetGridSize();
		if (gridSize.GetProductVolume() > 0){
			istd::CRange depthRange(pointCloud.GetBoundingCuboid().GetNear(), pointCloud.GetBoundingCuboid().GetFar());
			depthRange.Validate();

			istd::CChangeNotifier bitmapChangeNotifier(&bitmap);

			if (bitmap.CreateDepthBitmap(depthRange, gridInfoPtr->GetGridSize())){
				const imt3d::IPointCloud3d::CloudPoints& points = pointCloud.GetPoints();
				for (int pointIndex = 0; pointIndex < int(points.size()); ++pointIndex){
					istd::CIndex2d imagePos = gridInfoPtr->GetGridPosition(pointIndex);

					float* imageLinePtr = (float*)bitmap.GetLinePtr(imagePos.GetY());

					imageLinePtr[imagePos.GetX()] = points[pointIndex][2];
				}

			return true;
			}
		}
	}

	return false;
}


} // namespace imt3d


