#pragma once


// Qt includes
#include <QtCore/QLinkedList>

// ImtCore includes
#include <imt3d/imt3d.h>
#include <imt3d/IPointCloud3d.h>


namespace imt3d
{


/**
	Common cloud (list) of 3D-points implementation (IPointCloud3d interface).
*/
class CPointCloud3d: virtual public IPointCloud3d
{
public:
	CPointCloud3d();

	void AddPoint(const i3d::CVector3d& point);

	// reimplemented (IPointCloud3d)
	virtual const CloudPoints& GetPoints() const Q_DECL_OVERRIDE;

	// reimplemented (IObject3d)
	virtual bool IsEmpty() const Q_DECL_OVERRIDE;
	virtual i3d::CVector3d GetCenter() const Q_DECL_OVERRIDE;
	virtual void MoveCenterTo(const i3d::CVector3d& position) Q_DECL_OVERRIDE;
	virtual CCuboid GetBoundingCuboid() const Q_DECL_OVERRIDE;

private:
	void CalculateCloudCenterPoint();

private:
	CloudPoints m_cloudPoints;

	mutable i3d::CVector3d m_cloudCenter;
	mutable CCuboid m_boundingCuboid;
	mutable bool m_isCloudCenterCalculationValid;
	mutable bool m_isCloudCuboidCalculationValid;
};


} // namespace imt3d


