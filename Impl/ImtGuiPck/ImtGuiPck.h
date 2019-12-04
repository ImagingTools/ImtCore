#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtgui/CThumbnailDecoratorGuiComp.h>
#include <imtgui/CSingleStatusProviderGuiComp.h>
#include <imtgui/CMultiStatusProviderGuiComp.h>
#include <imtgui/CObjectCollectionViewComp.h>
#include <imtgui/CFileObjectCollectionViewDelegate.h>
#include <imtgui/CDocumentBasedFileCollectionDelegateComp.h>
#include <imtgui/CCollectionDocumentWorkspaceGuiComp.h>


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
typedef imtgui::CCollectionDocumentWorkspaceGuiComp CollectionDocumentWorkspaceGui;


} // namespace ImtProc3dPck


