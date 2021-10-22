/********************************************************************************
**
**	Copyright (C) 2017-2020 ImagingTools GmbH
**
**	This file is part of the ImagingTools SDK.
**
**	This file may be used under the terms of the GNU Lesser
**	General Public License version 2.1 as published by the Free Software
**	Foundation and appearing in the file LicenseLGPL.txt included in the
**	packaging of this file.  Please review the following information to
**	ensure the GNU Lesser General Public License version 2.1 requirements
**	will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**	If you are unsure which license is appropriate for your use, please
**	contact us at info@imagingtools.de.
**
**
********************************************************************************/


#pragma once


// STD includes

#ifdef __GNUC__
	#include <mm_malloc.h>
#endif
#ifdef __MINGW32__
	#include <mm_malloc.h>
#endif
#ifdef __MINGW64__
	#include <mm_malloc.h>
#endif


// ImtCore includes
#include <imt3d/IObject3d.h>


namespace imt3d
{


/**
	Common interface describing object of 3d points with specified format
*/
class IPointsBasedObject : virtual public imt3d::IObject3d
{
public:
	enum ChangeFlags
	{
		CF_CREATE = 20000,
		CF_APPEND
	};

	enum PointFormat
	{
		PF_XYZ_32 = 0,					// 3 float point coordinates
		PF_XYZ_64,						// 3 double point coordinates
		PF_XYZ_ABC_32,					// position data (XYZ) and Euler angles (ABC) in radians, both as 32-bit float number
		PF_XYZW_32,						// 4 float point coordinates (homogeneous point coordinates)
		PF_XYZW_NORMAL_CURVATURE_32,	// 4 float point coordinates + 4 float normal coordinates + 4 float curvature
		PF_XYZW_NORMAL_RGBA_32,			// 4 float point coordinates + 4 float normal coordinates + color information as 32-bit float number
		PF_XYZW_RGBA_32,					// 4 float point coordinates + color information as 32-bit float number
		PF_UNDEFINED
	};

	I_DECLARE_ENUM(PointFormat, PF_XYZ_32, PF_XYZ_64, PF_XYZ_ABC_32, PF_XYZW_32, PF_XYZW_NORMAL_CURVATURE_32, PF_XYZW_NORMAL_RGBA_32, PF_XYZW_RGBA_32);

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4324)
#endif
	template <typename CoordinateType, int AlignSize, int ... InitValues>
	struct alignas(AlignSize) Point
	{
		CoordinateType data[sizeof...(InitValues)] = { InitValues... };

		void* operator new(size_t size){ return _mm_malloc(size, AlignSize); }
		void *operator new[](size_t size){ return _mm_malloc(size, AlignSize); }
		void operator delete(void* ptr){ _mm_free(ptr); }
		void operator delete[](void* ptr){ _mm_free(ptr); }
	};
#ifdef _MSC_VER
#pragma warning(pop) 
#endif

	typedef Point<float, 16, 0, 0, 0> PointXyz32;
	typedef Point<double, 32, 0, 0, 0> PointXyz64;
	typedef Point<float, 16, 0, 0, 0, 0, 0, 0> PointXyzAbc32;
	typedef Point<float, 16, 0, 0, 0, 1> PointXyzw32;
	typedef Point<float, 16, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0> PointXyzwNormal32;
	typedef Point<float, 16, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0> PointXyzwNormalRgba32;
	typedef Point<float, 16, 0, 0, 0, 1, 0, 1, 0, 1> PointXyzwRgba32;

	/**
		Get point format.
	*/
	virtual PointFormat GetPointFormat() const = 0;

	/**
		Get number of points in the point cloud.
	*/
	virtual int GetPointsCount() const = 0;

	/**
		Get full access to the point data buffer.
	*/
	virtual void* GetPointData(size_t pointIndex) = 0;

	/**
		Get read-only access to the point data buffer.
	*/
	virtual const void* GetPointData(size_t pointIndex) const = 0;

	/**
		Return pointer to the internal data buffer.
	*/
	virtual void* GetData() = 0;

	/**
		Return const pointer to the internal data buffer.
	*/
	virtual const void* GetData() const = 0;

	/**
		Return bytes count of one point
	*/
	virtual int GetPointBytesSize() const = 0;
};


} // namespace imt3d


