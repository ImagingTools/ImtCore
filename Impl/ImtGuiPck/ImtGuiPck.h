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
#include <imtgui/CMultiDocumentWorkspaceGuiComp.h>
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
#include <imtgui/CDesignManagerComp.h>
#include <imtwidgets/CMenuPanelDelegate.h>
#include <imtwidgets/CMenuPanelDelegateMin.h>
#include <imtgui/CVirtualKeyboardShowCommandComp.h>
#include <imtgui/CFullScreenCommandComp.h>
#include <imtgui/CSingleDocumentWorkspaceGuiComp.h>
#include <imtgui/CDocumentCollectionViewDelegateComp.h>
#include <imtgui/CMenuBasedCommandsProviderComp.h>
#include <imtgui/CTimeFilterParamEditorComp.h>
#include <imtgui/CPaginationGuiComp.h>
#include <imtgui/CPopupControllerComp.h>
#include <imtgui/CStandardPopupWidgetFactoryComp.h>
#include <imtgui/CPopupTestGuiComp.h>
#include <imtgui/CStandardClientAreaProviderComp.h>
#include <imtgui/TPopupControllerCompWrap.h>
#include <imtgui/CGuiElementModelComp.h>
#include <imtgui/CGuiElementContainerComp.h>


/**
	ImtGuiPck package
*/
namespace ImtGuiPck
{


typedef imtbase::TComponentFactoryComp<iqtgui::IGuiObject> GuiComponentFactory;
typedef imtgui::TPopupControllerCompWrap<
			imtgui::CThumbnailDecoratorGuiComp> ThumbnailDecoratorGui;
typedef imtgui::CSingleStatusProviderGuiComp SingleStatusProviderGui;
typedef imtgui::CMultiStatusProviderGuiComp MultiStatusProviderGui;
typedef icomp::TModelCompWrap<imtgui::CObjectCollectionViewComp> ObjectCollectionView;
typedef icomp::TModelCompWrap<imtgui::CMultiDocumentWorkspaceGuiComp> DocumentWorkspaceGui;
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
typedef icomp::TModelCompWrap<imtgui::CDesignManagerComp> DesignManager;
typedef imtgui::CVirtualKeyboardShowCommandComp VirtualKeyboardShowCommand;
typedef imtgui::CFullScreenCommandComp FullScreenCommand;
typedef icomp::TMakeComponentWrap<imtwidgets::CMenuPanelDelegate, imtwidgets::IMenuPanelDelegate> MenuPanelDelegate;
typedef icomp::TMakeComponentWrap<imtwidgets::CMenuPanelDelegateMin, imtwidgets::IMenuPanelDelegate> MenuPanelDelegateMin;
typedef imtgui::CSingleDocumentWorkspaceGuiComp SingleDocumentWorkspace;
typedef imtgui::CDocumentCollectionViewDelegateComp DocumentCollectionViewDelegate;
typedef icomp::TModelCompWrap<imtgui::CMenuBasedCommandsProviderComp> MenuBasedCommandsProvider;
typedef imtgui::CTimeFilterParamEditorComp TimeFilterParamEditor;
typedef imtgui::CPaginationGuiComp PaginationGui;
typedef imtgui::CPopupControllerComp PopupController;
typedef imtgui::CStandardPopupWidgetFactoryComp StandardPopupWidgetFactory;
typedef imtgui::CPopupTestGuiComp PopupTestGui;
typedef icomp::TModelCompWrap<imtgui::CStandardClientAreaProviderComp> StandardClientAreaProvider;
typedef imtgui::CGuiElementModelComp GuiElementModel;
typedef imtgui::CGuiElementContainerComp GuiElementContainer;


} // namespace ImtGuiPck


