#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtlicgui/CLicenseInfoEditorGuiComp.h>
#include <imtlicgui/CLicenseManagerViewDelegateComp.h>
#include <imtlicgui/CFeatureCollectionViewDelegateComp.h>
#include <imtlicgui/CFeatureInfoEditorGuiComp.h>
#include <imtlicgui/CProductLicensingInfoViewDelegateComp.h>
#include <imtlicgui/CProductInstanceInfoViewDelegateComp.h>
#include <imtlicgui/CProductInstanceInfoEditorComp.h>
#include <imtlicgui/CProductInstanceInfoViewComp.h>


/**
	ImtLicenseGuiPck package
*/
namespace ImtLicenseGuiPck
{


typedef imtlicgui::CLicenseInfoEditorGuiComp LicenseInfoEditorGui;
typedef imtlicgui::CLicenseManagerViewDelegateComp LicenseManagerViewDelegate;
typedef imtlicgui::CFeatureCollectionViewDelegateComp FeatureCollectionViewDelegate;
typedef imtlicgui::CFeatureInfoEditorGuiComp FeatureInfoEditorGui;
typedef imtlicgui::CProductLicensingInfoViewDelegateComp ProductLicensingInfoViewDelegate;
typedef imtlicgui::CProductInstanceInfoViewDelegateComp ProductInstanceInfoViewDelegate;
typedef imtlicgui::CProductInstanceInfoEditorComp ProductInstanceInfoEditor;
typedef imtlicgui::CProductInstanceInfoViewComp ProductInstanceInfoView;


} // namespace ImtLicenseGuiPck


