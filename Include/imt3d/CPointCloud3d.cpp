#include <imt3d/CPointCloud3d.h>


// ACF includes
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


// static methods

bool CPointCloud3d::IsPointValid(const i3d::CVector3d& position)
{
	return !qIsNaN(position.GetX()) && !qIsNaN(position.GetY()) && !qIsNaN(position.GetZ());
}


i3d::CVector3d CPointCloud3d::GetInvalidPoint()
{
	return i3d::CVector3d(qQNaN(), qQNaN(), qQNaN());
}


// public methods

void CPointCloud3d::SetGridSize(const istd::CIndex2d& gridSize)
{
	m_gridSize = gridSize;
}


// reimplemented (IPointCloud3d)

void CPointCloud3d::CreateCloud(const IPointCloud3d::CloudPoints &points)
{
	m_isCloudCenterCalculationValid = false;

	istd::CChangeNotifier changeNotifier(this);

	m_cloudPoints = points;
}


const IPointCloud3d::CloudPoints& CPointCloud3d::GetPoints() const
{
	return m_cloudPoints;
}


const istd::CIndex2d CPointCloud3d::GetGridSize() const
{
	return m_gridSize;
}


const istd::CIndex2d CPointCloud3d::GetGridPosition(int index) const
{
	int y = index / m_gridSize.GetY();
	int x = index - m_gridSize.GetY() * y;
	return istd::CIndex2d(x, y);
}


int CPointCloud3d::GetCloudPosition(const istd::CIndex2d & index) const
{
	return m_gridSize.GetY()*index.GetY() + index.GetX();
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
			if (IsPointValid(*pointIter)){
				pointIter->SetX(pointIter->GetX() + delta.GetX());
				pointIter->SetY(pointIter->GetY() + delta.GetY());
				pointIter->SetZ(pointIter->GetZ() + delta.GetZ());
			}
		}

		m_cloudCenter = position;
	}
}


const CCuboid& CPointCloud3d::GetBoundingCuboid() const
{
	if (!m_isCloudCuboidCalculationValid){
		m_isCloudCuboidCalculationValid = true;

		if (!m_cloudPoints.isEmpty()){
			double left = std::numeric_limits<double>::max();
			double right = std::numeric_limits<double>::min();
			double bottom = std::numeric_limits<double>::max();
			double top = std::numeric_limits<double>::min();
			double near = std::numeric_limits<double>::min();
			double far = std::numeric_limits<double>::max();

			for (IPointCloud3d::CloudPoints::const_iterator pointIter = m_cloudPoints.constBegin(); pointIter != m_cloudPoints.constEnd(); pointIter++){
				if (!imt3d::CPointCloud3d::IsPointValid(*pointIter)){
					continue;
				}

				if ((pointIter->GetX() > right)){
					right = pointIter->GetX();
				}

				if ((pointIter->GetX() < left)){
					left = pointIter->GetX();
				}
				if ((pointIter->GetY() > top)){
					top = pointIter->GetY();
				}

				if ((pointIter->GetY() < bottom)){
					bottom = pointIter->GetY();
				}
				if ((pointIter->GetZ() > near)){
					near = pointIter->GetZ();
				}

				if ((pointIter->GetZ() < far)){
					far = pointIter->GetZ();
				}
			}

			m_boundingCuboid = CCuboid(left, right, bottom, top, near, far);
		}
		else{
			m_boundingCuboid = CCuboid();
		}
	}

	return m_boundingCuboid;
}


// reimplemented (iser::ISerializable)

bool CPointCloud3d::Serialize(iser::IArchive &archive)
{
	static iser::CArchiveTag pointCloudTag("PointCloud", "Point cloud", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag pointTag("Point", "Point", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag gridSizeXTag("GridSizeX", "Size of points grid throw x", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag gridSizeYTag("GridSizeY", "Size of points grid throw y", iser::CArchiveTag::TT_LEAF);

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

	int sizeX = m_gridSize.GetX();
	int sizeY = m_gridSize.GetY();

	retVal = retVal && archive.BeginTag(gridSizeXTag);
	retVal = retVal && archive.Process(sizeX);
	retVal = retVal && archive.EndTag(gridSizeXTag);
	retVal = retVal && archive.BeginTag(gridSizeYTag);
	retVal = retVal && archive.Process(sizeY);
	retVal = retVal && archive.EndTag(gridSizeYTag);

	if (!archive.IsStoring()){
		m_gridSize.SetX(sizeX);
		m_gridSize.SetY(sizeY);
	}

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CPointCloud3d::CopyFrom(const istd::IChangeable &object, istd::IChangeable::CompatibilityMode /*mode*/)
{
	const CPointCloud3d* objectPtr = dynamic_cast<const CPointCloud3d*>(&object);
	if (objectPtr != NULL){
		istd::CChangeNotifier changeNotifier(this);

		m_cloudPoints = objectPtr->m_cloudPoints;
		m_gridSize = objectPtr->m_gridSize;

		return true;
	}

	return false;
}


bool CPointCloud3d::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_cloudPoints.clear();

	return true;
}


// private methods

void CPointCloud3d::EnsureCenterCalculated() const
{
	if (!IsEmpty() && !m_isCloudCenterCalculationValid){
		istd::CRange xRange(qInf(), -qInf());
		istd::CRange yRange(qInf(), -qInf());
		istd::CRange zRange(qInf(), -qInf());

		for (		CloudPoints::ConstIterator pointIter = m_cloudPoints.constBegin();
					pointIter != m_cloudPoints.constEnd();
					pointIter++){
			if (!IsPointValid(*pointIter)){
				continue;
			}

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


