#include <imt3d/CPointCloud3d.h>


// Qt includes
#include <QtCore/QLinkedList>

// Acf includes
#include <istd/CChangeNotifier.h>
#include <iser/CArchiveTag.h>

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


void CPointCloud3d::AddPoint(const i3d::CVector3d &point)
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

bool CPointCloud3d::Serialize(iser::IArchive &archive)
{
	static iser::CArchiveTag pointCloudTag("PointCloud", "Point cloud", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag pointTag("Point", "Point", iser::CArchiveTag::TT_LEAF);

	bool retVal = true;
	int count = m_cloudPoints.count();
	archive.BeginMultiTag(pointCloudTag, pointTag, count);

	if (archive.IsStoring()){
		for (CloudPoints::iterator pointIter = m_cloudPoints.begin(); pointIter != m_cloudPoints.end(); pointIter++){
			retVal = retVal && archive.BeginTag(pointTag);
			retVal = retVal && pointIter->Serialize(archive);
			retVal = retVal && archive.EndTag(pointTag);
		}
	}
	else{
		i3d::CVector3d point;
		for (int i = 0; i < count; ++i){
			retVal = retVal && archive.BeginTag(pointTag);
			retVal = retVal && point.Serialize(archive);
			retVal = retVal && archive.EndTag(pointTag);
			if (retVal){
				m_cloudPoints.push_back(point);
			}
		}
	}

	archive.EndTag(pointCloudTag);

	return retVal;
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


