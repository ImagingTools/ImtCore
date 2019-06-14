#pragma once


// ImtCore includes
#include <imt3d/IObject3d.h>


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
		PF_XYZF,
		PF_XYZD,
		PF_XYZF_1I
	};

	I_DECLARE_ENUM(PointFormat, PF_XYZF, PF_XYZD, PF_XYZF_1I);

#pragma pack(push, 1)
	template <typename Type>
	struct PointStructXyz
	{
		Type xyz[3];
	};

	template <typename Type>
	struct PointStructXyz1I
	{
		Type xyz[3];
		int i;
	};
#pragma pack(pop)

	typedef PointStructXyz<float> PointStructXyzF;
	typedef PointStructXyz<double> PointStructXyzD;
	typedef PointStructXyz1I<float> PointStructXyzF1I;

	/**
		Create point cloud with specified size and format using external data buffer.
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
	virtual const void* GetPointData(int pointIndex) const = 0;
};


// macro to call templated method to process typed point cloud data
#define CALL_TEMPLATED_POINTCLOUD_METHOD(pointCloudPtr, templatedMethod, defaultResult, ...) \
switch (pointCloudPtr->GetPointFormat()){ \
	case imt3d::IPointCloud3d::PF_XYZF: \
		return templatedMethod<imt3d::IPointCloud3d::PointStructXyzF>(##__VA_ARGS__); \
	case imt3d::IPointCloud3d::PF_XYZD: \
		return templatedMethod<imt3d::IPointCloud3d::PointStructXyzD>(##__VA_ARGS__); \
	case imt3d::IPointCloud3d::PF_XYZF_1I: \
		return templatedMethod<imt3d::IPointCloud3d::PointStructXyzF1I>(##__VA_ARGS__); \
	default: \
		return defaultResult; \
}


} // namespace imt3d


