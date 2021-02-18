#include "Imt3dPck.h"


// ACF includes
#include <icomp/export.h>


namespace Imt3dPck
{


I_EXPORT_PACKAGE(
			"Imt3dPck",
			"ImagingTools 3D-geometry component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

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


} // namespace Imt3dPck


