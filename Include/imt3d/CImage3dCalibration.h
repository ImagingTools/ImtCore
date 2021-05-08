#pragma once


// STL includes
#include <vector>

// ACF includes
#include <istd/TRange.h>
#include <iimg/CGeneralBitmap.h>

// ImtCore includes
#include <imt3d/IImage3dCalibration.h>


namespace imt3d
{


/**
	Common implementation of a 3D-points data.
*/
class CImage3dCalibration: virtual public IImage3dCalibration, virtual public iser::ISerializable
{
public:
	CImage3dCalibration() {}
	CImage3dCalibration(const iimg::CGeneralBitmap& xyzBitmap);

	// reimplemented (imt3d::IImage3dCalibration)
	virtual bool Transform(IPointsBasedObject::PointXyz32& result, const i2d::CVector2d& pixelPosition) const override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS);
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS);
	virtual bool IsEqual(const IChangeable& object) const;

	// reimplemented (istd::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	iimg::CGeneralBitmap m_xyzBitmap;
};


} // namespace imt3d


