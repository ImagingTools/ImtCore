#include "ImtCorePck.h"


#include <icomp/export.h>



namespace ImtCorePck
{


I_EXPORT_PACKAGE("ImtCorePck", "ImagingTools core component package", "Core Basics");


I_EXPORT_COMPONENT(
			ObjectContainer,
			"General container of the data objects",
			"Object Data Model Composition");

I_EXPORT_COMPONENT(
			PointCloud,
			"3D-point cloud",
			"3D Points");

I_EXPORT_COMPONENT(
			ObjPointCloudPersistence,
			"OBJ-format-based point cloud persistence",
			"3D Points OBJ Persistence");

I_EXPORT_COMPONENT(
			TetrahedralMesh3D,
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


} // namespace ImtCorePck


