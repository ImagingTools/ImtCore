#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>


// ImtCore includes
#include <imtbase/CObjectContainerComp.h>
#include <imtbase/CDelegatedObjectContainerSupplierComp.h>
#include <imtbase/CMultiStatusProviderComp.h>

#include <imt3d/CPointCloud3d.h>
#include <imt3d/CObjPointCloudPersistenceComp.h>
#include <imt3d/CMesh3d.h>
#include <imt3d/CStlMeshPersistenceComp.h>
#include <imt3d/CDepthBitmap.h>


/**
	ImtCorePck package
*/
namespace ImtCorePck
{


typedef icomp::TModelCompWrap<imtbase::CObjectContainerComp> ObjectContainer;
typedef icomp::TModelCompWrap<imtbase::CDelegatedObjectContainerSupplierComp> DelegatedObjectContainerSupplier;
typedef icomp::TModelCompWrap<imtbase::CMultiStatusProviderComp> MultiStatusProvider;

typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap<imt3d::CPointCloud3d, imt3d::IPointCloud3d, imt3d::IObject3d, iser::ISerializable>> PointCloud;
typedef imt3d::CObjPointCloudPersistenceComp ObjPointCloudPersistence;

typedef icomp::TModelCompWrap<
	icomp::TMakeComponentWrap<imt3d::CMesh3d, imt3d::IMesh3d, imt3d::IObject3d, iser::ISerializable>> TetrahedralMesh3d;

typedef imt3d::CStlMeshPersistenceComp StlMeshPersistence;

typedef icomp::TModelCompWrap<
	icomp::TMakeComponentWrap<imt3d::CDepthBitmap,imt3d::IDepthBitmap, iimg::IQImageProvider, iimg::IBitmap, iimg::IRasterImage, i2d::IObject2d, iser::ISerializable>> DepthBitmap;


} // namespace ImtCorePck



