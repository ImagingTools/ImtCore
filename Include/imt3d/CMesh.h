#pragma once


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
	CMesh();

	// reimplemented from imt3d::IObject3d
	bool IsEmpty() const;
	i3d::CVector3d GetCenter() const;
	void MoveCenterTo(const i3d::CVector3d& position);
	CCuboid GetBoundingCuboid() const;

	// reimplemented iser::ISerializable
	bool Serialize(iser::IArchive& archive);
};


} // namespace imt3d


