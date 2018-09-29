#include <imt3d/CPointCloud3d.h>


// Qt includes
#include <QtCore/QLinkedList>

// Acf includes
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imt3d/IObject3d.h>
#include <i3d/CVector3d.h>


namespace imt3d
{


CPointCloud3d::CPointCloud3d()
	:m_isCloudCenterCalculationValid(false),
	m_isCloudCuboidCalculationValid(false)
{
}


void CPointCloud3d::AddPoint(const Point3d &point)
{
	istd::CChangeNotifier notifier(this);

	m_cloudPoints.push_back(point);
	m_isCloudCenterCalculationValid = false;
	m_isCloudCuboidCalculationValid = false;
}


// reimplemented (IPointCloud3d)

const CloudPoints& CPointCloud3d::GetPoints() const
{
	return m_cloudPoints;
}


// reimplemented (IObject3d)

bool CPointCloud3d::IsEmpty() const
{
	return m_cloudPoints.isEmpty();
}


i3d::CVector3d CPointCloud3d::GetCenter() const
{
	if (!IsEmpty() && !m_isCloudCenterCalculationValid){
		const_cast<CPointCloud3d*>(this)->CalculateCloudCenterPoint();
	}

	return m_cloudCenter;
}


void CPointCloud3d::MoveCenterTo(const i3d::CVector3d &position)
{
	i3d::CVector3d center = GetCenter();
	if (center != position){
		istd::CChangeNotifier notifier(this);

		i3d::CVector3d delta = position - center;
		for (CloudPoints::iterator pointIter = m_cloudPoints.begin(); pointIter != m_cloudPoints.end(); pointIter++){
			pointIter->SetX(pointIter->GetX() + delta.GetX());
			pointIter->SetY(pointIter->GetY() + delta.GetY());
			pointIter->SetZ(pointIter->GetZ() + delta.GetZ());
		}

		m_cloudCenter = position;
	}
}


CCuboid CPointCloud3d::GetBoundingCuboid() const
{
	if (!IsEmpty() && !m_isCloudCuboidCalculationValid){
		istd::CChangeNotifier notifier(const_cast<CPointCloud3d*>(this));

		m_boundingCuboid = CCuboid::FromCloudPoints(m_cloudPoints);
		m_isCloudCuboidCalculationValid = true;
	}

	return m_boundingCuboid;
}


// private methods

void CPointCloud3d::CalculateCloudCenterPoint()
{
	i3d::CVector3d tempVal;

	int pointsCount = m_cloudPoints.count();
	for (CloudPoints::const_iterator pointIter = m_cloudPoints.constBegin(); pointIter != m_cloudPoints.constEnd(); pointIter++){
		tempVal[0] = tempVal[0] + pointIter->GetX() / pointsCount;
		tempVal[1] = tempVal[1] + pointIter->GetY() / pointsCount;
		tempVal[2] = tempVal[2] + pointIter->GetZ() / pointsCount;
	}

	if (tempVal != m_cloudCenter){
		istd::CChangeNotifier notifier(this);

		m_cloudCenter = tempVal;
	}

	m_isCloudCenterCalculationValid = true;
}


} // namespace imt3d


