#include <imt3d/CPointCloud3d.h>


// ACF includes
#include <istd/TRange.h>
#include <istd/CChangeNotifier.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>

// ImtCore includes
#include <imt3d/IObject3d.h>
#include <i3d/CVector3d.h>


namespace imt3d
{


// public methods

CPointCloud3d::CPointCloud3d()
	:m_pointFormat(PointFormat::PF_XYZF),
	m_pointsCount(0),
	m_isCloudCenterCalculationValid(false),
	m_isCloudCuboidCalculationValid(false)
{
}


bool CPointCloud3d::CreateCloud(PointFormat pointFormat,
			int pointsCount,
			void* dataPtr,
			bool releaseFlag,
			const istd::CIndex2d* gridSizePtr)
{
	if (pointsCount <= 0 || !dataPtr){
		return false;
	}

	m_isCloudCenterCalculationValid = false;

	istd::CChangeNotifier changeNotifier(this);

	m_pointFormat = pointFormat;
	m_pointsCount = pointsCount;

	m_dataPtr.SetPtr(static_cast<quint8*>(dataPtr), releaseFlag);

	if (gridSizePtr){
		m_gridSize = *gridSizePtr;
	}

	return true;
}


IPointCloud3d::PointFormat CPointCloud3d::GetPointFormat() const
{
	return m_pointFormat;
}


int CPointCloud3d::GetPointsCount() const
{
	return m_pointsCount;
}


const void* CPointCloud3d::GetPointData(int pointIndex) const
{
	CALL_TEMPLATED_POINTCLOUD_METHOD(this, TGetPointData, nullptr, pointIndex, false);
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
	if (!m_gridSize.IsSizeEmpty() && m_gridSize.IsValid()){
		int y = index / m_gridSize.GetX();
		int x = index % m_gridSize.GetX();

		return istd::CIndex2d(x, y);
	}

	return istd::CIndex2d::GetInvalid();
}


int CPointCloud3d::GetCloudPosition(const istd::CIndex2d& index) const
{
	return m_gridSize.GetX() * index.GetY() + index.GetX();
}


// reimplemented (IObject3d)

bool CPointCloud3d::IsEmpty() const
{
	return m_pointsCount == 0;
}


i3d::CVector3d CPointCloud3d::GetCenter() const
{
	EnsureCenterCalculated();

	return m_cloudCenter;
}


void CPointCloud3d::MoveCenterTo(const i3d::CVector3d& position)
{
	CALL_TEMPLATED_POINTCLOUD_METHOD(this, TMoveCenterTo, , position);
}


CCuboid CPointCloud3d::GetBoundingCuboid() const
{
	EnsureCuboidCalculated();

	return m_boundingCuboid;
}


// reimplemented (iser::ISerializable)

bool CPointCloud3d::Serialize(iser::IArchive& archive)
{
	static iser::CArchiveTag pointFormatTag("PointFormat", "Point format", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag pointsCountTag("PointsCount", "Points count", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag componentsCountTag("ComponentsCount", "Components count", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag dataTag("Data", "Point data", iser::CArchiveTag::TT_GROUP);
	static iser::CArchiveTag gridSizeXTag("GridSizeX", "Size of points grid throw x", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag gridSizeYTag("GridSizeY", "Size of points grid throw y", iser::CArchiveTag::TT_LEAF);

	// point format
	bool retVal = archive.BeginTag(pointFormatTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeEnum<PointFormat, ToString, FromString>(archive, m_pointFormat);
	retVal = retVal && archive.EndTag(pointFormatTag);

	// points count
	retVal = retVal && archive.BeginTag(pointsCountTag);
	retVal = retVal && archive.Process(m_pointsCount);
	retVal = retVal && archive.EndTag(pointsCountTag);

	// components count
	retVal = retVal && archive.BeginTag(componentsCountTag);
	retVal = retVal && archive.Process(m_componentsCount);
	retVal = retVal && archive.EndTag(componentsCountTag);

	// data
	int dataSize = GetDataSize();

	if (archive.IsStoring()){
		retVal = retVal && archive.BeginTag(dataTag);
		retVal = retVal && archive.ProcessData(m_dataPtr.GetPtr(), dataSize);
		retVal = retVal && archive.EndTag(dataTag);
	}
	else{
		ReallocateData();

		retVal = retVal && archive.BeginTag(dataTag);
		retVal = retVal && archive.ProcessData(m_dataPtr.GetPtr(), dataSize);
		retVal = retVal && archive.EndTag(dataTag);
	}

	// grid size
	int gridSizeX = m_gridSize.GetX();
	int gridSizeY = m_gridSize.GetY();

	retVal = retVal && archive.BeginTag(gridSizeXTag);
	retVal = retVal && archive.Process(gridSizeX);
	retVal = retVal && archive.EndTag(gridSizeXTag);

	retVal = retVal && archive.BeginTag(gridSizeYTag);
	retVal = retVal && archive.Process(gridSizeY);
	retVal = retVal && archive.EndTag(gridSizeYTag);

	if (!archive.IsStoring()){
		m_gridSize.SetX(gridSizeX);
		m_gridSize.SetY(gridSizeY);
	}

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CPointCloud3d::CopyFrom(const istd::IChangeable& object, istd::IChangeable::CompatibilityMode /*mode*/)
{
	const CPointCloud3d* objectPtr = dynamic_cast<const CPointCloud3d*>(&object);
	if (objectPtr != NULL){
		istd::CChangeNotifier changeNotifier(this);

		m_pointFormat = objectPtr->m_pointFormat;
		m_pointsCount = objectPtr->m_pointsCount;
		m_gridSize = objectPtr->m_gridSize;

		int dataSize = GetDataSize();

		ReallocateData();

		std::memcpy(m_dataPtr.GetPtr(), objectPtr->m_dataPtr.GetPtr(), dataSize);

		return true;
	}

	return false;
}


bool CPointCloud3d::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_dataPtr.Reset();
	m_gridSize.Reset();

	m_pointFormat = IPointCloud3d::PF_XYZF;
	m_pointsCount = 0;
	m_componentsCount = 0;

	return true;
}


// protected methods

// reimplemented (istd::IChangeable)

void CPointCloud3d::OnEndChanges(const ChangeSet& /*changes*/)
{
	m_isCloudCuboidCalculationValid = false;

	m_isCloudCenterCalculationValid = false;
}


// private methods

void CPointCloud3d::EnsureCenterCalculated() const
{
	CALL_TEMPLATED_POINTCLOUD_METHOD(this, TEnsureCenterCalculated,,);
}


template <typename DataType>
void CPointCloud3d::TEnsureCenterCalculated() const
{
	if (!IsEmpty() && !m_isCloudCenterCalculationValid){
		istd::CRange xRange, yRange, zRange;
		GetBoundingRanges<DataType>(xRange, yRange, zRange);

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
	CALL_TEMPLATED_POINTCLOUD_METHOD(this, TEnsureCuboidCalculated,,);
}


template <typename DataType>
void CPointCloud3d::TEnsureCuboidCalculated() const
{
	if (!IsEmpty() && !m_isCloudCuboidCalculationValid){
		istd::CRange xRange, yRange, zRange;
		GetBoundingRanges<DataType>(xRange, yRange, zRange);

		if (xRange.IsValidNonEmpty() && yRange.IsValidNonEmpty() && zRange.IsValidNonEmpty()){
			double left = xRange.GetMinValue();
			double right = xRange.GetMaxValue();
			double bottom = yRange.GetMinValue();
			double top = yRange.GetMaxValue();
			double far = zRange.GetMinValue();
			double near = zRange.GetMaxValue();

			m_boundingCuboid = CCuboid(left, right, bottom, top, near, far);

			m_isCloudCuboidCalculationValid = true;
		}
	}
}


template <typename DataType>
DataType* CPointCloud3d::TGetPointData(int pointIndex, bool validOnly) const
{
	Q_ASSERT(pointIndex >= 0 && pointIndex < m_pointsCount);

	quint8* rawDataPtr = m_dataPtr.GetPtr() + pointIndex * sizeof(DataType);
	DataType* typedDataPtr = reinterpret_cast<DataType*>(rawDataPtr);

	if (validOnly && typedDataPtr && !IsPointValid<DataType>(*typedDataPtr)){
		return nullptr;
	}

	return typedDataPtr;
}


template<typename DataType>
void CPointCloud3d::TMoveCenterTo(const i3d::CVector3d& position)
{
	i3d::CVector3d center = GetCenter();

	if (center != position){
		i3d::CVector3d delta = position - center;

		for (int i = 0; i < m_pointsCount; ++i){
			DataType* pointData = TGetPointData<DataType>(i, true);

			if (pointData){
				typedef std::remove_reference<decltype(*pointData->xyz)>::type PointCoordinateType;

				pointData->xyz[0] += static_cast<PointCoordinateType>(delta.GetX());
				pointData->xyz[1] += static_cast<PointCoordinateType>(delta.GetY());
				pointData->xyz[2] += static_cast<PointCoordinateType>(delta.GetZ());
			}
		}
	}

	m_cloudCenter = position;
}


template<typename DataType>
void CPointCloud3d::GetBoundingRanges(istd::CRange& xRange, istd::CRange& yRange, istd::CRange& zRange) const
{
	xRange.SetMinValue(qInf());
	xRange.SetMaxValue(-qInf());
	yRange.SetMinValue(qInf());
	yRange.SetMaxValue(-qInf());
	zRange.SetMinValue(qInf());
	zRange.SetMaxValue(-qInf());

	for (int i = 0; i < m_pointsCount; ++i){
		const DataType* pointDataPtr = TGetPointData<DataType>(i, true);
		if (!pointDataPtr){
			continue;
		}

		double x = static_cast<double>(pointDataPtr->xyz[0]);
		double y = static_cast<double>(pointDataPtr->xyz[1]);
		double z = static_cast<double>(pointDataPtr->xyz[2]);

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
}


template <typename DataType>
bool CPointCloud3d::IsPointValid(const DataType& pointData) const
{
	return !qIsNaN(pointData.xyz[0]) && !qIsNaN(pointData.xyz[1]) && !qIsNaN(pointData.xyz[2]);
}


int CPointCloud3d::GetDataSize() const
{
	switch (m_pointFormat){
		case IPointCloud3d::PF_XYZF:
			return m_pointsCount * sizeof(PointStructXyzF);
		case IPointCloud3d::PF_XYZD:
			return m_pointsCount * sizeof(PointStructXyzD);
		case IPointCloud3d::PF_XYZF_1I:
			return m_pointsCount * sizeof(PointStructXyzF1I);
		default:
			return 0;
	}
}


void CPointCloud3d::ReallocateData()
{
	int dataSize = GetDataSize();

	if (dataSize > 0){
		m_dataPtr.SetPtr(new quint8[dataSize], true);
	}
	else{
		m_dataPtr.Reset();
	}
}


} // namespace imt3d


