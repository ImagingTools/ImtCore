#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtlicgui/CLicenseInfoEditorGuiComp.h>
#include <imtlicgui/CFeaturePackageGuiComp.h>
#include <imtlicgui/CProductLicensingInfoGuiComp.h>
#include <imtlicgui/CLicenseManagerViewDelegateComp.h>
#include <imtlicgui/CFeatureCollectionViewDelegateComp.h>
#include <imtlicgui/CFeatureInfoEditorGuiComp.h>
#include <imtlicgui/CFeaturePackageViewDelegateComp.h>
#include <imtlicgui/CProductLicensingInfoViewDelegateComp.h>
#include <imtlicgui/CProductInstanceInfoViewDelegateComp.h>
#include <imtlicgui/CProductInstanceInfoEditorComp.h>
#include <imtlicgui/CProductInstanceInfoViewComp.h>
#include <imtlicgui/CFeatureTreeGuiComp.h>
#include <imtlicgui/CDesignManagerComp.h>


/**
	ImtLicenseGuiPck package
*/
namespace ImtLicenseGuiPck
{


typedef imtlicgui::CLicenseInfoEditorGuiComp LicenseInfoEditorGui;
typedef icomp::TModelCompWrap<imtlicgui::CFeaturePackageGuiComp> FeaturePackageGui;
typedef imtlicgui::CProductLicensingInfoGuiComp ProductLicensingInfoGui;
typedef imtlicgui::CLicenseManagerViewDelegateComp LicenseManagerViewDelegate;
typedef imtlicgui::CFeatureCollectionViewDelegateComp FeatureCollectionViewDelegate;
typedef imtlicgui::CFeatureInfoEditorGuiComp FeatureInfoEditorGui;
typedef imtlicgui::CFeaturePackageViewDelegateComp FeaturePackageViewDelegate;
typedef imtlicgui::CProductLicensingInfoViewDelegateComp ProductLicensingInfoViewDelegate;
typedef imtlicgui::CProductInstanceInfoViewDelegateComp ProductInstanceInfoViewDelegate;
typedef imtlicgui::CProductInstanceInfoEditorComp ProductInstanceInfoEditor;
typedef imtlicgui::CProductInstanceInfoViewComp ProductInstanceInfoView;
typedef imtlicgui::CFeatureTreeGuiComp FeatureTreeGui;
typedef icomp::TModelCompWrap<imtlicgui::CDesignManagerComp> DesignManager;


} // namespace ImtLicenseGuiPck


