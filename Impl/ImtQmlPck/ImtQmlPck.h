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
#include <imtqml/CUserRelatedSettingsControllerComp.h>
#include <imtqml/CCommandPermissionsFilterComp.h>
#include <imtqml/CObjectRepresentationCompBase.h>
#include <imtqml/CSelectionRepresentationComp.h>
#include <imtqml/CTextRepresentationComp.h>
#include <imtqml/CCompositeObjectRepresentationComp.h>
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
typedef imtqml::CCompositeObjectRepresentationComp CompositeObjectRepresentation;
typedef imtqml::CHeadersDataProviderComp HeadersDataProvider;
typedef imtqml::CObserverQmlComp ObserverQml;
typedef imtqml::CClientUserContextComp ClientUserContext;
typedef imtqml::CApplicationInfoDataProviderComp ApplicationInfoDataProvider;
typedef imtqml::CObjectViewDataProviderComp ObjectViewDataProvider;
typedef imtqml::CUserRelatedSettingsControllerComp UserRelatedSettingsController;
typedef imtqml::CCommandPermissionsFilterComp CommandPermissionsFilter;
typedef imtqml::CObjectRepresentationCompBase ObjectRepresentation;
typedef imtqml::CSelectionRepresentationComp SelectionRepresentation;
typedef imtqml::CTextRepresentationComp TextRepresentation;
typedef imtqml::CPageBasedGuiModelComp PageBasedModel;


} // namespace ImtQmlPck


