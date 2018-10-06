#include <imt3d/CPointCloud3d.h>


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

void CPointCloud3d::CreateCloud(const imt3d::CloudPoints &points)
{
	m_isCloudCenterCalculationValid = false;

	istd::CChangeNotifier changeNotifier(this);

	m_cloudPoints = points;
}


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
	EnsureCenterCalculated();

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


// reimplemented (istd::IChangeable)

bool CPointCloud3d::CopyFrom(const istd::IChangeable &object, istd::IChangeable::CompatibilityMode /*mode*/)
{
	const CPointCloud3d* objectPtr = dynamic_cast<const CPointCloud3d*>(&object);
	if (objectPtr != NULL){
		m_cloudPoints = objectPtr->m_cloudPoints;
	}

	return false;
}


// static methods

IPointCloud3d::PointCloudPtr CPointCloud3d::FromImage(iimg::IRasterImage &image, istd::CIndex2d& size, double step)
{
	IPointCloud3d::PointCloudPtr retVal(new imt3d::CPointCloud3d());
	if (retVal.IsValid()){
		imt3d::CPointCloud3d* pointCloudPtr = dynamic_cast<imt3d::CPointCloud3d*>(retVal.GetPtr());
		if (pointCloudPtr != NULL){
			istd::CIndex2d imageSize = image.GetImageSize();
			int xSize = size.GetX();
			int ySize = size.GetY();
			for (int i = 0; i < xSize; ++i){
				for (int j = 0; j < ySize; ++j){
					// Calculate index in image
					double xCoef = 1. * imageSize.GetX() / size.GetX();
					double xIndex = xCoef * i;
					double yCoef = 1. * imageSize.GetY() / size.GetY();
					double yIndex = yCoef * j;
					istd::CIndex2d positionIndex((int)xIndex, (int)yIndex);
					icmm::CVarColor varColor = image.GetColorAt(positionIndex);
					double value = 0.;
					for (int n = 0; n < varColor.GetElementsCount(); ++n){
						value += varColor.GetElement(n);
					}
					pointCloudPtr->AddPoint(i3d::CVector3d(step * j, value, step * i));
				}
			}
		}
	}

	return retVal;
}


// private methods

void CPointCloud3d::EnsureCenterCalculated() const
{
	if (!IsEmpty() && !m_isCloudCenterCalculationValid) {
		i3d::CVector3d centerValue = i3d::CVector3d(0, 0, 0);

		int pointsCount = m_cloudPoints.count();
		for (CloudPoints::const_iterator pointIter = m_cloudPoints.constBegin(); pointIter != m_cloudPoints.constEnd(); pointIter++) {
			centerValue[0] = centerValue[0] + pointIter->GetX() / pointsCount;
			centerValue[1] = centerValue[1] + pointIter->GetY() / pointsCount;
			centerValue[2] = centerValue[2] + pointIter->GetZ() / pointsCount;
		}

		m_cloudCenter = centerValue;

		m_isCloudCenterCalculationValid = true;
	}
}


} // namespace imt3d


