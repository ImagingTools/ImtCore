#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtbase/TComponentFactoryComp.h>
#include <imtgui/CThumbnailDecoratorGuiComp.h>
#include <imtgui/CSingleStatusProviderGuiComp.h>
#include <imtgui/CMultiStatusProviderGuiComp.h>
#include <imtgui/CObjectCollectionViewComp.h>
#include <imtgui/CStandardCollectionViewDelegateComp.h>
#include <imtgui/CDocumentWorkspaceGuiComp.h>
#include <imtgui/CObjectCollectionMetaInfoViewComp.h>
#include <imtgui/CDelegatedCommandsProviderComp.h>
#include <imtgui/CMenuPanelComp.h>
#include <imtgui/CMonitorInfoProviderComp.h>
#include <imtgui/CLayoutManagerGuiComp.h>
#include <imtgui/CLayout.h>
#include <imtgui/CCompositeObjectViewComp.h>
#include <imtgui/CPageNavigationControllerComp.h>
#include <imtgui/CPageSwitcherComp.h>
#include <imtgui/CPluginStatusViewerComp.h>
#include <imtgui/CPluginStatusMonitorViewDelegateComp.h>
#include <imtgui/CApplicationSwitchCommandComp.h>


/**
	ImtGuiPck package
*/
namespace ImtGuiPck
{


typedef imtbase::TComponentFactoryComp<iqtgui::IGuiObject> GuiComponentFactory;
typedef imtgui::CThumbnailDecoratorGuiComp ThumbnailDecoratorGui;
typedef imtgui::CSingleStatusProviderGuiComp SingleStatusProviderGui;
typedef imtgui::CMultiStatusProviderGuiComp MultiStatusProviderGui;
typedef icomp::TModelCompWrap<imtgui::CObjectCollectionViewComp> ObjectCollectionView;
typedef icomp::TModelCompWrap<imtgui::CDocumentWorkspaceGuiComp> DocumentWorkspaceGui;
typedef imtgui::CObjectCollectionMetaInfoViewComp ObjectCollectionMetaInfoView;
typedef imtgui::CDelegatedCommandsProviderComp DelegatedCommandsProvider;
typedef imtgui::CMenuPanelComp MenuPanel;
typedef icomp::TModelCompWrap<imtgui::CMonitorInfoProviderComp> MonitorInfoProvider;
typedef icomp::TModelCompWrap<icomp::TMakeComponentWrap<imtgui::CLayout, imtgui::ILayout, iser::ISerializable>> LayoutManager;
typedef icomp::TModelCompWrap<imtgui::CLayoutManagerGuiComp> LayoutManagerGui;
typedef imtgui::CCompositeObjectViewComp CompositeObjectView;
typedef icomp::TModelCompWrap<imtgui::CPageNavigationControllerComp> PageNavigationController;
typedef ibase::TModelObserverCompWrap<imtgui::CPageSwitcherComp> PageSwitcher;
typedef imtgui::CStandardCollectionViewDelegateComp StandardCollectionViewDelegate;
typedef imtgui::CPluginStatusViewerComp PluginStatusViewer;
typedef imtgui::CPluginStatusMonitorViewDelegateComp PluginStatusMonitorViewDelegate;
typedef imtgui::CApplicationSwitchCommandComp ApplicationSwitchCommand;


} // namespace ImtGuiPck


