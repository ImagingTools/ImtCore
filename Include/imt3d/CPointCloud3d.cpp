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
	:m_dataPtr(nullptr),
	m_dataOwner(false),
	m_pointFormat(PointFormat::PF_XYZ_32),
	m_pointsCount(0),
	m_isCloudCenterCalculationValid(false),
	m_isCloudCuboidCalculationValid(false)
{
}

CPointCloud3d::~CPointCloud3d()
{
	FreeData();
}


// reimplemented (IPointCloud3d)

bool CPointCloud3d::CreateCloud(PointFormat pointFormat, int pointsCount, const istd::CIndex2d* gridSizePtr)
{
	return CreateCloudHelper(pointFormat, pointsCount, nullptr, true, gridSizePtr);
}


bool CPointCloud3d::CreateCloud(PointFormat pointFormat,
			int pointsCount,
			void* dataPtr,
			bool releaseFlag,
			const istd::CIndex2d* gridSizePtr)
{
	if (!dataPtr){
		return false;
	}

	return CreateCloudHelper(pointFormat, pointsCount, dataPtr, releaseFlag, gridSizePtr);
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
	switch (m_pointFormat){
		case imt3d::IPointCloud3d::PF_XYZ_32:
			return TGetPointData<PointXyz32>(pointIndex);
		case imt3d::IPointCloud3d::PF_XYZ_64:
			return TGetPointData<PointXyz64>(pointIndex);
		case imt3d::IPointCloud3d::PF_XYZW_32:
			return TGetPointData<PointXyzw32>(pointIndex);
		case imt3d::IPointCloud3d::PF_XYZ_ABC_32:
			return TGetPointData<PointXyzAbc32>(pointIndex);
		default:
			return nullptr;
	}
}


void* CPointCloud3d::GetPointData(int pointIndex)
{
	const CPointCloud3d* constThis = static_cast<const CPointCloud3d*>(this);
	return const_cast<void*>(constThis->GetPointData(pointIndex));
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
	switch (m_pointFormat){
		case imt3d::IPointCloud3d::PF_XYZ_32:
			return TMoveCenterTo<PointXyz32>(position);
		case imt3d::IPointCloud3d::PF_XYZ_64:
			return TMoveCenterTo<PointXyz64>(position);
		case imt3d::IPointCloud3d::PF_XYZW_32:
			return TMoveCenterTo<PointXyzw32>(position);
		case imt3d::IPointCloud3d::PF_XYZ_ABC_32:
			return TMoveCenterTo<PointXyzAbc32>(position);
	}
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

	// data
	int dataSize = GetDataSize();

	if (archive.IsStoring()){
		retVal = retVal && archive.BeginTag(dataTag);
		retVal = retVal && archive.ProcessData(m_dataPtr, dataSize);
		retVal = retVal && archive.EndTag(dataTag);
	}
	else{
		AllocateData();

		retVal = retVal && archive.BeginTag(dataTag);
		retVal = retVal && archive.ProcessData(m_dataPtr, dataSize);
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

		AllocateData();

		if (m_dataPtr){
			std::memcpy(m_dataPtr, objectPtr->m_dataPtr, dataSize);

			return true;
		}
	}

	return false;
}


bool CPointCloud3d::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	FreeData();
	m_gridSize.Reset();

	m_pointFormat = IPointCloud3d::PF_XYZ_32;
	m_pointsCount = 0;

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

bool CPointCloud3d::CreateCloudHelper(PointFormat pointFormat,
			int pointsCount,
			void* dataPtr,
			bool releaseFlag,
			const istd::CIndex2d* gridSizePtr)
{
	if (pointsCount <= 0){
		return false;
	}

	m_isCloudCenterCalculationValid = false;

	istd::CChangeNotifier changeNotifier(this);

	m_pointFormat = pointFormat;
	m_pointsCount = pointsCount;

	if (dataPtr){
		FreeData();
		m_dataPtr = static_cast<quint8*>(dataPtr);
	}
	else{
		AllocateData();
	}

	m_dataOwner = releaseFlag;

	if (gridSizePtr){
		m_gridSize = *gridSizePtr;
	}

	return true;
}


void CPointCloud3d::EnsureCenterCalculated() const
{
	switch (m_pointFormat){
		case imt3d::IPointCloud3d::PF_XYZ_32:
			return TEnsureCenterCalculated<PointXyz32>();
		case imt3d::IPointCloud3d::PF_XYZ_64:
			return TEnsureCenterCalculated<PointXyz64>();
		case imt3d::IPointCloud3d::PF_XYZW_32:
			return TEnsureCenterCalculated<PointXyzw32>();
		case imt3d::IPointCloud3d::PF_XYZ_ABC_32:
			return TEnsureCenterCalculated<PointXyzAbc32>();
	}
}


template <typename PointType>
void CPointCloud3d::TEnsureCenterCalculated() const
{
	if (!IsEmpty() && !m_isCloudCenterCalculationValid){
		istd::CRange xRange, yRange, zRange;
		GetBoundingRanges<PointType>(xRange, yRange, zRange);

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
	switch (m_pointFormat){
		case imt3d::IPointCloud3d::PF_XYZ_32:
			return TEnsureCuboidCalculated<PointXyz32>();
		case imt3d::IPointCloud3d::PF_XYZ_64:
			return TEnsureCuboidCalculated<PointXyz64>();
		case imt3d::IPointCloud3d::PF_XYZW_32:
			return TEnsureCuboidCalculated<PointXyzw32>();
		case imt3d::IPointCloud3d::PF_XYZ_ABC_32:
			return TEnsureCuboidCalculated<PointXyzAbc32>();
	}
}


template <typename PointType>
void CPointCloud3d::TEnsureCuboidCalculated() const
{
	if (!IsEmpty() && !m_isCloudCuboidCalculationValid){
		istd::CRange xRange, yRange, zRange;
		GetBoundingRanges<PointType>(xRange, yRange, zRange);

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


template <typename PointType>
PointType* CPointCloud3d::TGetPointData(int pointIndex, bool validOnly) const
{
	Q_ASSERT(pointIndex >= 0 && pointIndex < m_pointsCount);

	if (!m_dataPtr){
		return nullptr;
	}

	PointType* typedDataPtr = reinterpret_cast<PointType*>(m_dataPtr);
	typedDataPtr += pointIndex;

	if (validOnly && !IsPointValid<PointType>(*typedDataPtr)){
		return nullptr;
	}

	return typedDataPtr;
}


template<typename PointType>
void CPointCloud3d::TMoveCenterTo(const i3d::CVector3d& position)
{
	i3d::CVector3d center = GetCenter();

	if (center != position){
		i3d::CVector3d delta = position - center;

		for (int i = 0; i < m_pointsCount; ++i){
			PointType* pointDataPtr = TGetPointData<PointType>(i, true);

			if (pointDataPtr){
				typedef std::remove_reference<decltype(*pointDataPtr->data)>::type PointCoordinateType;

				pointDataPtr->data[0] += static_cast<PointCoordinateType>(delta.GetX());
				pointDataPtr->data[1] += static_cast<PointCoordinateType>(delta.GetY());
				pointDataPtr->data[2] += static_cast<PointCoordinateType>(delta.GetZ());
			}
		}
	}

	m_cloudCenter = position;
}


template<typename PointType>
void CPointCloud3d::GetBoundingRanges(istd::CRange& xRange, istd::CRange& yRange, istd::CRange& zRange) const
{
	xRange.SetMinValue(qInf());
	xRange.SetMaxValue(-qInf());
	yRange.SetMinValue(qInf());
	yRange.SetMaxValue(-qInf());
	zRange.SetMinValue(qInf());
	zRange.SetMaxValue(-qInf());

	for (int i = 0; i < m_pointsCount; ++i){
		const PointType* pointDataPtr = TGetPointData<PointType>(i, true);
		if (!pointDataPtr){
			continue;
		}

		double x = static_cast<double>(pointDataPtr->data[0]);
		double y = static_cast<double>(pointDataPtr->data[1]);
		double z = static_cast<double>(pointDataPtr->data[2]);

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


template <typename PointType>
bool CPointCloud3d::IsPointValid(const PointType& pointData) const
{
	return !qIsNaN(pointData.data[0]) && !qIsNaN(pointData.data[1]) && !qIsNaN(pointData.data[2]);
}


int CPointCloud3d::GetDataSize() const
{
	switch (m_pointFormat){
		case imt3d::IPointCloud3d::PF_XYZ_32:
			return m_pointsCount * sizeof(PointXyz32);
		case imt3d::IPointCloud3d::PF_XYZ_64:
			return m_pointsCount * sizeof(PointXyz64);
		case imt3d::IPointCloud3d::PF_XYZW_32:
			return m_pointsCount * sizeof(PointXyzw32);
		case imt3d::IPointCloud3d::PF_XYZ_ABC_32:
			return m_pointsCount * sizeof(PointXyzAbc32);
		default:
			return 0;
	}
}


void CPointCloud3d::AllocateData()
{
	switch (m_pointFormat){
		case imt3d::IPointCloud3d::PF_XYZ_32:
			return AllocateData<PointXyz32>();
		case imt3d::IPointCloud3d::PF_XYZ_64:
			return AllocateData<PointXyz64>();
		case imt3d::IPointCloud3d::PF_XYZW_32:
			return AllocateData<PointXyzw32>();
		case imt3d::IPointCloud3d::PF_XYZ_ABC_32:
			return AllocateData<PointXyzAbc32>();
	}
}


void CPointCloud3d::FreeData()
{
	switch (m_pointFormat){
		case imt3d::IPointCloud3d::PF_XYZ_32:
			return FreeData<PointXyz32>();
		case imt3d::IPointCloud3d::PF_XYZ_64:
			return FreeData<PointXyz64>();
		case imt3d::IPointCloud3d::PF_XYZW_32:
			return FreeData<PointXyzw32>();
		case imt3d::IPointCloud3d::PF_XYZ_ABC_32:
			return FreeData<PointXyzAbc32>();
	}
}

template <typename PointType>
void CPointCloud3d::AllocateData()
{
	FreeData();

	int dataSize = GetDataSize();

	if (dataSize > 0){
		m_dataPtr = reinterpret_cast<quint8*>(new PointType[dataSize]);
		m_dataOwner = true;
	}
}


template <typename PointType>
void CPointCloud3d::FreeData()
{
	if (m_dataOwner){
		PointType* dataPtr = reinterpret_cast<PointType*>(m_dataPtr);
		delete[] dataPtr;
	}

	m_dataPtr = nullptr;
	m_dataOwner = false;
}


} // namespace imt3d


