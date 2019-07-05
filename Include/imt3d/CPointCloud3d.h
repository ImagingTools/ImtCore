#pragma once


// ImtCore includes
#include <imt3d/IPointCloud3d.h>
#include <imt3d/IGridInfo.h>
#include <imt3d/CPointsBasedObject.h>


namespace imt3d
{


/**
	Common implementation of a 3D-point cloud.
*/
class CPointCloud3d: virtual public IPointCloud3d, virtual public IGridInfo, public CPointsBasedObject
{
public:
	// reimplemented (imt3d::IPointCloud3d)
	bool CreateCloud(PointFormat pointFormat, int pointsCount, const istd::CIndex2d* gridSizePtr = nullptr) override;
	bool CreateCloud(PointFormat pointFormat,
			int pointsCount,
			void* dataPtr,
			bool releaseFlag,
			const istd::CIndex2d* gridSizePtr = nullptr) override;

	// reimplemented (imt3d::IGridInfo)
	istd::CIndex2d GetGridSize() const override;
	void SetGridSize(const istd::CIndex2d& gridSize) override;
	istd::CIndex2d GetGridPosition(int index) const override;
	int GetCloudPosition(const istd::CIndex2d& index) const override;

	// reimplemented (iser::ISerializable)
	bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	bool CopyFrom(const istd::IChangeable& object, istd::IChangeable::CompatibilityMode mode) override;
	bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	istd::CIndex2d m_gridSize;
};


} // namespace imt3d


