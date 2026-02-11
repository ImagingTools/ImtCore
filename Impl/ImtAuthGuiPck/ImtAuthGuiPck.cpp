// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "ImtAuthGuiPck.h"


// ACF includes
#include <icomp/export.h>


namespace ImtAuthGuiPck
{


I_EXPORT_PACKAGE(
			"ImtAuthGuiPck",
			"ImagingTools license management UI-component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			ContactInfoEditor,
			"Contact info editor",
			"Contact Info Editor");

I_EXPORT_COMPONENT(
			AddressEditor,
			"Address editor",
			"Address Editor");

I_EXPORT_COMPONENT(
			AccountInfoEditor,
			"Account info editor",
			"Account Info Editor");

I_EXPORT_COMPONENT(
			ContactInfoViewDelegate,
			"Contact info view delegate",
			"Contact Address Info View Delegate");

I_EXPORT_COMPONENT(
			AccountInfoViewDelegate,
			"Account info view delegate",
			"Account Info View Delegate");

I_EXPORT_COMPONENT(
			StandardAccountMenu,
			"Standard account menu",
			"Account Menu Login Logout");

I_EXPORT_COMPONENT(
			StandardLoginGui,
			"Standard login gui",
			"Login Logout Gui");

I_EXPORT_COMPONENT(
			RemoteStandardLoginGui,
			"Remote Standard login gui",
			"Remote Login Logout Gui");

I_EXPORT_COMPONENT(
			AdministrationObserverQml,
			"Administration observer qml",
			"Administration Observer Qm");

I_EXPORT_COMPONENT(
			ServerConnectionEditor,
			"Server connection editor",
			"Server connection editor");


} // namespace ImtAuthGuiPck


