#pragma once


// ImtCore includes
#include <imtbase/IApplicationInfoRepresentation.h>
#include <imtgql/CGqlRepresentationDataControllerCompBase.h>
#include <GeneratedFiles/imtappsdl/SDL/1.0/CPP/Application.h>


namespace imtgql
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
	virtual sdl::imtapp::Application::V1_0::CApplicationInfo OnGetApplicationInfo(
				const sdl::imtapp::Application::V1_0::CGetApplicationInfoGqlRequest& getApplicationInfoRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

protected:
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
};


} // namespace imtgql


