// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CRemoteCommandsControllerComp.h>


namespace imtclientgql
{


// protected methods

// reimplemented (sdl::imtbase::Commands::CGraphQlHandlerCompBase)

bool CRemoteCommandsControllerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	bool isSupported = BaseClass::IsRequestSupported(gqlRequest);
	if (isSupported){
		const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
		if (inputParamPtr == nullptr){
			return false;
		}

		if (m_typeIdsAttrPtr.IsValid()){
			QByteArray typeId = inputParamPtr->GetParamArgumentValue("typeId").toByteArray();
			return m_typeIdsAttrPtr.FindValue(typeId) != -1;
		}
	}

	return false;
}


sdl::imtbase::Commands::CGuiElementContainer CRemoteCommandsControllerComp::OnGetCommands(
			const sdl::imtbase::Commands::CGetCommandsGqlRequest& /*getCommandsRequest*/,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	return SendModelRequest<sdl::imtbase::Commands::CGuiElementContainer>(gqlRequest, errorMessage);
}


} // namespace imtclientgql


