#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtrepogui/CFileObjectCollectionViewDelegate.h>
#include <imtrepogui/CDocumentBasedFileCollectionViewDelegateComp.h>


/**
	ImtGuiPck package
*/
namespace ImtRepoGuiPck
{


typedef icomp::TMakeComponentWrap<imtrepogui::CFileObjectCollectionViewDelegate, imtgui::ICollectionViewDelegate> FileObjectCollectionViewDelegate;
typedef imtrepogui::CDocumentBasedFileCollectionViewDelegateComp DocumentBasedFileCollectionViewDelegate;


} // namespace ImtRepoGuiPck


