#pragma once


// ImtCore includes
#include <imt3d/IObject3d.h>
#include <imt3d/imt3d.h>

namespace imt3d
{


/**
	Common interface for describing the cloud (list) of 3D-points.
*/
class IPointCloud3d: virtual public IObject3d
{
public:
	enum PointFormat
	{
		PF_XYZ_32 = 0,	// float 3D point coordinates
		PF_XYZ_64,		// double 3D point coordinates
		PF_XYZW_32,		// float homogeneous point coordinates
		PF_XYZ_ABC_32	// float 3D point coordinates + 3 additional components
	};

	I_DECLARE_ENUM(PointFormat, PF_XYZ_32, PF_XYZ_64, PF_XYZW_32, PF_XYZ_ABC_32);

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4324)
#endif
	template <typename CoordinateType, int AlignSize, int ... InitValues>
	struct alignas(AlignSize) Point
	{
		CoordinateType data[sizeof...(InitValues)] = { InitValues... };

		void* operator new(size_t size) { return _mm_malloc(size, AlignSize); }
		void *operator new[](size_t size) { return _mm_malloc(size, AlignSize); }
		void operator delete(void* ptr) { _mm_free(ptr); }
		void operator delete[](void* ptr) { _mm_free(ptr); }
	};
#ifdef _MSC_VER
#pragma warning(pop) 
#endif

	typedef Point<float, 16, 0, 0, 0> PointXyz32;
	typedef Point<double, 32, 0, 0, 0> PointXyz64;
	typedef Point<float, 16, 0, 0, 0, 1> PointXyzw32;
	typedef Point<float, 16, 0, 0, 0, 0, 0, 0> PointXyzAbc32;

	/**
		Create point cloud with specified point format.
	*/
	virtual bool CreateCloud(PointFormat pointFormat, int pointsCount, const istd::CIndex2d* gridSizePtr = nullptr) = 0;

	/**
		Create point cloud with specified point format using external data buffer.
	*/
	virtual bool CreateCloud(PointFormat pointFormat, int pointsCount, void* dataPtr, bool releaseFlag, const istd::CIndex2d* gridSizePtr = nullptr) = 0;

	/**
		Get point format.
	*/
	virtual PointFormat GetPointFormat() const = 0;

	/**
		Get points count.
	*/
	virtual int GetPointsCount() const = 0;

	/**
		Get points data.
	*/
	virtual void* GetPointData(int pointIndex) = 0;
	virtual const void* GetPointData(int pointIndex) const = 0;
};


} // namespace imt3d


