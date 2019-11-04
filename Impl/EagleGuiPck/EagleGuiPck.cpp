#include "EagleGuiPck.h"


#include <icomp/export.h>


I_REGISTER_QT_RESOURCE(eaglegui)


namespace EagleGuiPck
{


I_EXPORT_PACKAGE("EagleGuiPck", "Easy GL Engine UI component package", "Easy GL Engine Gui")


I_EXPORT_COMPONENT(
			EagleViewProvider,
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

} // namespace EagleGuiPck


