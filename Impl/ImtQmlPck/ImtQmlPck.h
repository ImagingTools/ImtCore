#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtqml/CQuickApplicationComp.h>
#include <imtqml/CQuickObjectCompBase.h>
#include <imtqml/CGuiQuickWrapComp.h>
#include <imtqml/CCommandDataProviderComp.h>
#include <imtqml/CPageDataProviderComp.h>
#include <imtqml/CHeadersDataProviderComp.h>
#include <imtqml/CObserverQmlComp.h>
#include <imtqml/CClientUserContextComp.h>
#include <imtqml/CApplicationInfoDataProviderComp.h>
#include <imtqml/CObjectViewDataProviderComp.h>
#include <imtqml/CCommandPermissionsFilterComp.h>
#include <imtqml/CPageBasedGuiModelComp.h>


/**
	ImtQmlPck package
*/
namespace ImtQmlPck
{


typedef imtqml::CQuickApplicationComp QuickApplication;
typedef imtqml::CQuickObjectCompBase QuickObject;
typedef imtqml::CGuiQuickWrapComp GuiQuickWrap;
typedef imtqml::CCommandDataProviderComp CommandDataProvider;
typedef imtqml::CPageDataProviderComp PageDataProvider;
typedef imtqml::CHeadersDataProviderComp HeadersDataProvider;
typedef imtqml::CObserverQmlComp ObserverQml;
typedef imtqml::CClientUserContextComp ClientUserContext;
typedef imtqml::CApplicationInfoDataProviderComp ApplicationInfoDataProvider;
typedef imtqml::CObjectViewDataProviderComp ObjectViewDataProvider;
typedef imtqml::CCommandPermissionsFilterComp CommandPermissionsFilter;
typedef imtqml::CPageBasedGuiModelComp PageBasedModel;


} // namespace ImtQmlPck


