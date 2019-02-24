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

// reimplemented (IPointCloud3d)

void CPointCloud3d::CreateCloud(const IPointCloud3d::CloudPoints &points, const istd::CIndex2d* gridSizePtr)
{
	m_isCloudCenterCalculationValid = false;

	istd::CChangeNotifier changeNotifier(this);

	m_cloudPoints = points;

	if (gridSizePtr != nullptr){
		m_gridSize= *gridSizePtr;
	}
}


const IPointCloud3d::CloudPoints& CPointCloud3d::GetPoints() const
{
	return m_cloudPoints;
}


// reimplemented (IGridInfo)

istd::CIndex2d CPointCloud3d::GetGridSize() const
{
	return m_gridSize;
}


void CPointCloud3d::SetGridSize(const istd::CIndex2d& gridSize)
{
	if (m_gridSize != gridSize){
		istd::CChangeNotifier changeNotifier(this);

		m_gridSize = gridSize;
	}
}


istd::CIndex2d CPointCloud3d::GetGridPosition(int index) const
{
	int y = index / m_gridSize.GetX();
	int x = index % m_gridSize.GetX();

	return istd::CIndex2d(x, y);
}


int CPointCloud3d::GetCloudPosition(const istd::CIndex2d & index) const
{
	return m_gridSize.GetX() * index.GetY() + index.GetX();
}


// reimplemented (IObject3d)

bool CPointCloud3d::IsEmpty() const
{
	return m_cloudPoints.empty();
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
		i3d::CVector3d delta = position - center;
		for (		CloudPoints::iterator pointIter = m_cloudPoints.begin();
					pointIter != m_cloudPoints.end();
					++pointIter){
			if (IsPointValid(*pointIter)){
				pointIter->SetX(pointIter->GetX() + delta.GetX());
				pointIter->SetY(pointIter->GetY() + delta.GetY());
				pointIter->SetZ(pointIter->GetZ() + delta.GetZ());
			}
		}

		m_cloudCenter = position;
	}
}


CCuboid CPointCloud3d::GetBoundingCuboid() const
{
	EnsureCuboidCalculated();

	return m_boundingCuboid;
}


// reimplemented (iser::ISerializable)

bool CPointCloud3d::Serialize(iser::IArchive &archive)
{
	static iser::CArchiveTag pointCountTag("PointCount", "Point count", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag dataTag("Data", "Point data", iser::CArchiveTag::TT_GROUP);
	static iser::CArchiveTag gridSizeXTag("GridSizeX", "Size of points grid throw x", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag gridSizeYTag("GridSizeY", "Size of points grid throw y", iser::CArchiveTag::TT_LEAF);

	int count = int(m_cloudPoints.size());

	bool retVal = true;

	retVal = retVal && archive.BeginTag(pointCountTag);
	retVal = retVal && archive.Process(count);
	retVal = retVal && archive.EndTag(pointCountTag);

	int dataSize = count * sizeof(i3d::CVector3d);

	if (archive.IsStoring()){
		retVal = retVal && archive.BeginTag(dataTag);
		retVal = retVal && archive.ProcessData(&m_cloudPoints[0], dataSize);
		retVal = retVal && archive.EndTag(dataTag);
	}
	else{
		m_cloudPoints.resize(count);

		retVal = retVal && archive.BeginTag(dataTag);
		retVal = retVal && archive.ProcessData(&m_cloudPoints[0], dataSize);
		retVal = retVal && archive.EndTag(dataTag);
	}

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

		for (		CloudPoints::const_iterator pointIter = m_cloudPoints.cbegin();
					pointIter != m_cloudPoints.cend();
					++pointIter){
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


void CPointCloud3d::EnsureCuboidCalculated() const
{
	if (!IsEmpty() && !m_isCloudCuboidCalculationValid){
		istd::CRange xRange(qInf(), -qInf());
		istd::CRange yRange(qInf(), -qInf());
		istd::CRange zRange(qInf(), -qInf());

		for (const i3d::CVector3d& point : m_cloudPoints){
			if (!IsPointValid(point)){
				continue;
			}

			double x = point.GetElement(0);
			double y = point.GetElement(1);
			double z = point.GetElement(2);

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
			const double left = xRange.GetMinValue();
			const double right = xRange.GetMaxValue();
			const double bottom = yRange.GetMinValue();
			const double top = yRange.GetMaxValue();
			const double far = zRange.GetMinValue();
			const double near = zRange.GetMaxValue();

			m_boundingCuboid = CCuboid(left, right, bottom, top, near, far);

			m_isCloudCuboidCalculationValid = true;
		}
	}
}


} // namespace imt3d


