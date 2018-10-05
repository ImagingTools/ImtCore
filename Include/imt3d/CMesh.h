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

	// still to be considered. 
	// Each triangle needs pointers to neighboring triangles.. (I think)

	typedef imath::TVector<3, float> Vector3d_f;

	typedef Vector3d_f Vertex;
	typedef Vector3d_f Normal;
	typedef istd::TArray<int, 3> Triangle; // contains indices of the corresp Vertices
	//typedef istd::TArray<Vertex*, 3> Triangle;


	CMesh();

	bool SaveToStlFile(const QString& filePath) const;
	bool LoadFromStlFile(const QString& filePath);

	// reimplemented from imt3d::IObject3d
	bool IsEmpty() const;
	i3d::CVector3d GetCenter() const;
	void MoveCenterTo(const i3d::CVector3d& position);
	CCuboid GetBoundingCuboid() const;

	// reimplemented iser::ISerializable
	bool Serialize(iser::IArchive& archive);


private:
	std::vector<Vertex> Vertices;
	std::vector<Normal> Normals;
	std::vector<Triangle> Triangles;
};


} // namespace imt3d


