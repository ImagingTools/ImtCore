#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtqml/CQuickApplicationComp.h>
#include <imtqml/CQuickObjectCompBase.h>
#include <imtqml/CGuiQuickWrapComp.h>
#include <imtqml/CCommandDataProviderCompBase.h>
#include <imtqml/CPageDataProviderCompBase.h>
#include <imtqml/CParamsDataProviderCompBase.h>
#include <imtqml/CDatabaseSettingsDataProviderComp.h>
#include <imtqml/CApplicationDataEnumProviderComp.h>
#include <imtqml/CPageDataEnumProviderComp.h>
#include <imtqml/CCommandDataEnumProviderComp.h>
#include <imtqml/CMenuDataEnumProviderComp.h>
#include <imtqml/CJoinDataProviderComp.h>
#include <imtqml/CHeadersDataProviderComp.h>
#include <imtqml/CObserverQmlComp.h>
#include <imtqml/CClientUserContextComp.h>


/**
	ImtQmlPck package
*/
namespace ImtQmlPck
{


typedef imtqml::CQuickApplicationComp QuickApplication;
typedef imtqml::CQuickObjectCompBase QuickObject;
typedef imtqml::CGuiQuickWrapComp GuiQuickWrap;
typedef imtqml::CCommandDataProviderCompBase CommandDataProvider;
typedef imtqml::CPageDataProviderCompBase PageDataProvider;
typedef imtqml::CParamsDataProviderCompBase ParamsDataProvider;
typedef imtqml::CDatabaseSettingsDataProviderComp DatabaseSettingsDataProvider;
typedef imtqml::CApplicationDataEnumProviderComp ApplicationDataEnumProvider;
typedef imtqml::CPageDataEnumProviderComp PageDataEnumProvider;
typedef imtqml::CCommandDataEnumProviderComp CommandDataEnumProvider;
typedef imtqml::CMenuDataEnumProviderComp MenuDataEnumProvider;
typedef imtqml::CJoinDataProviderComp JoinDataProvider;
typedef imtqml::CHeadersDataProviderComp HeadersDataProvider;
typedef imtqml::CObserverQmlComp ObserverQml;
typedef imtqml::CClientUserContextComp Context;


} // namespace ImtQmlPck


