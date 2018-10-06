#pragma once

// ACF includes
#include <istd/TArray.h>


// ImtCore includes
#include <imt3d/imt3d.h>
#include <imt3d/IObject3d.h>


namespace imt3d
{


/**
	Tetrahedral mesh.
*/
class CMesh: virtual public IObject3d
{
public:
	typedef imath::TVector<3, float> FloatVector3d;
	typedef FloatVector3d Vertex;
	typedef FloatVector3d Normal;

	/**
		Contains indices of the corresp Vertices
	*/
	typedef istd::TArray<int, 3> Triangle;

	CMesh();

	bool SaveToStlFile(const QString& filePath) const;
	bool LoadFromStlFile(const QString& filePath);

	// reimplemented (imt3d::IObject3d)
	bool IsEmpty() const;
	i3d::CVector3d GetCenter() const;
	void MoveCenterTo(const i3d::CVector3d& position);
	const CCuboid& GetBoundingCuboid() const;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

private:
	typedef std::vector<Vertex> Vertices;
	typedef std::vector<Normal> Normals;
	typedef std::vector<Triangle> Triangles;

	Vertices m_vertices;
	Normals m_normals;
	Triangles m_triangles;
};


} // namespace imt3d


