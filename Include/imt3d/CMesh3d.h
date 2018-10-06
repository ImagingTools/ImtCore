#pragma once

// ACF includes
#include <istd/TArray.h>


// ImtCore includes
#include <imt3d/imt3d.h>
#include <imt3d/IMesh3d.h>


namespace imt3d
{


/**
	Tetrahedral mesh.
*/
class CMesh3d: virtual public IMesh3d
{
public:

	CMesh3d();

	bool SaveToStlFile(const QString& filePath) const;
	bool LoadFromStlFile(const QString& filePath);

	// reimplemented (imt3d::IMesh3d)
	virtual const MeshVertices& GetVertices() const;
	virtual const MeshEdgesPtr GetEdges() const;
	virtual const MeshIndexEdgesPtr GetIndexEdges() const;
	virtual const MeshTriangles& GetTriangles() const;

	// reimplemented (imt3d::IObject3d)
	bool IsEmpty() const;
	i3d::CVector3d GetCenter() const;
	void MoveCenterTo(const i3d::CVector3d& position);
	const CCuboid& GetBoundingCuboid() const;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:

	MeshVertices m_vertices;
	MeshNormals m_normals;
	MeshTriangles m_triangles;
};


} // namespace imt3d


