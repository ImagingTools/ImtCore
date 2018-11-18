#pragma once


// Qt includes
#include <QtCore/QLinkedList>

// Acf includes
#include <iimg/IRasterImage.h>

// ImtCore includes
#include <imt3d/imt3d.h>
#include <imt3d/IPointCloud3d.h>
#include <imt3d/IGridInfo.h>


namespace imt3d
{


/**
	Common cloud (list) of 3D-points implementation (IPointCloud3d interface).
*/
class CPointCloud3d: virtual public IPointCloud3d, virtual public IGridInfo
{
public:
	CPointCloud3d();

	void SetGridSize(const istd::CIndex2d& gridSize);

	void AddPoint(const i3d::CVector3d& point);

	// reimplemented (IPointCloud3d)
	virtual void CreateCloud(const imt3d::CloudPoints &points) override;
	virtual const CloudPoints& GetPoints() const override;

	virtual const istd::CIndex2d GetGridSize() const;
	virtual const istd::CIndex2d GetGridPosition(int index) const;
	virtual int GetCloudPosition(const istd::CIndex2d& index) const;

	// reimplemented (IObject3d)
	virtual bool IsEmpty() const override;
	virtual i3d::CVector3d GetCenter() const override;
	virtual void MoveCenterTo(const i3d::CVector3d& position) override;
	virtual const CCuboid& GetBoundingCuboid() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const istd::IChangeable& object, istd::IChangeable::CompatibilityMode mode) override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

	// static methods
	/*
		image size will be adjusted to required size
		\ todo think to make height (color value in index) scaleable
	*/
	static IPointCloud3d::PointCloudPtr FromImage(iimg::IRasterImage& image, istd::CIndex2d& size, double step = 0.05);

	static bool IsPointValid(const i3d::CVector3d& position);

	static i3d::CVector3d GetInvalidPoint();
private:
	void EnsureCenterCalculated() const;

private:
	CloudPoints m_cloudPoints;
	istd::CIndex2d m_gridSize;

	mutable i3d::CVector3d m_cloudCenter;
	mutable CCuboid m_boundingCuboid;
	mutable bool m_isCloudCenterCalculationValid;
	mutable bool m_isCloudCuboidCalculationValid;
};


} // namespace imt3d


