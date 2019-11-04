#include "Imt3dGuiPck.h"


#include <icomp/export.h>


I_REGISTER_QT_RESOURCE(imt3dgui)


namespace Imt3dGuiPck
{


I_EXPORT_PACKAGE("Imt3dGuiPck", "Easy GL Engine UI component package", "Easy GL Engine Gui")


I_EXPORT_COMPONENT(
			View3dProvider,
			"Eagle Gui widget",
			"Eagle Gui widget")

I_EXPORT_COMPONENT(
			PointCloudView,
			"Viewer of the 3D-point cloud",
			"Cloud 3D Points View")

I_EXPORT_COMPONENT(
			MeshView,
			"Viewer of the 3D mesh",
			"3D Mesh View")

} // namespace Imt3dGuiPck


