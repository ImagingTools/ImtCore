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
#include <imtqml/CCollectionManagerBridge.h>
#include <imtqml/CDataModelBridgeDemultiplexer.h>
#include <imtqml/CDocumentManagerBridge.h>
#include <imtqml/CGqlClientBridge.h>
#include <imtqml/CGqlCollectionManagerBridge.h>
#include <imtqml/CGqlDataModelBridge.h>
#include <imtqml/CGqlDocumentManagerBridge.h>
#include <imtqml/CRepresentationControllerBridge.h>
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
typedef imtqml::CGqlClientBridge GqlClientBridge;
typedef imtqml::CGqlDocumentManagerBridge GqlDocumentManagerBridge;
typedef imtqml::CGqlCollectionManagerBridge GqlCollectionManagerBridge;
typedef imtqml::CGqlDataModelBridge GqlDataModelBridge;
typedef imtqml::CDocumentManagerBridge DocumentManagerBridge;
typedef imtqml::CCollectionManagerBridge CollectionManagerBridge;
typedef imtqml::CDataModelBridgeDemultiplexer DataModelBridge;
typedef imtqml::CRepresentationControllerBridge RepresentationControllerBridge;
typedef imtqml::CPageGuiElementModelComp PageGuiElementModel;
typedef icomp::TModelCompWrap<imtqml::CCommandsObserverQmlComp> CommandsObserverQml;
typedef imtqml::CQmlObjectCollectionComp QmlObjectCollection;
typedef imtqml::CStaticQmlTypeRegistratorComp StaticQmlTypeRegistrator;
typedef imtqml::CStaticQmlModuleRegistratorComp StaticQmlModuleRegistrator;
typedef	imtqml::CStaticQmlBaseTypeRegistratorComp StaticQmlBaseTypeRegistrator;


} // namespace ImtQmlPck


