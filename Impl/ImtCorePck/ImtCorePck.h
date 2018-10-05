#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>


// ImtCore includes
#include <imt3d/CPointCloud3d.h>
#include <imt3d/CObjPointCloudPersistenceComp.h>
#include <imt3d/CStlMeshPersistenceComp.h>


/**
	ImtCorePck package
*/
namespace ImtCorePck
{


typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<imt3d::CPointCloud3d, imt3d::IPointCloud3d, imt3d::IObject3d, iser::ISerializable>> PointCloud;
typedef imt3d::CObjPointCloudPersistenceComp ObjPointCloudPersistence;

typedef imt3d::CStlMeshPersistenceComp StlMeshPersistence;

} // namespace ImtCorePck



