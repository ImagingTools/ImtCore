#include <imt3d/CMesh.h>


// STL includes
#include <array>


namespace imt3d
{


// static attributes
static const istd::TIndex<3> indexZero(0);
static const istd::TIndex<3> indexOne(1);
static const istd::TIndex<3> indexTwo(2);


CMesh::CMesh()
{
	m_vertices.clear();
	m_triangles.clear();
}


bool CMesh::SaveToStlFile(const QString& filePath) const
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

		Vertex firstVert = m_vertices[oneTriangle.GetAt(indexZero)];
		Vertex secondVert = m_vertices[oneTriangle.GetAt(indexOne)];
		Vertex thirdVert = m_vertices[oneTriangle.GetAt(indexTwo)];

		std::fwrite(&firstVert[0], sizeof(float), 3, file);
		std::fwrite(&secondVert[0], sizeof(float), 3, file);
		std::fwrite(&thirdVert[0], sizeof(float), 3, file);

		uint16_t attributes = 0;
		std::fwrite(&attributes, sizeof(attributes), 1, file);
	}

	std::fclose(file);

	return true;
}


bool CMesh::LoadFromStlFile(const QString& filePath)
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
		oneTriangle.SetAt(indexZero, i * 3);
		oneTriangle.SetAt(indexOne, i * 3 + 1);
		oneTriangle.SetAt(indexTwo, i * 3 + 2);

		m_triangles.emplace_back(oneTriangle);

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


// reimplemented (imt3d::IObject3d)

bool CMesh::IsEmpty() const
{
	bool nope = (m_vertices.size() > 0);
	return nope;
}


i3d::CVector3d CMesh::GetCenter() const
{
	return i3d::CVector3d::GetZero();
}


void CMesh::MoveCenterTo(const i3d::CVector3d& position)
{
	Q_UNUSED(position);
}


CCuboid CMesh::GetBoundingCuboid() const
{
	return CCuboid::CCuboid();
}


// reimplemented (iser::ISerializable)

bool CMesh::Serialize(iser::IArchive& archive)
{
	Q_UNUSED(archive);

	return false;
}


} // namespace imt3d


