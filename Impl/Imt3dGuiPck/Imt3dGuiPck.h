#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>
#include <ibase/TModelObserverCompWrap.h>
#include <ibase/TMakeModelObserverCompWrap.h>

// ImtCore includes
#include <imt3dgui/CView3dProviderComp.h>
#include <imt3dgui/CPointCloudViewComp.h>
#include <imt3dgui/CMeshViewComp.h>
#include <imt3dgui/CDesignManagerComp.h>


/**
	Imt3dGuiPck package
*/
namespace Imt3dGuiPck
{


typedef imod::TModelWrap<imt3dgui::CView3dProviderComp> View3dProvider;
typedef icomp::TModelCompWrap<ibase::TModelObserverCompWrap<imt3dgui::CPointCloudViewComp>> PointCloudView;
typedef icomp::TModelCompWrap<ibase::TModelObserverCompWrap<imt3dgui::CMeshViewComp>> MeshView;
typedef icomp::TModelCompWrap<imt3dgui::CDesignManagerComp> DesignManager;
typedef ibase::TMakeModelObserverCompWrap<
			imt3dgui::CAxisShape,
			imt3dgui::IShape3d,
			imt3dview::IScene3dItem> AxisShape;
typedef ibase::TMakeModelObserverCompWrap<
			imt3dgui::CGridShape,
			imt3dgui::IShape3d,
			imt3dview::IScene3dItem> GridShape;
typedef ibase::TMakeModelObserverCompWrap<
			imt3dgui::CPointCloudShape,
			imt3dgui::IShape3d,
			imt3dview::IScene3dItem> PointCloudShape;
typedef ibase::TMakeModelObserverCompWrap<
			imt3dgui::CMeshShape,
			imt3dgui::IShape3d,
			imt3dview::IScene3dItem> MeshShape;


} // namespace Imt3dGuiPck



