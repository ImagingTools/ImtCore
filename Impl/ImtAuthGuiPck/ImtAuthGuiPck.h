#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>
#include <icomp/TModelCompWrap.h>

// ImtCore includes
#include <imtauthgui/CContactInfoEditorComp.h>
#include <imtauthgui/CContactInfoRepresentationDataProviderComp.h>
#include <imtauthgui/CContactInfoRepresentationDataController.h>
#include <imtauthgui/CAddressEditorComp.h>
#include <imtauthgui/CAccountInfoEditorComp.h>
#include <imtauthgui/CContactInfoViewDelegateComp.h>
#include <imtauthgui/CAccountInfoViewDelegateComp.h>
#include <imtauthgui/CDesignManagerComp.h>
#include <imtauthgui/CStandardAccountMenuComp.h>
#include <imtauthgui/CStandardLoginGuiComp.h>


namespace ImtAuthGuiPck
{

typedef imtauthgui::CContactInfoEditorComp ContactInfoEditor;
typedef imtauthgui::CAddressEditorComp AddressEditor;
typedef imtauthgui::CAccountInfoEditorComp AccountInfoEditor;
typedef imtauthgui::CContactInfoViewDelegateComp ContactInfoViewDelegate;
typedef imtauthgui::CAccountInfoViewDelegateComp AccountInfoViewDelegate;
typedef icomp::TModelCompWrap<imtauthgui::CDesignManagerComp> DesignManager;
typedef icomp::TMakeComponentWrap<
							imtauthgui::CContactInfoRepresentationDataController,
							imtbase::IItemBasedRepresentationDataController> CContactInfoRepresentationDataController;
typedef imtauthgui::CStandardAccountMenuComp StandardAccountMenu;
typedef imtauthgui::CStandardLoginGuiComp StandardLoginGui;


} // namespace ImtAuthGuiPck


