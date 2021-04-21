#include "Imt3dGuiPck.h"


#include <icomp/export.h>


I_REGISTER_QT_RESOURCE(imt3dguilight)


namespace Imt3dGuiPck
{


I_EXPORT_PACKAGE(
			"Imt3dGuiPck",
			"ImagingTools 3D-geometry UI-component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			View3dProvider,
			"3D-view provider",
			"3D View Scene")

I_EXPORT_COMPONENT(
			PointCloudView,
			"Viewer of the 3D-point cloud",
			"Cloud 3D Points View")

I_EXPORT_COMPONENT(
			MeshView,
			"Viewer of the 3D mesh",
			"3D Mesh View")

I_EXPORT_COMPONENT(
			DesignManager,
			"Design Manager",
			"Style Design Manager");


} // namespace Imt3dGuiPck


