#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtgui/CThumbnailDecoratorGuiComp.h>
#include <imtgui/CSingleStatusProviderGuiComp.h>
#include <imtgui/CMultiStatusProviderGuiComp.h>
#include <imtgui/CObjectCollectionViewComp.h>
#include <imtgui/CFileObjectCollectionViewDelegate.h>
#include <imtgui/CDocumentBasedFileCollectionDelegateComp.h>
#include <imtgui/CDocumentWorkspaceGuiComp.h>
#include <imtgui/CFileCollectionMetaInfoViewComp.h>
#include <imtgui/CDelegatedCommandsProviderComp.h>
#include <imtgui/CMenuPanelComp.h>
#include <imtgui/CMonitorInfoProviderComp.h>
#include <imtgui/CLayoutManagerComp.h>
#include <imtgui/CCompositeObjectViewComp.h>


/**
	ImtGuiPck package
*/
namespace ImtGuiPck
{


typedef imtgui::CThumbnailDecoratorGuiComp ThumbnailDecoratorGui;
typedef imtgui::CSingleStatusProviderGuiComp SingleStatusProviderGui;
typedef imtgui::CMultiStatusProviderGuiComp MultiStatusProviderGui;
typedef imtgui::CObjectCollectionViewComp ObjectCollectionView;
typedef icomp::TMakeComponentWrap<imtgui::CFileObjectCollectionViewDelegate, imtgui::ICollectionViewDelegate> FileObjectCollectionViewDelegate;
typedef imtgui::CDocumentBasedFileCollectionDelegateComp DocumentBasedFileCollectionDelegate;
typedef icomp::TModelCompWrap<imtgui::CDocumentWorkspaceGuiComp> DocumentWorkspaceGui;
typedef imtgui::CFileCollectionMetaInfoViewComp FileCollectionMetaInfoView;
typedef imtgui::CDelegatedCommandsProviderComp DelegatedCommandsProvider;
typedef imtgui::CMenuPanelComp MenuPanel;
typedef icomp::TModelCompWrap<imtgui::CMonitorInfoProviderComp> MonitorInfoProvider;
typedef imtgui::CLayoutManagerComp LayoutManager;
typedef imtgui::CCompositeObjectViewComp CompositeObjectView;


} // namespace ImtGuiPck


