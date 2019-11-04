#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>
#include <ibase/TModelObserverCompWrap.h>

// Eagle includes
#include <eaglegui/CEagleViewProviderComp.h>
#include <eaglegui/CPointCloudViewComp.h>
#include <eaglegui/CMeshViewComp.h>


/**
	EagleGui package
*/
namespace EagleGuiPck
{


typedef imod::TModelWrap<eaglegui::CEagleViewProviderComp> EagleViewProvider;
typedef icomp::TModelCompWrap<ibase::TModelObserverCompWrap<eaglegui::CPointCloudViewComp>> PointCloudView;
typedef icomp::TModelCompWrap<ibase::TModelObserverCompWrap<eaglegui::CMeshViewComp>> MeshView;


} // namespace EagleGuiPck



