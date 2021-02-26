#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtlicgui/CLicenseInfoEditorGuiComp.h>
#include <imtlicgui/CFeatureInfoProviderGuiComp.h>
#include <imtlicgui/CProductLicensingInfoGuiComp.h>
#include <imtlicgui/CLicenseManagerViewDelegateComp.h>
#include <imtlicgui/CFeatureInfoProviderCollectionViewDelegateComp.h>
#include <imtlicgui/CProductLicensingInfoCollectionViewDelegateComp.h>


/**
	ImtLicenseGuiPck package
*/
namespace ImtLicenseGuiPck
{

typedef imtlicgui::CLicenseInfoEditorGuiComp LicenseInfoEditorGui;
typedef icomp::TModelCompWrap<imtlicgui::CFeatureInfoProviderGuiComp> FeatureInfoProviderGui;
typedef imtlicgui::CProductLicensingInfoGuiComp ProductLicensingInfoGui;
typedef imtlicgui::CLicenseManagerViewDelegateComp LicenseManagerViewDelegate;
typedef imtlicgui::CFeatureInfoProviderCollectionViewDelegateComp FeatureInfoProviderCollectionViewDelegate;
typedef imtlicgui::CProductLicensingInfoCollectionViewDelegateComp ProductLicensingInfoCollectionViewDelegate;


} // namespace ImtLicenseGuiPck


