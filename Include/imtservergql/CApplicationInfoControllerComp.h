#pragma once


// ACF includes
#include <ibase/IApplicationInfo.h>

// ImtCore includes
#include <GeneratedFiles/imtappsdl/SDL/1.0/CPP/Application.h>


namespace imtservergql
{


class CApplicationInfoControllerComp: public sdl::imtapp::Application::V1_0::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtapp::Application::V1_0::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CApplicationInfoControllerComp);
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application data info", true, "ApplicationInfo");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtapp::Application::V1_0::CGraphQlHandlerCompBase)
	virtual sdl::imtapp::Application::CApplicationInfo::V1_0 OnGetApplicationInfo(
				const sdl::imtapp::Application::V1_0::CGetApplicationInfoGqlRequest& getApplicationInfoRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

protected:
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
};


} // namespace imtservergql


