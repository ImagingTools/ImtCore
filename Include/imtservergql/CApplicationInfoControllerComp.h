// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ibase/IApplicationInfo.h>

// ImtCore includes
#include <GeneratedFiles/imtappsdl/SDL/1.0/CPP/Application.h>


namespace imtservergql
{


class CApplicationInfoControllerComp: public sdl::imtapp::Application::CGraphQlHandlerCompBase
{
public:
	using BaseClass = sdl::imtapp::Application::CGraphQlHandlerCompBase;
	using ApplicationInfo = sdl::imtapp::Application::CApplicationInfo;

	I_BEGIN_COMPONENT(CApplicationInfoControllerComp);
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application data info", true, "ApplicationInfo");
		I_ASSIGN(m_applicationPreferencesCompPtr, "ApplicationPreferences", "Optional reference to ParamSet written to XML file containing application preferences. May overwrite default ApplicationInfo attrubutes if configured", false, "IParamsSet");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtapp::Application::CGraphQlHandlerCompBase)
	virtual ApplicationInfo OnGetApplicationInfo(
				const sdl::imtapp::Application::CGetApplicationInfoGqlRequest& getApplicationInfoRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

protected:
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
	I_REF(iprm::IParamsSet, m_applicationPreferencesCompPtr);
};


} // namespace imtservergql


