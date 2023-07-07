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
#include <imtqml/CPageGuiElementModelComp.h>
#include <imtqml/CCommandsObserverQmlComp.h>
#include <imtqml/CClientMainWindowQmlComp.h>


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
typedef imtqml::CPageGuiElementModelComp PageGuiElementModel;
typedef imtqml::CClientMainWindowQmlComp ClientMainWindowQml;
typedef icomp::TModelCompWrap<imtqml::CCommandsObserverQmlComp> CommandsObserverQml;


} // namespace ImtQmlPck


