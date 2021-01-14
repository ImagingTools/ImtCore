#include <imt3d/CPointsBasedObject.h>


// STL includes
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
	m_bufferSize(0),
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


void* CPointsBasedObject::GetData() const
{
	return m_dataPtr;
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
			TMoveCenterTo<PointXyz32>(position);
			break;
		case IPointsBasedObject::PF_XYZ_64:
			TMoveCenterTo<PointXyz64>(position);
			break;
		case IPointsBasedObject::PF_XYZW_32:
			TMoveCenterTo<PointXyzw32>(position);
			break;
		case IPointsBasedObject::PF_XYZ_ABC_32:
			TMoveCenterTo<PointXyzAbc32>(position);
			break;
		case IPointsBasedObject::PF_XYZW_NORMAL_CURVATURE_32:
			TMoveCenterTo<IPointsBasedObject::PointXyzwNormal32>(position);
			break;
		case IPointsBasedObject::PF_XYZW_NORMAL_RGBA_32:
			TMoveCenterTo<IPointsBasedObject::PointXyzwNormalRgba32>(position);
			break;
		case IPointsBasedObject::PF_XYZW_RGBA_32:
			TMoveCenterTo<IPointsBasedObject::PointXyzwRgba32>(position);
			break;
		default:
			break;
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
		CreateInternalBuffer();

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

		CreateInternalBuffer();

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


bool CPointsBasedObject::Create(PointFormat pointFormat, int pointsCount, void* dataPtr, bool copyData)
{
	FreeData();

	m_pointFormat = pointFormat;
	m_pointsCount = pointsCount;
	m_dataOwner = copyData;
	bool result = true;

	if (m_dataOwner){
		result = CreateInternalBuffer();

		if (dataPtr && result) {
			int dataSize = GetDataSize();
			memcpy(m_dataPtr, dataPtr, dataSize);
		}
	}
	else{
		if (dataPtr) {
			m_dataPtr = static_cast<quint8*>(dataPtr);
		}
	}

	m_bufferSize = GetDataSize();
	return result;
}


bool CPointsBasedObject::Append(int pointsCount, const void* dataPtr)
{
	if (pointsCount <= 0){
		return false;
	}

	if (!m_dataOwner){
		return false;
	}

	int oldBufferSize = GetBufferSize(m_pointFormat, m_pointsCount);
	int appendSize = GetBufferSize(m_pointFormat, pointsCount);
	int newBufferSize = oldBufferSize + appendSize;

	if (newBufferSize > m_bufferSize) {
		m_bufferSize = 2 * newBufferSize;
		m_dataPtr = static_cast<quint8*>(realloc(m_dataPtr, m_bufferSize));

		if (m_dataPtr == nullptr) {
			return false;
		}
	}

	memcpy(m_dataPtr + oldBufferSize, dataPtr, appendSize);
	m_pointsCount += pointsCount;

	return true;
}


int CPointsBasedObject::GetDataSize() const
{
	return GetBufferSize(m_pointFormat, m_pointsCount);
}


bool CPointsBasedObject::CreateInternalBuffer()
{
	switch (m_pointFormat){
		case IPointsBasedObject::PF_XYZ_32:
			return AllocateObjectBuffer<PointXyz32>();
		case IPointsBasedObject::PF_XYZ_64:
			return AllocateObjectBuffer<PointXyz64>();
		case IPointsBasedObject::PF_XYZW_32:
			return AllocateObjectBuffer<PointXyzw32>();
		case IPointsBasedObject::PF_XYZ_ABC_32:
			return AllocateObjectBuffer<PointXyzAbc32>();
		case IPointsBasedObject::PF_XYZW_NORMAL_CURVATURE_32:
			return AllocateObjectBuffer<PointXyzwNormal32>();
		case IPointsBasedObject::PF_XYZW_NORMAL_RGBA_32:
			return AllocateObjectBuffer<PointXyzwNormalRgba32>();
		case IPointsBasedObject::PF_XYZW_RGBA_32:
			return AllocateObjectBuffer<PointXyzwRgba32>();
		default:
			return false;
	}
}


template <typename PointType>
bool CPointsBasedObject::AllocateObjectBuffer()
{
	FreeData();

	int dataSize = GetDataSize();
	if (dataSize > 0){
		if (!AllocateInternal<PointType>(dataSize, m_dataPtr)) {
			return false;
		}

		m_dataOwner = true;
	}

	return true;
}


bool CPointsBasedObject::AllocateData(int size, quint8*& buffer)
{
	switch (m_pointFormat){
		case IPointsBasedObject::PF_XYZ_32:
			return AllocateInternal<PointXyz32>(size, buffer);
		case IPointsBasedObject::PF_XYZ_64:
			return AllocateInternal<PointXyz64>(size, buffer);
		case IPointsBasedObject::PF_XYZW_32:
			return AllocateInternal<PointXyzw32>(size, buffer);
		case IPointsBasedObject::PF_XYZ_ABC_32:
			return AllocateInternal<PointXyzAbc32>(size, buffer);
		case IPointsBasedObject::PF_XYZW_NORMAL_CURVATURE_32:
			return AllocateInternal<PointXyzwNormal32>(size, buffer);
		case IPointsBasedObject::PF_XYZW_NORMAL_RGBA_32:
			return AllocateInternal<PointXyzwNormalRgba32>(size, buffer);
		case IPointsBasedObject::PF_XYZW_RGBA_32:
			return AllocateInternal<PointXyzwRgba32>(size, buffer);
		default:
			return false;
	}
}


void CPointsBasedObject::FreeData()
{
	switch (m_pointFormat){
		case IPointsBasedObject::PF_XYZ_32:
			TFreeData<PointXyz32>();
			break;
		case IPointsBasedObject::PF_XYZ_64:
			TFreeData<PointXyz64>();
			break;
		case IPointsBasedObject::PF_XYZW_32:
			TFreeData<PointXyzw32>();
			break;
		case IPointsBasedObject::PF_XYZ_ABC_32:
			TFreeData<PointXyzAbc32>();
			break;
		case IPointsBasedObject::PF_XYZW_NORMAL_CURVATURE_32:
			TFreeData<PointXyzwNormal32>();
			break;
		case IPointsBasedObject::PF_XYZW_NORMAL_RGBA_32:
			TFreeData<PointXyzwNormalRgba32>();
			break;
		case IPointsBasedObject::PF_XYZW_RGBA_32:
			TFreeData<PointXyzwRgba32>();
			break;
		default:
			break;
	}
}


template <typename PointType>
void CPointsBasedObject::TFreeData()
{
	if (m_dataOwner && m_dataPtr != nullptr){
		free(m_dataPtr);
	}

	m_dataPtr = nullptr;
	m_bufferSize = 0;
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
			TEnsureCenterCalculated<PointXyz32>();
			break;
		case IPointsBasedObject::PF_XYZ_64:
			TEnsureCenterCalculated<PointXyz64>();
			break;
		case IPointsBasedObject::PF_XYZW_32:
			TEnsureCenterCalculated<PointXyzw32>();
			break;
		case IPointsBasedObject::PF_XYZ_ABC_32:
			TEnsureCenterCalculated<PointXyzAbc32>();
			break;
		case IPointsBasedObject::PF_XYZW_NORMAL_CURVATURE_32:
			TEnsureCenterCalculated<PointXyzwNormal32>();
			break;
		case IPointsBasedObject::PF_XYZW_NORMAL_RGBA_32:
			TEnsureCenterCalculated<PointXyzwNormalRgba32>();
			break;
		case IPointsBasedObject::PF_XYZW_RGBA_32:
			TEnsureCenterCalculated<PointXyzwRgba32>();
			break;
		default:
			break;
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
			TEnsureCuboidCalculated<PointXyz32>();
			break;
		case IPointsBasedObject::PF_XYZ_64:
			TEnsureCuboidCalculated<PointXyz64>();
			break;
		case IPointsBasedObject::PF_XYZW_32:
			TEnsureCuboidCalculated<PointXyzw32>();
			break;
		case IPointsBasedObject::PF_XYZ_ABC_32:
			TEnsureCuboidCalculated<PointXyzAbc32>();
			break;
		case IPointsBasedObject::PF_XYZW_NORMAL_CURVATURE_32:
			TEnsureCuboidCalculated<PointXyzwNormal32>();
			break;
		case IPointsBasedObject::PF_XYZW_NORMAL_RGBA_32:
			TEnsureCuboidCalculated<PointXyzwNormalRgba32>();
			break;
		case IPointsBasedObject::PF_XYZW_RGBA_32:
			TEnsureCuboidCalculated<PointXyzwRgba32>();
			break;
		default:
			break;
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


// protected static methods

template <typename PointType>
bool CPointsBasedObject::AllocateInternal(int size, quint8*& buffer)
{
	buffer = reinterpret_cast<quint8*>(malloc(size*sizeof(PointType)));

	return buffer != nullptr;
}


int CPointsBasedObject::GetBufferSize(PointFormat pointFormat, int pointsCount)
{
	switch (pointFormat){
		case IPointsBasedObject::PF_XYZ_32:
			return pointsCount * sizeof(PointXyz32);
		case IPointsBasedObject::PF_XYZ_64:
			return pointsCount * sizeof(PointXyz64);
		case IPointsBasedObject::PF_XYZW_32:
			return pointsCount * sizeof(PointXyzw32);
		case IPointsBasedObject::PF_XYZ_ABC_32:
			return pointsCount * sizeof(PointXyzAbc32);
		case IPointsBasedObject::PF_XYZW_NORMAL_CURVATURE_32:
			return pointsCount * sizeof(PointXyzwNormal32);
		case IPointsBasedObject::PF_XYZW_NORMAL_RGBA_32:
			return pointsCount * sizeof(PointXyzwNormalRgba32);
		case IPointsBasedObject::PF_XYZW_RGBA_32:
			return pointsCount * sizeof(PointXyzwRgba32);
		default:
			return 0;
	}
}


} // namespace imt3d


