#pragma once


// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Commands.h>


namespace imtclientgql
{


class CRemoteCommandsControllerComp:
			public imtclientgql::TClientRequestManagerCompWrap<
						sdl::imtbase::Commands::CGraphQlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<
				sdl::imtbase::Commands::CGraphQlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteCommandsControllerComp);
		I_ASSIGN_MULTI_0(m_typeIdsAttrPtr, "TypeIds", "Type ID-s", true);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtbase::Commands::CGraphQlHandlerCompBase)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual sdl::imtbase::Commands::CGuiElementContainer OnGetCommands(
				const sdl::imtbase::Commands::CGetCommandsGqlRequest& getCommandsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

protected:
	I_MULTIATTR(QByteArray, m_typeIdsAttrPtr);
};


} // namespace imtclientgql


