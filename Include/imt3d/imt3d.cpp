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

	QVector<float> pointCoordinates;
	pointCoordinates.reserve(bitmapSize.GetX() * bitmapSize.GetY());

	bool isOrganized = true;

	for (int y = 0; y < bitmapSize.GetY(); ++y){
		const float* linePtr = (const float*)bitmap.GetLinePtr(y);

		for (int x = 0; x < bitmapSize.GetX(); ++x){
			if (qIsNaN(linePtr[x])){
				isOrganized = false;

				continue;
			}

			pointCoordinates.push_back(static_cast<float>(x));
			pointCoordinates.push_back(static_cast<float>(y));
			pointCoordinates.push_back(static_cast<float>(linePtr[x]));
		}
	}

	int pointsCount = pointCoordinates.size() / 3;

	float* pointsDataPtr = new float[pointCoordinates.size()];
	memcpy(pointsDataPtr, pointCoordinates.constData(), pointCoordinates.size() * sizeof(float));

	istd::CIndex2d* gridSizePtr = isOrganized ? &bitmapSize : nullptr;

	return pointCloud.CreateCloud(IPointCloud3d::PF_XYZF, pointsCount, pointsDataPtr, true, gridSizePtr);
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

			if (bitmap.CreateDepthBitmap(depthRange, gridSize)){
				for (int pointIndex = 0; pointIndex < pointCloud.GetPointsCount(); ++pointIndex){
					const float* pointDataPtr = static_cast<const float*>(pointCloud.GetPointData(pointIndex));
					istd::CIndex2d imagePos = gridInfoPtr->GetGridPosition(pointIndex);

					float* imageLinePtr = (float*)bitmap.GetLinePtr(imagePos.GetY());
					imageLinePtr[imagePos.GetX()] = pointDataPtr[2];
				}

				return true;
			}
		}
	}

	return false;
}


} // namespace imt3d


