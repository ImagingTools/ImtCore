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

	// reimplemented (IPointCloud3d)
	bool CreateCloud(PointFormat pointFormat,
			int pointsCount,
			void* dataPtr,
			bool releaseFlag,
			const istd::CIndex2d* gridSizePtr = nullptr) override;
	PointFormat GetPointFormat() const override;
	int GetPointsCount() const override;
	const void* GetPointData(int pointIndex) const override;

	// reimplemented (IGridInfo)
	virtual istd::CIndex2d GetGridSize() const override;
	virtual void SetGridSize(const istd::CIndex2d& gridSize) override;
	virtual istd::CIndex2d GetGridPosition(int index) const override;
	virtual int GetCloudPosition(const istd::CIndex2d& index) const override;

	// reimplemented (IObject3d)
	virtual bool IsEmpty() const override;
	virtual i3d::CVector3d GetCenter() const override;
	virtual void MoveCenterTo(const i3d::CVector3d& position) override;
	virtual CCuboid GetBoundingCuboid() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const istd::IChangeable& object, istd::IChangeable::CompatibilityMode mode) override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	// reimplemented (istd::IChangeable)
	virtual void OnEndChanges(const ChangeSet& changes) override;

private:
	void EnsureCenterCalculated() const;
	template <typename DataType> void TEnsureCenterCalculated() const;
	void EnsureCuboidCalculated() const;
	template <typename DataType> void TEnsureCuboidCalculated() const;
	template <typename DataType> DataType* TGetPointData(int pointIndex, bool validOnly) const;
	template <typename DataType> void TMoveCenterTo(const i3d::CVector3d& position);
	template <typename DataType> void GetBoundingRanges(istd::CRange& xRange, istd::CRange& yRange, istd::CRange& zRange) const;
	template <typename DataType> bool IsPointValid(const DataType& pointData) const;
	int GetDataSize() const;
	void ReallocateData();

private:
	istd::TOptDelPtr<quint8, true> m_dataPtr;
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


