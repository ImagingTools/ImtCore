#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>
#include <ibase/TModelObserverCompWrap.h>

// ImtCore includes
#include <imt3dgui/CView3dProviderComp.h>
#include <imt3dgui/CPointCloudViewComp.h>
#include <imt3dgui/CMeshViewComp.h>


/**
	Imt3dGuiPck package
*/
namespace Imt3dGuiPck
{


typedef imod::TModelWrap<imt3dgui::CView3dProviderComp> View3dProvider;
typedef icomp::TModelCompWrap<ibase::TModelObserverCompWrap<imt3dgui::CPointCloudViewComp>> PointCloudView;
typedef icomp::TModelCompWrap<ibase::TModelObserverCompWrap<imt3dgui::CMeshViewComp>> MeshView;


} // namespace Imt3dGuiPck



