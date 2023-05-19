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

I_EXPORT_COMPONENT(
			CompositeShapeView,
			"3D View for list of shapes",
			"3D View");

I_EXPORT_COMPONENT(
			AxisShape,
			"3D-axis shape",
			"3D Shape Axis");

I_EXPORT_COMPONENT(
			GridShape,
			"3D-grid shape",
			"3D Shape Grid");

I_EXPORT_COMPONENT(
			PointCloudShape,
			"3D-point could shape",
			"3D Shape Point-Cloud");

I_EXPORT_COMPONENT(
			MeshShape,
			"3D-mesh shape",
			"3D Shape Mesh");

I_EXPORT_COMPONENT(
			WireShape,
			"3D-wire shape",
			"3D Shape Wire");

} // namespace Imt3dGuiPck


