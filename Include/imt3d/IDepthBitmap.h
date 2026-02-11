// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/TRange.h>
#include <iimg/IBitmap.h>
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imt3d/imt3d.h>
#include <imt3d/IImage3dCalibration.h>


namespace imt3d
{


/**
	Interface for a depth (range) image.
*/
class IDepthBitmap: virtual public iimg::IBitmap
{
public:
	enum
	{
		MIT_MIN_DEPTH = idoc::IDocumentMetaInfo::MIT_USER,
		MIT_MAX_DEPTH,

		/**
			Matrix used for transofrmation of depth values into the world coordinates given as QMatrix4x4
		*/
		MIT_REPROJECTION_MATRIX,

		MIT_LAST = MIT_REPROJECTION_MATRIX
	};

	/**
		Get the depth range of the bitmap space. The depth range defines the possible range of the pixel values in the bitmap.
	*/
	virtual istd::CRange GetDepthRange() const = 0;

	/**
		Create bitmap with specified size and format.
		\param	size			bitmap size.
		\param	pixelBitsCount	bits per pixel. Default value \c 0 means that the number of bits per pixel is defined by format specification.
		\param	componentsCount	components per pixel. Default value \c 0 means that the number of bits per pixel is defined by format specification.
	*/
	virtual bool CreateDepthBitmap(
				const istd::CRange& depthRange,
				const istd::CIndex2d& size) = 0;

	virtual const IImage3dCalibration* GetCalibration3d() const = 0;
	virtual const iimg::IBitmap* GetReferenceBitmap() const = 0;

	struct RobotPosition
	{
		int64_t timestampMicrosecond;
		float x;
		float y;
		float z;
	};

	typedef std::vector<RobotPosition> RobotTrajectory;

	virtual RobotTrajectory GetRobotTrajectory() const = 0;

	struct BaseCorrection {
		imath::TVector<4, float> translation;
		imath::TVector<3, float> rotationDeg;
		BaseCorrection() {
			translation.Reset();
			rotationDeg.Reset();
		}
	};

	virtual BaseCorrection GetBaseCorrection() const = 0;
};


} // namespace imt3d


