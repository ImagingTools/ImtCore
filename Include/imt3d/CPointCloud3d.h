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
class CPointCloud3d:
			public CPointsBasedObject,
			virtual public IPointCloud3d,
			virtual public IGridInfo
{
public:
	typedef CPointsBasedObject BaseClass;

	// reimplemented (imt3d::IPointCloud3d)
	bool CreateCloud(PointFormat pointFormat) override;
	bool CreateCloud(PointFormat pointFormat,
			int pointsCount,
			const void* dataPtr,
			const istd::CIndex2d* gridSizePtr = nullptr) override;
	virtual bool InsertPoints(
				int pointsCount,
				const void* dataPtr) override;

	// reimplemented (imt3d::IGridInfo)
	istd::CIndex2d GetGridSize() const override;
	void SetGridSize(const istd::CIndex2d& gridSize) override;
	istd::CIndex2d GetGridPosition(int index) const override;
	int GetCloudPosition(const istd::CIndex2d& index) const override;

	// reimplemented (iser::ISerializable)
	bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	istd::CIndex2d m_gridSize;
};


} // namespace imt3d


