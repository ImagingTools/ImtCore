#include "ImtCorePck.h"


#include <icomp/export.h>



namespace ImtCorePck
{


I_EXPORT_PACKAGE("ImtCorePck", "ImagingTools core component package", "Core Basics");


I_EXPORT_COMPONENT(
			PointCloud,
			"3D-point cloud",
			"3D Points");

I_EXPORT_COMPONENT(
			ObjPointCloudPersistence,
			"OBJ-format based cloud point persistence",
			"3D Points OBJ Persistence");


} // namespace ImtCorePck


