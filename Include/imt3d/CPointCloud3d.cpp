#include <imt3d/CPointCloud3d.h>


// Acf includes
#include <istd/TRange.h>
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
		for (		CloudPoints::iterator pointIter = m_cloudPoints.begin();
					pointIter != m_cloudPoints.end();
					pointIter++){
			pointIter->SetX(pointIter->GetX() + delta.GetX());
			pointIter->SetY(pointIter->GetY() + delta.GetY());
			pointIter->SetZ(pointIter->GetZ() + delta.GetZ());
		}

		m_cloudCenter = position;
	}
}


const CCuboid& CPointCloud3d::GetBoundingCuboid() const
{
	if (!IsEmpty() && !m_isCloudCuboidCalculationValid){
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
	if (!IsEmpty() && !m_isCloudCenterCalculationValid){
		istd::CRange xRange(qInf(), -qInf());
		istd::CRange yRange(qInf(), -qInf());
		istd::CRange zRange(qInf(), -qInf());

		for (CloudPoints::const_iterator pointIter = m_cloudPoints.constBegin(); pointIter != m_cloudPoints.constEnd(); pointIter++){
			double x = pointIter->GetX();
			double y = pointIter->GetY();
			double z = pointIter->GetZ();

			if (x < xRange.GetMinValue()){
				xRange.SetMinValue(x);
			}

			if (y < yRange.GetMinValue()){
				yRange.SetMinValue(y);
			}

			if (z < zRange.GetMinValue()){
				zRange.SetMinValue(z);
			}

			if (x > xRange.GetMaxValue()){
				xRange.SetMaxValue(x);
			}

			if (y > yRange.GetMaxValue()){
				yRange.SetMaxValue(y);
			}

			if (z > zRange.GetMaxValue()){
				zRange.SetMaxValue(z);
			}
		}

		if (xRange.IsValidNonEmpty() && yRange.IsValidNonEmpty() && zRange.IsValidNonEmpty()){
			m_cloudCenter = i3d::CVector3d(
						xRange.GetValueFromAlpha(0.5),
						yRange.GetValueFromAlpha(0.5),
						zRange.GetValueFromAlpha(0.5));

			m_isCloudCenterCalculationValid = true;
		}
	}
}


} // namespace imt3d


