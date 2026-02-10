// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtqml/CQuickApplicationComp.h>
#include <imtqml/CGqlQuickObjectComp.h>
#include <imtqml/CGuiQuickWrapComp.h>
#include <imtqml/CObserverQmlComp.h>
#include <imtqml/CClientUserContextComp.h>
#include <imtqml/CPageGuiElementModelComp.h>
#include <imtqml/CCommandsObserverQmlComp.h>
#include <imtqml/CQmlObjectCollectionComp.h>
#include <imtqml/CStaticQmlTypeRegistratorComp.h>
#include <imtqml/CStaticQmlBaseTypeRegistratorComp.h>
#include <imtqml/CStaticQmlModuleRegistratorComp.h>


/**
	ImtQmlPck package
*/
namespace ImtQmlPck
{


typedef imtqml::CQuickApplicationComp QuickApplication;
typedef imtqml::CGqlQuickObjectComp QuickObject;
typedef imtqml::CGuiQuickWrapComp GuiQuickWrap;
typedef imtqml::CObserverQmlComp ObserverQml;
typedef imtqml::CClientUserContextComp ClientUserContext;
typedef imtqml::CPageGuiElementModelComp PageGuiElementModel;
typedef icomp::TModelCompWrap<imtqml::CCommandsObserverQmlComp> CommandsObserverQml;
typedef imtqml::CQmlObjectCollectionComp QmlObjectCollection;
typedef imtqml::CStaticQmlTypeRegistratorComp StaticQmlTypeRegistrator;
typedef imtqml::CStaticQmlModuleRegistratorComp StaticQmlModuleRegistrator;
typedef	imtqml::CStaticQmlBaseTypeRegistratorComp StaticQmlBaseTypeRegistrator;


} // namespace ImtQmlPck


