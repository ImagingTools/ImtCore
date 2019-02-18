#include <imt3d/CMesh3d.h>


// STL includes
#include <array>

// ACF includes
#include <istd/CChangeNotifier.h>


// vertex element access
static const int E_X = 0;
static const int E_Y = 1;
static const int E_Z = 2;


namespace imt3d
{


// public methods

CMesh3d::CMesh3d():
	m_isMeshCenterCalculationValid(false),
	m_isMeshCuboidCalculationValid(false)
{
}


bool CMesh3d::SaveToStlFile(const QString& filePath) const
{
	if (m_vertices.isEmpty()){
		return false;
	}

	std::FILE* file = std::fopen(filePath.toStdString().c_str(), "wb");
	
	if (file == NULL){
		return false;
	}

	std::array<quint8, 80> stlHeader = { 0 };
	std::fwrite(stlHeader.data(), sizeof(stlHeader[0]), stlHeader.size(), file);

	quint32 trianglesCount = static_cast<quint32>(m_triangles.size());
	std::fwrite(&trianglesCount, sizeof(trianglesCount), 1, file);

	for (uint index = 0; index < trianglesCount; index++){
		const Triangle& oneTriangle = m_triangles[index];

		Normal normal;
		normal.Clear();
		std::fwrite(&normal[0], sizeof(float), 3, file);

		Vertex firstVert = m_vertices[oneTriangle.GetElement(0)];
		Vertex secondVert = m_vertices[oneTriangle.GetElement(1)];
		Vertex thirdVert = m_vertices[oneTriangle.GetElement(2)];

		std::fwrite(&firstVert[0], sizeof(float), 3, file);
		std::fwrite(&secondVert[0], sizeof(float), 3, file);
		std::fwrite(&thirdVert[0], sizeof(float), 3, file);

		quint16 attributes = 0;
		std::fwrite(&attributes, sizeof(attributes), 1, file);
	}

	std::fclose(file);

	return true;
}


bool CMesh3d::LoadFromStlFile(const QString& filePath)
{
	istd::CChangeNotifier changeNotifier(this);

	m_vertices.clear();
	m_triangles.clear();
	m_normals.clear();

	m_isMeshCenterCalculationValid = false;
	m_isMeshCuboidCalculationValid = false;

	std::FILE* file = std::fopen(filePath.toStdString().c_str(), "rb");
	if (file == NULL){
		return false;
	}

	std::array<quint8, 80> stlHeader = { {0} };
	std::fread(stlHeader.data(), sizeof(stlHeader[0]), stlHeader.size(), file);

	quint32 numTris = 0;
	std::fread(&numTris, sizeof(numTris), 1, file);

	m_triangles.reserve(numTris);
	m_normals.reserve(numTris);
	m_vertices.reserve(3 * numTris);

	for (quint32 i = 0; i < numTris; ++i){
		// store indices of the m_vertices
		Triangle oneTriangle;
		oneTriangle.SetElement(0, i * 3);
		oneTriangle.SetElement(1, i * 3 + 1);
		oneTriangle.SetElement(2, i * 3 + 2);

		m_triangles.push_back(oneTriangle);

		FloatVector3d normal;
		normal.Clear();

		std::fread(&normal[0], sizeof(float), 3, file);

		FloatVector3d firstVert;
		FloatVector3d secondVert;
		FloatVector3d thirdVert;

		std::fread(&firstVert[0], sizeof(float), 3, file);
		std::fread(&secondVert[0], sizeof(float), 3, file);
		std::fread(&thirdVert[0], sizeof(float), 3, file);

		quint16 attributes = 0;
		std::fread(&attributes, sizeof(attributes), 1, file);

		m_vertices.push_back(firstVert);
		m_vertices.push_back(secondVert);
		m_vertices.push_back(thirdVert);

		m_normals.push_back(normal);
	}

	std::fclose(file);

	return true;
}


// reimplemented (imt3d::IMesh3d)
const CMesh3d::MeshVertices& CMesh3d::GetVertices() const
{
	return m_vertices;
}


const CMesh3d::MeshEdgesPtr CMesh3d::GetEdges() const
{
	//TODO
	return istd::TSmartPtr<MeshEdges>();
}


const CMesh3d::MeshIndexEdgesPtr CMesh3d::GetIndexEdges() const
{
	//TODO
	return istd::TSmartPtr<MeshIndexEdges>();
}


const CMesh3d::MeshTriangles& CMesh3d::GetTriangles() const
{
	return m_triangles;
}


const CMesh3d::MeshNormals& CMesh3d::GetNormals() const
{
	return m_normals;
}


// reimplemented (imt3d::IObject3d)

bool CMesh3d::IsEmpty() const
{
	return m_vertices.isEmpty();
}


i3d::CVector3d CMesh3d::GetCenter() const
{
	EnsureCenterCalculated();

	return m_meshCenter;
}


void CMesh3d::MoveCenterTo(const i3d::CVector3d& position)
{
	Q_UNUSED(position);

	m_isMeshCenterCalculationValid = false;
	m_isMeshCuboidCalculationValid = false;
}


CCuboid CMesh3d::GetBoundingCuboid() const
{
	EnsureCuboidCalculated();

	return m_meshCuboid;
}


// reimplemented (iser::ISerializable)

bool CMesh3d::Serialize(iser::IArchive& archive)
{
	Q_UNUSED(archive);

	return false;
}


// private methods

void CMesh3d::EnsureCenterCalculated() const
{
	if (!IsEmpty() && !m_isMeshCenterCalculationValid){
		const CCuboid boundingCuboid = GetBoundingCuboid();

		istd::CRange xRange(boundingCuboid.GetLeft(), boundingCuboid.GetRight());
		istd::CRange yRange(boundingCuboid.GetBottom(), boundingCuboid.GetTop());
		istd::CRange zRange(boundingCuboid.GetNear(), boundingCuboid.GetFar());

		if (xRange.IsValidNonEmpty() && yRange.IsValidNonEmpty() && zRange.IsValidNonEmpty()){
			m_meshCenter = i3d::CVector3d(
				xRange.GetValueFromAlpha(0.5),
				yRange.GetValueFromAlpha(0.5),
				zRange.GetValueFromAlpha(0.5));

			m_isMeshCenterCalculationValid = true;
		}
	}
}


void CMesh3d::EnsureCuboidCalculated() const
{
	if (!IsEmpty() && !m_isMeshCuboidCalculationValid){
		istd::CRange xRange(qInf(), -qInf());
		istd::CRange yRange(qInf(), -qInf());
		istd::CRange zRange(qInf(), -qInf());

		for (MeshVertices::const_iterator itVertex = m_vertices.constBegin(); itVertex != m_vertices.constEnd(); itVertex++){
			double x = itVertex->GetElement(E_X);
			double y = itVertex->GetElement(E_Y);
			double z = itVertex->GetElement(E_Z);

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

			m_meshCuboid = CCuboid(left, right, bottom, top, near, far);

			m_isMeshCuboidCalculationValid = true;
		}
	}
}


} // namespace imt3d


