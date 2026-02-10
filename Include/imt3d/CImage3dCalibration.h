// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	CImage3dCalibration(){}
	CImage3dCalibration(const iimg::CGeneralBitmap& xyzBitmap);

	// reimplemented (imt3d::IImage3dCalibration)
	virtual bool Transform(IPointsBasedObject::PointXyz32& result, const i2d::CVector2d& pixelPosition) const override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;

	// reimplemented (istd::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	const iimg::CGeneralBitmap& GetXZYBitmap() const { return m_xyzBitmap; }

private:
	iimg::CGeneralBitmap m_xyzBitmap;
};


} // namespace imt3d


