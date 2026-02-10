// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	virtual bool CreateCloud(PointFormat pointFormat) override;
	bool CreateCloud(PointFormat pointFormat,
				int pointsCount,
				const void* dataPtr,
				const istd::CIndex2d* gridSizePtr = nullptr) override;
	virtual bool InsertPoints(
				int pointsCount,
				const void* dataPtr) override;

	// reimplemented (imt3d::IGridInfo)
	virtual istd::CIndex2d GetGridSize() const override;
	virtual void SetGridSize(const istd::CIndex2d& gridSize) override;
	virtual istd::CIndex2d GetGridPosition(int index) const override;
	virtual int GetCloudPosition(const istd::CIndex2d& index) const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	istd::CIndex2d m_gridSize;
};


} // namespace imt3d


