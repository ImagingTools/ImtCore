// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtrestdemo/CDemoAdminServletComp.h>
#include <imtrestdemo/CDemoBehaviorServletComp.h>
#include <imtrestdemo/CDemoBodyPolicyComp.h>
#include <imtrestdemo/CDemoDynamicBodyServletComp.h>
#include <imtrestdemo/CDemoEchoServletComp.h>
#include <imtrestdemo/CDemoFileResolverComp.h>
#include <imtrestdemo/CDemoInfoServletComp.h>
#include <imtrestdemo/CDemoSettingsControllerComp.h>
#include <imtrestdemo/CDemoStreamServletComp.h>
#include <imtrestdemo/CDemoUploadServletComp.h>


/**
	ImtRestDemoPck package
*/
namespace ImtRestDemoPck
{


typedef imtrestdemo::CDemoBodyPolicyComp DemoBodyPolicy;
typedef imtrestdemo::CDemoFileResolverComp DemoFileResolver;
typedef imtrestdemo::CDemoEchoServletComp DemoEchoServlet;
typedef imtrestdemo::CDemoInfoServletComp DemoInfoServlet;
typedef imtrestdemo::CDemoUploadServletComp DemoUploadServlet;
typedef imtrestdemo::CDemoDynamicBodyServletComp DemoDynamicBodyServlet;
typedef imtrestdemo::CDemoStreamServletComp DemoStreamServlet;
typedef imtrestdemo::CDemoBehaviorServletComp DemoBehaviorServlet;
typedef imtrestdemo::CDemoAdminServletComp DemoAdminServlet;
typedef imtrestdemo::CDemoSettingsControllerComp DemoSettingsController;


} // namespace ImtRestDemoPck


