#include "ImtCorePck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtCorePck
{


I_EXPORT_PACKAGE(
			"ImtCorePck",
			"ImagingTools core component package",
			IM_PROJECT("\"Core Framework\"") IM_COMPANY("ImagingTools") IM_AUTHOR("\"Kirill Lepskiy\""));

I_EXPORT_COMPONENT(
			ObjectContainer,
			"General container of the data objects",
			"Object Data Model Composition");

I_EXPORT_COMPONENT(
			DelegatedObjectContainerSupplier,
			"Delegated provider of a composite object (object container)",
			"Delegator Object Data Model Composition Supplier Provider");

I_EXPORT_COMPONENT(
			MultiStatusProvider,
			"Multiple information status provider",
			"Multiple Information Status Provider");

I_EXPORT_COMPONENT(
			MultiStatusManager,
			"Multiple information status manager",
			"Multiple Information Status Manager");

I_EXPORT_COMPONENT(
			PointCloud,
			"3D-point cloud",
			"3D Points");

I_EXPORT_COMPONENT(
			ObjPointCloudPersistence,
			"OBJ-format-based point cloud persistence",
			"3D Points OBJ Persistence");

I_EXPORT_COMPONENT(
			TetrahedralMesh3d,
			"3D tetrahedral mesh",
			"3D Mesh Triangle Tetrahedral");

I_EXPORT_COMPONENT(
			StlMeshPersistence,
			"STL-format mesh persistence",
			"Mesh STL Persistence");

I_EXPORT_COMPONENT(
			DepthBitmap,
			"Depth bitmap",
			"Bitmap 3D Depth Image");

I_EXPORT_COMPONENT(
			StatusManager,
			"Information status manager",
			"Information Status Manager");


} // namespace ImtCorePck


