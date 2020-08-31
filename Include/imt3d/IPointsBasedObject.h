#pragma once


#ifdef COMPILER_NAME
	#if COMPILER_NAME == GCC
	    #include <mm_malloc.h>
	#endif
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
	enum PointFormat
	{
		PF_XYZ_32 = 0,					// 3 float point coordinates
		PF_XYZ_64,						// 3 double point coordinates
		PF_XYZ_ABC_32,					// position data (XYZ) and Euler angles (ABC) in radians, both as 32-bit float number
		PF_XYZW_32,						// 4 float point coordinates (homogeneous point coordinates)
		PF_XYZW_NORMAL_CURVATURE_32,	// 4 float point coordinates + 4 float normal coordinates + 4 float curvature
		PF_XYZW_NORMAL_RGBA_32,			// 4 float point coordinates + 4 float normal coordinates + color information as 32-bit float number
		PF_XYZW_RGBA_32					// 4 float point coordinates + color information as 32-bit float number
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
	virtual void* GetPointData(int pointIndex) = 0;

	/**
		Get read-only access to the point data buffer.
	*/
	virtual const void* GetPointData(int pointIndex) const = 0;

	/**
		Return pointer to the internal data buffer.
	*/
	virtual void* GetData() const = 0;
};


} // namespace imt3d


