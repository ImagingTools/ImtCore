#include <imt3d/CImage3dCalibration.h>


namespace imt3d
{


CImage3dCalibration::CImage3dCalibration(const iimg::CGeneralBitmap & xyzBitmap)
	:m_xyzBitmap(xyzBitmap)
{
}


// reimplemented (imt3d::IImage3dCalibration)

bool CImage3dCalibration::Transform(IPointsBasedObject::PointXyz32 & result, const i2d::CVector2d & pixelPosition) const
{
	if (m_xyzBitmap.GetPixelFormat() != iimg::IBitmap::PF_XYZ32){
		return false;
	}

	istd::CIndex2d imageSize = m_xyzBitmap.GetImageSize();

	if (pixelPosition.GetX() < 0 || pixelPosition.GetY() < 0 ||
		pixelPosition.GetX() >= imageSize.GetX() - 1 || pixelPosition.GetY() >= imageSize.GetY() - 1){

		return false;
	}

	const int sourceX = int(pixelPosition.GetX());
	const double alphaX = pixelPosition.GetX() - sourceX;
	const int sourceY = int(pixelPosition.GetY());
	const double alphaY = pixelPosition.GetY() - sourceY;

	const float* linePtr = (const float*)m_xyzBitmap.GetLinePtr(sourceY);
	const float* line1Ptr = (const float*)m_xyzBitmap.GetLinePtr(sourceY + 1);
	const int lineOffset = sourceX * 3;

	float pixel11ValueX = linePtr[lineOffset];
	float pixel11ValueY = linePtr[lineOffset + 1];
	float pixel11ValueZ = linePtr[lineOffset + 2];
	float pixel12ValueX = linePtr[lineOffset + 3];
	float pixel12ValueY = linePtr[lineOffset + 4];
	float pixel12ValueZ = linePtr[lineOffset + 5];
	float pixel21ValueX = line1Ptr[lineOffset];
	float pixel21ValueY = line1Ptr[lineOffset + 1];
	float pixel21ValueZ = line1Ptr[lineOffset + 2];
	float pixel22ValueX = line1Ptr[lineOffset + 3];
	float pixel22ValueY = line1Ptr[lineOffset + 4];
	float pixel22ValueZ = line1Ptr[lineOffset + 5];

    if (std::isnan(pixel11ValueX) || std::isnan(pixel11ValueY) || std::isnan(pixel11ValueZ) ||
        std::isnan(pixel12ValueX) || std::isnan(pixel12ValueY) || std::isnan(pixel12ValueZ) ||
        std::isnan(pixel21ValueX) || std::isnan(pixel21ValueY) || std::isnan(pixel21ValueZ) ||
        std::isnan(pixel22ValueX) || std::isnan(pixel22ValueY) || std::isnan(pixel22ValueZ)){
		return false;
	}

	pixel11ValueX *= (1 - alphaX);
	pixel12ValueX *= alphaX;
	pixel12ValueX += pixel11ValueX;
	pixel12ValueX *= (1 - alphaY);
	pixel21ValueX *= (1 - alphaX);
	pixel22ValueX *= alphaX;
	pixel22ValueX += pixel21ValueX;
	pixel22ValueX *= alphaY;
	pixel22ValueX += pixel12ValueX;

	pixel11ValueY *= (1 - alphaX);
	pixel12ValueY *= alphaX;
	pixel12ValueY += pixel11ValueY;
	pixel12ValueY *= (1 - alphaY);
	pixel21ValueY *= (1 - alphaX);
	pixel22ValueY *= alphaX;
	pixel22ValueY += pixel21ValueY;
	pixel22ValueY *= alphaY;
	pixel22ValueY += pixel12ValueY;

	pixel11ValueZ *= (1 - alphaX);
	pixel12ValueZ *= alphaX;
	pixel12ValueZ += pixel11ValueZ;
	pixel12ValueZ *= (1 - alphaY);
	pixel21ValueZ *= (1 - alphaX);
	pixel22ValueZ *= alphaX;
	pixel22ValueZ += pixel21ValueZ;
	pixel22ValueZ *= alphaY;
	pixel22ValueZ += pixel12ValueZ;

	result.data[0] = pixel22ValueX;
	result.data[1] = pixel22ValueY;
	result.data[2] = pixel22ValueZ;

	return true;
}

// reimplemented (istd::IChangeable)

bool CImage3dCalibration::CopyFrom(const istd::IChangeable & object, CompatibilityMode mode)
{
	const CImage3dCalibration* sourcePtr = dynamic_cast<const CImage3dCalibration*>(&object);
	if (sourcePtr != NULL){
		return m_xyzBitmap.CopyFrom(sourcePtr->m_xyzBitmap, mode);
	}

	return false;
}


bool CImage3dCalibration::ResetData(CompatibilityMode /*mode*/)
{
	m_xyzBitmap.ResetImage();

	return true;
}


bool CImage3dCalibration::IsEqual(const IChangeable & object) const
{
	const CImage3dCalibration* sourcePtr = dynamic_cast<const CImage3dCalibration*>(&object);
	if (sourcePtr != NULL){
		return m_xyzBitmap.IsEqual(sourcePtr->m_xyzBitmap);
	}

	return false;
}


// reimplemented (istd::ISerializable)

bool CImage3dCalibration::Serialize(iser::IArchive & archive)
{
	return m_xyzBitmap.Serialize(archive);
}


} // namespace imt3d


