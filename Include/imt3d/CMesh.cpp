#include <imt3d/CMesh.h>

namespace imt3d
{

	CMesh::CMesh() {

	}


// reimplemented from imt3d::IObject3d

	bool CMesh::IsEmpty() const {
		
		return true;
	}

	i3d::CVector3d CMesh::GetCenter() const {

		return i3d::CVector3d::GetZero();
	}

	void CMesh::MoveCenterTo(const i3d::CVector3d& position) {

		Q_UNUSED(position);
	}

	CCuboid CMesh::GetBoundingCuboid() const {

		return CCuboid::CCuboid();
	}


	//reimplemented iser::ISerializable
	bool CMesh::Serialize(iser::IArchive& archive)
	{
		return false;
	}

} // namespace imt3d