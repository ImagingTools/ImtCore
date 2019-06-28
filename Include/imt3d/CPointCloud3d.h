#pragma once


// Qt includes
#include <QtCore/QLinkedList>

// ACF includes
#include <iimg/IRasterImage.h>
#include <istd/TOptDelPtr.h>

// ImtCore includes
#include <imt3d/imt3d.h>
#include <imt3d/IPointCloud3d.h>
#include <imt3d/IGridInfo.h>


namespace imt3d
{


/**
	Common implementation of a 3D-point cloud.
*/
class CPointCloud3d: virtual public IPointCloud3d, virtual public IGridInfo
{
public:
	CPointCloud3d();
	~CPointCloud3d();

	// reimplemented (IPointCloud3d)
	bool CreateCloud(PointFormat pointFormat, int pointsCount, const istd::CIndex2d* gridSizePtr = nullptr) override;
	bool CreateCloud(PointFormat pointFormat,
			int pointsCount,
			void* dataPtr,
			bool releaseFlag,
			const istd::CIndex2d* gridSizePtr = nullptr) override;
	PointFormat GetPointFormat() const override;
	int GetPointsCount() const override;
	const void* GetPointData(int pointIndex) const override;
	void* GetPointData(int pointIndex) override;

	// reimplemented (IGridInfo)
	istd::CIndex2d GetGridSize() const override;
	void SetGridSize(const istd::CIndex2d& gridSize) override;
	istd::CIndex2d GetGridPosition(int index) const override;
	int GetCloudPosition(const istd::CIndex2d& index) const override;

	// reimplemented (IObject3d)
	bool IsEmpty() const override;
	i3d::CVector3d GetCenter() const override;
	void MoveCenterTo(const i3d::CVector3d& position) override;
	CCuboid GetBoundingCuboid() const override;

	// reimplemented (iser::ISerializable)
	bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	bool CopyFrom(const istd::IChangeable& object, istd::IChangeable::CompatibilityMode mode) override;
	bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	// reimplemented (istd::IChangeable)
	void OnEndChanges(const ChangeSet& changes) override;

private:
	bool CreateCloudHelper(PointFormat pointFormat,
			int pointsCount,
			void* dataPtr,
			bool releaseFlag,
			const istd::CIndex2d* gridSizePtr);
	void EnsureCenterCalculated() const;
	template <typename PointType> void TEnsureCenterCalculated() const;
	void EnsureCuboidCalculated() const;
	template <typename PointType> void TEnsureCuboidCalculated() const;
	template <typename PointType> PointType* TGetPointData(int pointIndex, bool validOnly = false) const;
	template <typename PointType> void TMoveCenterTo(const i3d::CVector3d& position);
	template <typename PointType> void GetBoundingRanges(istd::CRange& xRange, istd::CRange& yRange, istd::CRange& zRange) const;
	template <typename PointType> bool IsPointValid(const PointType& pointData) const;
	int GetDataSize() const;
	void AllocateData();
	void FreeData();
	template <typename PointType> void AllocateData();
	template <typename PointType> void FreeData();

private:
	quint8* m_dataPtr;
	bool m_dataOwner;
	PointFormat m_pointFormat;
	int m_pointsCount;
	int m_componentsCount;

	istd::CIndex2d m_gridSize;

	mutable i3d::CVector3d m_cloudCenter;
	mutable CCuboid m_boundingCuboid;
	mutable bool m_isCloudCenterCalculationValid;
	mutable bool m_isCloudCuboidCalculationValid;
};


} // namespace imt3d


