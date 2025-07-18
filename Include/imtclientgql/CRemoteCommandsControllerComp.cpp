#include <imtclientgql/CRemoteCommandsControllerComp.h>


namespace imtclientgql
{


// protected methods

// reimplemented (sdl::imtbase::Commands::CGraphQlHandlerCompBase)

bool CRemoteCommandsControllerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	sdl::imtbase::Commands::CGetCommandsGqlRequest getCommandsGqlRequest(gqlRequest, false);

	QByteArray getCommandsCommandId = getCommandsGqlRequest.GetCommandId();
	QByteArray requestCommandId = gqlRequest.GetCommandId();
	if (getCommandsCommandId != requestCommandId){
		bool isSupported = BaseClass::IsRequestSupported(gqlRequest);
		if (!isSupported){
			return false;
		}
	}

	sdl::imtbase::Commands::GetCommandsRequestArguments arguments = getCommandsGqlRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.has_value()){
		return false;
	}

	QByteArray typeId;
	if (arguments.input.Version_1_0->typeId){
		typeId = *arguments.input.Version_1_0->typeId;
	}

	if (!typeId.isEmpty() && getCommandsGqlRequest.IsValid()){
		return m_typeIdsAttrPtr.FindValue(typeId) != -1;
	}

	return false;
}


sdl::imtbase::Commands::CGuiElementContainer CRemoteCommandsControllerComp::OnGetCommands(
			const sdl::imtbase::Commands::CGetCommandsGqlRequest& /*getCommandsRequest*/,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::Commands::CGuiElementContainer::V1_0 response;

	if (!SendModelRequest<sdl::imtbase::Commands::CGuiElementContainer::V1_0, sdl::imtbase::Commands::CGuiElementContainer>(gqlRequest, response, errorMessage)){
		errorMessage = QString("Unable to get remote commands. Error: %1").arg(errorMessage);
		SendErrorMessage(0, errorMessage, "CRemoteCommandsControllerComp");
		return sdl::imtbase::Commands::CGuiElementContainer();
	}

	sdl::imtbase::Commands::CGuiElementContainer retVal;
	retVal.Version_1_0 = std::move(response);

	return retVal;
}


} // namespace imtclientgql


