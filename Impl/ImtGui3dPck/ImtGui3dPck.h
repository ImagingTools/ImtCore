#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>
#include <ibase/TModelObserverCompWrap.h>

// ImtCore includes
#include <imtgui3d/CView3dProviderComp.h>
#include <imtgui3d/CPointCloudViewComp.h>
#include <imtgui3d/CMeshViewComp.h>


/**
	ImtGui3dPck package
*/
namespace ImtGui3dPck
{


typedef imod::TModelWrap<imtgui3d::CView3dProviderComp> View3dProvider;
typedef icomp::TModelCompWrap<ibase::TModelObserverCompWrap<imtgui3d::CPointCloudViewComp>> PointCloudView;
typedef icomp::TModelCompWrap<ibase::TModelObserverCompWrap<imtgui3d::CMeshViewComp>> MeshView;


} // namespace ImtGui3dPck



