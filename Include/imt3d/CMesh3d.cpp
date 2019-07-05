#include <imt3d/CMesh3d.h>


// ACF includes
#include <istd/CChangeNotifier.h>


namespace imt3d
{


// public methods

bool CMesh3d::SaveToStlFile(const QString& filePath) const
{
	switch (m_pointFormat){
		case IPointsBasedObject::PF_XYZW_NORMAL_32:
			return SaveToStlFile<IPointsBasedObject::PointXyzwNormal32>(filePath);
		default:
			// STL file format assumes the presence of normals
			return false;
	}
}


bool CMesh3d::LoadFromStlFile(const QString& filePath)
{
	istd::CChangeNotifier changeNotifier(this);

	ResetData();

	m_isCenterCalculationValid = false;
	m_isCuboidCalculationValid = false;

	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly)){
		return false;
	}

	// read header
	file.read(s_stlHeaderSize);

	quint32 trianglesCount = 0;
	file.read(reinterpret_cast<char*>(&trianglesCount), sizeof(trianglesCount));

	m_indices.resize(trianglesCount);

	int pointsCount = trianglesCount * 3;

	// allocate memory for points data
	IPointsBasedObject::PointXyzwNormal32* pointsDataPtr = new IPointsBasedObject::PointXyzwNormal32[pointsCount];

	for (quint32 i = 0; i < trianglesCount; ++i){
		// set indices
		m_indices[i].resize(3);
		m_indices[i][0] = i * 3 + 0;
		m_indices[i][1] = i * 3 + 1;
		m_indices[i][2] = i * 3 + 2;

		// read normal
		IPointsBasedObject::PointXyzwNormal32& point1 = pointsDataPtr[i * 3 + 0];
		IPointsBasedObject::PointXyzwNormal32& point2 = pointsDataPtr[i * 3 + 1];
		IPointsBasedObject::PointXyzwNormal32& point3 = pointsDataPtr[i * 3 + 2];

		if (!ReadPointData(file, &point1.data[4])){
			return false;
		}

		// read vertices
		if (!ReadPointData(file, &point1.data[0])){
			return false;
		}

		if (!ReadPointData(file, &point2.data[0])){
			return false;
		}

		if (!ReadPointData(file, &point3.data[0])){
			return false;
		}

		// point data structure assumes that normal is specified for each point,
		// so there is nothing to do but copy them
		memcpy(&point2.data[4], &point1.data[4], sizeof(float) * 3);
		memcpy(&point3.data[4], &point1.data[4], sizeof(float) * 3);

		// read (skip) attributes
		file.read(sizeof(quint16));
	}

	return Create(IPointsBasedObject::PF_XYZW_NORMAL_32, pointsCount, pointsDataPtr, true);
}


bool CMesh3d::CreateMesh(PointFormat pointFormat, int pointsCount, const Indices& indices)
{
	istd::CChangeNotifier changeNotifier(this);

	bool retVal = Create(pointFormat, pointsCount, nullptr, true);

	if (retVal){
		m_indices = indices;
	}

	return retVal;
}


bool CMesh3d::CreateMesh(PointFormat pointFormat, int pointsCount, void* pointsDataPtr, bool pointsDataReleaseFlag, const Indices& indices)
{
	istd::CChangeNotifier changeNotifier(this);

	bool retVal = Create(pointFormat, pointsCount, pointsDataPtr, pointsDataReleaseFlag);

	if (retVal){
		m_indices = indices;
	}

	return retVal;
}


const IMesh3d::Indices& CMesh3d::GetIndices() const
{
	return m_indices;
}


// reimplemented (iser::ISerializable)

bool CMesh3d::Serialize(iser::IArchive& /*archive*/)
{
	return false;
}


// reimplemented (istd::IChangeable)

bool CMesh3d::CopyFrom(const istd::IChangeable& object, istd::IChangeable::CompatibilityMode mode)
{
	bool retVal = CPointsBasedObject::CopyFrom(object, mode);

	if (retVal){
		const CMesh3d* objectPtr = dynamic_cast<const CMesh3d*>(&object);
		if (objectPtr != nullptr){
			istd::CChangeNotifier changeNotifier(this);

			m_indices = objectPtr->m_indices;
		}
	}

	return retVal;
}


bool CMesh3d::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	bool retVal = CPointsBasedObject::ResetData(mode);

	if (retVal){
		m_indices.clear();
	}

	return retVal;
}


template <typename PointType>
bool CMesh3d::SaveToStlFile(const QString& filePath) const
{
	if (IsEmpty()){
		return false;
	}

	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly)){
		return false;
	}

	// write header
	file.write(QByteArray(s_stlHeaderSize, 0));

	// write triangles count
	quint32 trianglesCount = static_cast<quint32>(m_indices.size());
	bool retVal = WriteTypedValue<quint16>(trianglesCount, file);

	// write triangles
	for (size_t i = 0; retVal && i < m_indices.size(); i++){
		const std::vector<uint32_t>& triangle = m_indices[i];

		retVal = (triangle.size() == 3);

		// write normal coordinates (assume that normal starts from the 4th point component)
		if (retVal){
			retVal = WritePointData<PointType>(triangle[0], 3, file);
		}

		// write points coordinates
		if (retVal){
			retVal = WritePointData<PointType>(triangle[0], 0, file);
		}
		if (retVal){
			retVal = WritePointData<PointType>(triangle[1], 0, file);
		}
		if (retVal){
			retVal = WritePointData<PointType>(triangle[2], 0, file);
		}

		// write attributes
		if (retVal){
			quint16 attributes = 0;
			retVal = WriteTypedValue<quint16>(attributes, file);
		}
	}

	return retVal;
}


template <typename DataType>
bool CMesh3d::WriteTypedValue(const DataType& data, QFile& file) const
{
	const char* bufPtr = reinterpret_cast<const char*>(&data);
	qint64 bufSize = sizeof(DataType);

	return file.write(bufPtr, bufSize) == bufSize;
}


template <typename PointType>
bool CMesh3d::WritePointData(int pointIndex, int pointComponentOffset, QFile& file) const
{
	const PointType* pointDataPtr = static_cast<const PointType*>(GetPointData(pointIndex));
	Q_ASSERT(pointDataPtr != nullptr);

	float buf[3]; // STL format requires all floating point data to be 32 bit so cast to float
	buf[0] = static_cast<float>(pointDataPtr->data[pointComponentOffset + 0]);
	buf[1] = static_cast<float>(pointDataPtr->data[pointComponentOffset + 1]);
	buf[2] = static_cast<float>(pointDataPtr->data[pointComponentOffset + 2]);

	const char* bufPtr = reinterpret_cast<const char*>(buf);
	qint64 bufSize = sizeof(buf);
	
	return file.write(bufPtr, bufSize) == bufSize;
}


bool CMesh3d::ReadPointData(QFile& file, float* pointDataPtr) const
{
	if (!pointDataPtr){
		return false;
	}

	char* bufPtr = reinterpret_cast<char*>(pointDataPtr);
	qint64 bufSize = sizeof(float) * 3;

	return (file.read(bufPtr, bufSize) == bufSize);
}


} // namespace imt3d
