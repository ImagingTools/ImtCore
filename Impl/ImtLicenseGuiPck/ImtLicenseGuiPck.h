#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtlicgui/CLicenseInfoEditorGuiComp.h>
#include <imtlicgui/CFeatureCollectionGuiComp.h>
#include <imtlicgui/CProductLicensingInfoGuiComp.h>
#include <imtlicgui/CLicenseManagerViewDelegateComp.h>
#include <imtlicgui/CFeatureCollectionViewDelegateComp.h>
#include <imtlicgui/CProductLicensingInfoCollectionViewDelegateComp.h>


/**
	ImtLicenseGuiPck package
*/
namespace ImtLicenseGuiPck
{

typedef imtlicgui::CLicenseInfoEditorGuiComp LicenseInfoEditorGui;
typedef icomp::TModelCompWrap<imtlicgui::CFeatureCollectionGuiComp> FeatureCollectionGui;
typedef imtlicgui::CProductLicensingInfoGuiComp ProductLicensingInfoGui;
typedef imtlicgui::CLicenseManagerViewDelegateComp LicenseManagerViewDelegate;
typedef imtlicgui::CFeatureCollectionViewDelegateComp FeatureCollectionViewDelegate;
typedef imtlicgui::CProductLicensingInfoCollectionViewDelegateComp ProductLicensingInfoCollectionViewDelegate;


} // namespace ImtLicenseGuiPck


