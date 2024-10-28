#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtqml/CQuickApplicationComp.h>
#include <imtqml/CQuickObjectCompBase.h>
#include <imtqml/CGuiQuickWrapComp.h>
#include <imtqml/CObserverQmlComp.h>
#include <imtqml/CClientUserContextComp.h>
#include <imtqml/CPageGuiElementModelComp.h>
#include <imtqml/CCommandsObserverQmlComp.h>
#include <imtqml/CClientMainWindowQmlComp.h>
#include <imtqml/CQmlObjectCollectionComp.h>
#include <imtqml/CObjectViewDataProviderComp.h>


/**
	ImtQmlPck package
*/
namespace ImtQmlPck
{


typedef imtqml::CQuickApplicationComp QuickApplication;
typedef imtqml::CQuickObjectCompBase QuickObject;
typedef imtqml::CGuiQuickWrapComp GuiQuickWrap;
typedef imtqml::CObserverQmlComp ObserverQml;
typedef imtqml::CClientUserContextComp ClientUserContext;
typedef imtqml::CPageGuiElementModelComp PageGuiElementModel;
typedef imtqml::CClientMainWindowQmlComp ClientMainWindowQml;
typedef icomp::TModelCompWrap<imtqml::CCommandsObserverQmlComp> CommandsObserverQml;
typedef imtqml::CQmlObjectCollectionComp QmlObjectCollection;
typedef imtqml::CObjectViewDataProviderComp ObjectViewDataProvider;


} // namespace ImtQmlPck


