#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtqml/CQuickApplicationComp.h>
#include <imtqml/CQuickObjectCompBase.h>
#include <imtqml/CGuiQuickWrapComp.h>
#include <imtqml/CApplicationRepresentationDataProviderCompBase.h>
#include <imtqml/CApplicationDataProviderCompBase.h>
#include <imtqml/CCommandDataProviderCompBase.h>
#include <imtqml/CPageDataProviderCompBase.h>
#include <imtqml/CApplicationDataEnumProviderComp.h>
#include <imtqml/CPageDataEnumProviderComp.h>
/**
	ImtQmlPck package
*/
namespace ImtQmlPck
{


typedef imtqml::CQuickApplicationComp QuickApplication;
typedef imtqml::CQuickObjectCompBase QuickObject;
typedef imtqml::CGuiQuickWrapComp GuiQuickWrap;
typedef imtqml::CApplicationDataProviderCompBase ApplicationDataProvider;
typedef imtqml::CApplicationRepresentationDataProviderCompBase ApplicationRepresentationDataProvider;
typedef imtqml::CCommandDataProviderCompBase CommandDataProvider;
typedef imtqml::CPageDataProviderCompBase PageDataProvider;
typedef imtqml::CApplicationDataEnumProviderComp EnumApplicationDataProvider;
typedef imtqml::CPageDataEnumProviderComp EnumPageDataProvider;


} // namespace ImtQmlPck


