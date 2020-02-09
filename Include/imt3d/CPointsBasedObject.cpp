#include <imt3d/CPointsBasedObject.h>
#include <string>
#include <type_traits>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imt3d
{


// public methods

CPointsBasedObject::CPointsBasedObject()
	:m_dataPtr(nullptr),
	m_dataOwner(false),
	m_pointFormat(PointFormat::PF_XYZ_32),
	m_pointsCount(0),
	m_isCenterCalculationValid(false),
	m_isCuboidCalculationValid(false)
{
}

CPointsBasedObject::~CPointsBasedObject()
{
	FreeData();
}


// reimplemented (imt3d::IPointsBasedObject)


IPointsBasedObject::PointFormat CPointsBasedObject::GetPointFormat() const
{
	return m_pointFormat;
}


int CPointsBasedObject::GetPointsCount() const
{
	return m_pointsCount;
}


const void* CPointsBasedObject::GetPointData(int pointIndex) const
{
	switch (m_pointFormat){
		case IPointsBasedObject::PF_XYZ_32:
			return TGetPointData<PointXyz32>(pointIndex);
		case IPointsBasedObject::PF_XYZ_64:
			return TGetPointData<PointXyz64>(pointIndex);
		case IPointsBasedObject::PF_XYZW_32:
			return TGetPointData<PointXyzw32>(pointIndex);
		case IPointsBasedObject::PF_XYZ_ABC_32:
			return TGetPointData<PointXyzAbc32>(pointIndex);
		case IPointsBasedObject::PF_XYZW_NORMAL_CURVATURE_32:
			return TGetPointData<IPointsBasedObject::PointXyzwNormal32>(pointIndex);
		case IPointsBasedObject::PF_XYZW_NORMAL_RGBA_32:
			return TGetPointData<IPointsBasedObject::PointXyzwNormalRgba32>(pointIndex);
		case IPointsBasedObject::PF_XYZW_RGBA_32:
			return TGetPointData<IPointsBasedObject::PointXyzwRgba32>(pointIndex);
		default:
			return nullptr;
	}
}


void* CPointsBasedObject::GetPointData(int pointIndex)
{
	const CPointsBasedObject* constThis = static_cast<const CPointsBasedObject*>(this);
	return const_cast<void*>(constThis->GetPointData(pointIndex));
}


// reimplemented (imt3d::IObject3d)

bool CPointsBasedObject::IsEmpty() const
{
	return m_pointsCount == 0;
}


i3d::CVector3d CPointsBasedObject::GetCenter() const
{
	EnsureCenterCalculated();

	return m_cloudCenter;
}


void CPointsBasedObject::MoveCenterTo(const i3d::CVector3d& position)
{
	switch (m_pointFormat){
		case IPointsBasedObject::PF_XYZ_32:
			return TMoveCenterTo<PointXyz32>(position);
		case IPointsBasedObject::PF_XYZ_64:
			return TMoveCenterTo<PointXyz64>(position);
		case IPointsBasedObject::PF_XYZW_32:
			return TMoveCenterTo<PointXyzw32>(position);
		case IPointsBasedObject::PF_XYZ_ABC_32:
			return TMoveCenterTo<PointXyzAbc32>(position);
		case IPointsBasedObject::PF_XYZW_NORMAL_CURVATURE_32:
			return TMoveCenterTo<IPointsBasedObject::PointXyzwNormal32>(position);
		case IPointsBasedObject::PF_XYZW_NORMAL_RGBA_32:
			return TMoveCenterTo<IPointsBasedObject::PointXyzwNormalRgba32>(position);
		case IPointsBasedObject::PF_XYZW_RGBA_32:
			return TMoveCenterTo<IPointsBasedObject::PointXyzwRgba32>(position);
	}
}


CCuboid CPointsBasedObject::GetBoundingCuboid() const
{
	EnsureCuboidCalculated();

	return m_boundingCuboid;
}


// reimplemented (iser::ISerializable)

bool CPointsBasedObject::Serialize(iser::IArchive& archive)
{
	static iser::CArchiveTag pointFormatTag("PointFormat", "Point format", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag pointsCountTag("PointsCount", "Points count", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag dataTag("Data", "Point data", iser::CArchiveTag::TT_GROUP);

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

	return retVal;
}


// reimplemented (istd::IChangeable)

int CPointsBasedObject::GetSupportedOperations() const
{
	return SO_CLONE | SO_COMPARE | SO_COPY | SO_RESET;
}


bool CPointsBasedObject::CopyFrom(const istd::IChangeable& object, istd::IChangeable::CompatibilityMode /*mode*/)
{
	const CPointsBasedObject* objectPtr = dynamic_cast<const CPointsBasedObject*>(&object);
	if (objectPtr != NULL){
		istd::CChangeNotifier changeNotifier(this);

		m_pointFormat = objectPtr->m_pointFormat;
		m_pointsCount = objectPtr->m_pointsCount;

		int dataSize = GetDataSize();

		AllocateData();

		if (m_dataPtr){
			memcpy(m_dataPtr, objectPtr->m_dataPtr, dataSize);

			return true;
		}
	}

	return false;
}


bool CPointsBasedObject::IsEqual(const IChangeable & object) const
{
	const CPointsBasedObject* sourcePtr = dynamic_cast<const CPointsBasedObject*>(&object);
	if (sourcePtr != nullptr){
		return ((GetDataSize() == sourcePtr->GetDataSize())
				&& (memcmp(m_dataPtr, sourcePtr->m_dataPtr, GetDataSize()) == 0)
				&& (m_dataOwner == sourcePtr->m_dataOwner)
				&& (m_pointFormat == sourcePtr->m_pointFormat)
				&& (m_pointsCount == sourcePtr->m_pointsCount)
				&& (m_cloudCenter == sourcePtr->m_cloudCenter)
				&& (m_boundingCuboid == sourcePtr->m_boundingCuboid)
				&& (m_isCenterCalculationValid == sourcePtr->m_isCenterCalculationValid)
				&& (m_isCuboidCalculationValid == sourcePtr->m_isCuboidCalculationValid));
	}

	return false;
}


istd::IChangeable* CPointsBasedObject::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CPointsBasedObject> clonePtr(new CPointsBasedObject());

	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CPointsBasedObject::ResetData(istd::IChangeable::CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	FreeData();

	m_dataPtr = nullptr;
	m_dataOwner = false;
	m_pointFormat = IPointsBasedObject::PF_XYZ_32;
	m_pointsCount = 0;

	m_cloudCenter.Reset();
	m_boundingCuboid = CCuboid();
	m_isCenterCalculationValid = false;
	m_isCuboidCalculationValid = false;

	return true;
}


// protected methods

template <typename PointType>
PointType* CPointsBasedObject::TGetPointData(int pointIndex, bool validOnly) const
{
	Q_ASSERT(pointIndex >= 0 && pointIndex < m_pointsCount);

	if (!m_dataPtr){
		return nullptr;
	}

	PointType* typedDataPtr = reinterpret_cast<PointType*>(m_dataPtr);
	typedDataPtr += pointIndex;

	if (validOnly && !TIsPointValid<PointType>(*typedDataPtr)){
		return nullptr;
	}

	return typedDataPtr;
}


bool CPointsBasedObject::Create(PointFormat pointFormat, int pointsCount, void* dataPtr, bool releaseFlag)
{
	if (pointsCount <= 0){
		return false;
	}

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

	return true;
}


int CPointsBasedObject::GetDataSize() const
{
	switch (m_pointFormat){
		case IPointsBasedObject::PF_XYZ_32:
			return m_pointsCount * sizeof(PointXyz32);
		case IPointsBasedObject::PF_XYZ_64:
			return m_pointsCount * sizeof(PointXyz64);
		case IPointsBasedObject::PF_XYZW_32:
			return m_pointsCount * sizeof(PointXyzw32);
		case IPointsBasedObject::PF_XYZ_ABC_32:
			return m_pointsCount * sizeof(PointXyzAbc32);
		case IPointsBasedObject::PF_XYZW_NORMAL_CURVATURE_32:
			return m_pointsCount * sizeof(PointXyzwNormal32);
		case IPointsBasedObject::PF_XYZW_NORMAL_RGBA_32:
			return m_pointsCount * sizeof(PointXyzwNormalRgba32);
		case IPointsBasedObject::PF_XYZW_RGBA_32:
			return m_pointsCount * sizeof(PointXyzwRgba32);
		default:
			return 0;
	}
}


void CPointsBasedObject::AllocateData()
{
	switch (m_pointFormat){
		case IPointsBasedObject::PF_XYZ_32:
			return TAllocateData<PointXyz32>();
		case IPointsBasedObject::PF_XYZ_64:
			return TAllocateData<PointXyz64>();
		case IPointsBasedObject::PF_XYZW_32:
			return TAllocateData<PointXyzw32>();
		case IPointsBasedObject::PF_XYZ_ABC_32:
			return TAllocateData<PointXyzAbc32>();
		case IPointsBasedObject::PF_XYZW_NORMAL_CURVATURE_32:
			return TAllocateData<PointXyzwNormal32>();
		case IPointsBasedObject::PF_XYZW_NORMAL_RGBA_32:
			return TAllocateData<PointXyzwNormalRgba32>();
		case IPointsBasedObject::PF_XYZW_RGBA_32:
			return TAllocateData<PointXyzwRgba32>();
	}
}


template <typename PointType>
void CPointsBasedObject::TAllocateData()
{
	FreeData();

	int dataSize = GetDataSize();

	if (dataSize > 0){
		m_dataPtr = reinterpret_cast<quint8*>(new PointType[dataSize]);
		m_dataOwner = true;
	}
}


void CPointsBasedObject::FreeData()
{
	switch (m_pointFormat){
		case IPointsBasedObject::PF_XYZ_32:
			return TFreeData<PointXyz32>();
		case IPointsBasedObject::PF_XYZ_64:
			return TFreeData<PointXyz64>();
		case IPointsBasedObject::PF_XYZW_32:
			return TFreeData<PointXyzw32>();
		case IPointsBasedObject::PF_XYZ_ABC_32:
			return TFreeData<PointXyzAbc32>();
		case IPointsBasedObject::PF_XYZW_NORMAL_CURVATURE_32:
			return TFreeData<PointXyzwNormal32>();
		case IPointsBasedObject::PF_XYZW_NORMAL_RGBA_32:
			return TFreeData<PointXyzwNormalRgba32>();
		case IPointsBasedObject::PF_XYZW_RGBA_32:
			return TFreeData<PointXyzwRgba32>();
	}
}


template <typename PointType>
void CPointsBasedObject::TFreeData()
{
	if (m_dataOwner){
		PointType* dataPtr = reinterpret_cast<PointType*>(m_dataPtr);
		delete[] dataPtr;
	}

	m_dataPtr = nullptr;
	m_dataOwner = false;
}


template <typename PointType>
bool CPointsBasedObject::TIsPointValid(const PointType& pointData) const
{
	return !qIsNaN(pointData.data[0]) && !qIsNaN(pointData.data[1]) && !qIsNaN(pointData.data[2]);
}


void CPointsBasedObject::EnsureCenterCalculated() const
{
	switch (m_pointFormat){
		case IPointsBasedObject::PF_XYZ_32:
			return TEnsureCenterCalculated<PointXyz32>();
		case IPointsBasedObject::PF_XYZ_64:
			return TEnsureCenterCalculated<PointXyz64>();
		case IPointsBasedObject::PF_XYZW_32:
			return TEnsureCenterCalculated<PointXyzw32>();
		case IPointsBasedObject::PF_XYZ_ABC_32:
			return TEnsureCenterCalculated<PointXyzAbc32>();
		case IPointsBasedObject::PF_XYZW_NORMAL_CURVATURE_32:
			return TEnsureCenterCalculated<PointXyzwNormal32>();
		case IPointsBasedObject::PF_XYZW_NORMAL_RGBA_32:
			return TEnsureCenterCalculated<PointXyzwNormalRgba32>();
		case IPointsBasedObject::PF_XYZW_RGBA_32:
			return TEnsureCenterCalculated<PointXyzwRgba32>();
	}
}


template <typename PointType>
void CPointsBasedObject::TEnsureCenterCalculated() const
{
	if (!IsEmpty() && !m_isCenterCalculationValid){
		istd::CRange xRange, yRange, zRange;
		GetBoundingRanges<PointType>(xRange, yRange, zRange);

		if (xRange.IsValidNonEmpty() && yRange.IsValidNonEmpty() && zRange.IsValidNonEmpty()){
			m_cloudCenter = i3d::CVector3d(
				xRange.GetValueFromAlpha(0.5),
				yRange.GetValueFromAlpha(0.5),
				zRange.GetValueFromAlpha(0.5));

			m_isCenterCalculationValid = true;
		}
	}
}


void CPointsBasedObject::EnsureCuboidCalculated() const
{
	switch (m_pointFormat){
		case IPointsBasedObject::PF_XYZ_32:
			return TEnsureCuboidCalculated<PointXyz32>();
		case IPointsBasedObject::PF_XYZ_64:
			return TEnsureCuboidCalculated<PointXyz64>();
		case IPointsBasedObject::PF_XYZW_32:
			return TEnsureCuboidCalculated<PointXyzw32>();
		case IPointsBasedObject::PF_XYZ_ABC_32:
			return TEnsureCuboidCalculated<PointXyzAbc32>();
		case IPointsBasedObject::PF_XYZW_NORMAL_CURVATURE_32:
			return TEnsureCuboidCalculated<PointXyzwNormal32>();
		case IPointsBasedObject::PF_XYZW_NORMAL_RGBA_32:
			return TEnsureCuboidCalculated<PointXyzwNormalRgba32>();
		case IPointsBasedObject::PF_XYZW_RGBA_32:
			return TEnsureCuboidCalculated<PointXyzwRgba32>();
	}
}


template <typename PointType>
void CPointsBasedObject::TEnsureCuboidCalculated() const
{
	if (!IsEmpty() && !m_isCuboidCalculationValid){
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

			m_isCuboidCalculationValid = true;
		}
	}
}


template<typename PointType>
void CPointsBasedObject::TMoveCenterTo(const i3d::CVector3d& position)
{
	i3d::CVector3d center = GetCenter();

	if (center != position){
		i3d::CVector3d delta = position - center;

		for (int i = 0; i < m_pointsCount; ++i){
			PointType* pointDataPtr = TGetPointData<PointType>(i, true);

			if (pointDataPtr){
				typedef typename std::remove_reference<decltype(*pointDataPtr->data)>::type PointCoordinateType;

				pointDataPtr->data[0] += static_cast<PointCoordinateType>(delta.GetX());
				pointDataPtr->data[1] += static_cast<PointCoordinateType>(delta.GetY());
				pointDataPtr->data[2] += static_cast<PointCoordinateType>(delta.GetZ());
			}
		}
	}

	m_cloudCenter = position;
}


template<typename PointType>
void CPointsBasedObject::GetBoundingRanges(istd::CRange& xRange, istd::CRange& yRange, istd::CRange& zRange) const
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


// reimplemented (istd::IChangeable)

void CPointsBasedObject::OnEndChanges(const ChangeSet& /*changes*/)
{
	m_isCuboidCalculationValid = false;
	m_isCenterCalculationValid = false;
}


} // namespace imt3d


