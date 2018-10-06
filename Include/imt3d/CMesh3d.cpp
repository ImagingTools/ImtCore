#include <imt3d/CMesh3d.h>


// STL includes
#include <array>


namespace imt3d
{


// static attributes
static const istd::TIndex<3> indexZero(0);
static const istd::TIndex<3> indexOne(1);
static const istd::TIndex<3> indexTwo(2);


CMesh3d::CMesh3d()
{
}


bool CMesh3d::SaveToStlFile(const QString& filePath) const
{
	if (m_vertices.size() < 1){
		return false;
	}

	std::FILE* file = std::fopen(filePath.toStdString().c_str(), "wb");
	
	if (file == NULL) {
		return false;
	}

	std::array<uint8_t, 80> stlHeader = { 0 };
	std::fwrite(stlHeader.data(), sizeof(stlHeader[0]), stlHeader.size(), file);

	uint32_t trianglesCount = static_cast<uint32_t>(m_triangles.size());
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

		uint16_t attributes = 0;
		std::fwrite(&attributes, sizeof(attributes), 1, file);
	}

	std::fclose(file);

	return true;
}


bool CMesh3d::LoadFromStlFile(const QString& filePath)
{
	m_vertices.clear();
	m_triangles.clear();

	std::FILE* file = std::fopen(filePath.toStdString().c_str(), "rb");
	if (file == NULL) {
		return false;
	}

	std::array<uint8_t, 80> stlHeader = { {0} };
	std::fread(stlHeader.data(), sizeof(stlHeader[0]), stlHeader.size(), file);

	uint32_t numTris = 0;
	std::fread(&numTris, sizeof(numTris), 1, file);

	for (uint32_t i = 0; i < numTris; ++i) {
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

		uint16_t attributes = 0;
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


// reimplemented (imt3d::IObject3d)

bool CMesh3d::IsEmpty() const
{
	bool nope = (m_vertices.size() > 0);
	return nope;
}


i3d::CVector3d CMesh3d::GetCenter() const
{
	return i3d::CVector3d::GetZero();
}


void CMesh3d::MoveCenterTo(const i3d::CVector3d& position)
{
	Q_UNUSED(position);
}


const CCuboid& CMesh3d::GetBoundingCuboid() const
{
	return CCuboid::GetEmpty();
}


// reimplemented (iser::ISerializable)

bool CMesh3d::Serialize(iser::IArchive& archive)
{
	Q_UNUSED(archive);

	return false;
}


} // namespace imt3d


