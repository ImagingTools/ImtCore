// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CServerConnectionParamRepresentationControllerComp.h>


// ImtCore includes
#include <imtcom/IServerConnectionInterface.h>


namespace imtserverapp
{


// protected methods

// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CUrlParam::V1_0>)

QByteArray CServerConnectionParamRepresentationControllerComp::GetTypeId() const
{
	return sdl::imtbase::ImtBaseTypes::CParamTypeIds::V1_0::ParamTypeIdsFields::ServerConnectionParam.toUtf8();
}


bool CServerConnectionParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const auto* serverConnectionParamPtr = dynamic_cast<const imtcom::IServerConnectionInterface*>(&dataModel);

	return serverConnectionParamPtr != nullptr;
}


bool CServerConnectionParamRepresentationControllerComp::GetSdlRepresentationFromDataModel(
			sdl::imtbase::ImtBaseTypes::CServerConnectionParam::V1_0& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const imtcom::IServerConnectionInterface* serverConnectionParamPtr =
				dynamic_cast<const imtcom::IServerConnectionInterface*>(&dataModel);
	Q_ASSERT(serverConnectionParamPtr != nullptr);
	if (serverConnectionParamPtr == nullptr){
		return false;
	}

	const QString host = serverConnectionParamPtr->GetHost();
	sdlRepresentation.host = host;

	int flags = serverConnectionParamPtr->GetConnectionFlags();


	switch (flags){
	case imtcom::IServerConnectionInterface::CF_DEFAULT:
		sdlRepresentation.isSecure = false;
		break;
	case imtcom::IServerConnectionInterface::CF_SECURE:
		sdlRepresentation.isSecure = true;
		break;
	};

	int httpPort = serverConnectionParamPtr->GetPort(imtcom::IServerConnectionInterface::PT_HTTP);
	sdlRepresentation.httpPort = httpPort;

	int wsPort = serverConnectionParamPtr->GetPort(imtcom::IServerConnectionInterface::PT_WEBSOCKET);
	sdlRepresentation.wsPort = wsPort;

	return true;
}


bool CServerConnectionParamRepresentationControllerComp::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::imtbase::ImtBaseTypes::CServerConnectionParam::V1_0& sdlRepresentation) const
{
	imtcom::IServerConnectionInterface* serverConnectionParamPtr =
				dynamic_cast<imtcom::IServerConnectionInterface*>(&dataModel);
	if (serverConnectionParamPtr == nullptr){
		return false;
	}

	if (sdlRepresentation.host){
		QString host = *sdlRepresentation.host;
		serverConnectionParamPtr->SetHost(host);
	}

	if (sdlRepresentation.httpPort){
		int httpPort = *sdlRepresentation.httpPort;
		serverConnectionParamPtr->SetPort(imtcom::IServerConnectionInterface::PT_HTTP, httpPort);
	}

	if (sdlRepresentation.wsPort){
		int wsPort = *sdlRepresentation.wsPort;
		serverConnectionParamPtr->SetPort(imtcom::IServerConnectionInterface::PT_WEBSOCKET, wsPort);
	}

	if (sdlRepresentation.isSecure){
		bool isSecure = *sdlRepresentation.isSecure;
		if (isSecure){
			serverConnectionParamPtr->SetConnectionFlags(imtcom::IServerConnectionInterface::CF_SECURE);
		}
		else{
			serverConnectionParamPtr->SetConnectionFlags(imtcom::IServerConnectionInterface::CF_DEFAULT);
		}
	}

	return true;
}


} // namespace imtserverapp


