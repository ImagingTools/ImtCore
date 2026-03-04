// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservice/CUrlConnectionParamRepresentationController.h>


namespace imtservice
{


QByteArray CUrlConnectionParamRepresentationController::GetModelId() const
{
	return QByteArray();
}


bool CUrlConnectionParamRepresentationController::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const CUrlConnectionParam* urlConnectionParamPtr = dynamic_cast<const CUrlConnectionParam*>(&dataModel);

	return urlConnectionParamPtr != nullptr;
}


bool CUrlConnectionParamRepresentationController::GetRepresentationFromDataModel(
			const istd::IChangeable& dataModel,
			imtbase::CTreeItemModel& representation,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	const CUrlConnectionParam* urlConnectionParamPtr = dynamic_cast<const CUrlConnectionParam*>(&dataModel);
	if (urlConnectionParamPtr == nullptr){
		return false;
	}

	const imtcom::IServerConnectionInterface& defaultInterface  = urlConnectionParamPtr->GetDefaultInterface();
	QByteArray serviceTypeId = urlConnectionParamPtr->GetServiceTypeId();

	representation.SetData("ServiceTypeName", serviceTypeId);

	imtcom::IServerConnectionInterface::ProtocolTypes defaultProtocols = defaultInterface.GetSupportedProtocols();

	for (imtcom::IServerConnectionInterface::ProtocolType protocolType : defaultProtocols){
		switch (protocolType){
		case imtcom::IServerConnectionInterface::PT_HTTP:
			{
				QUrl url;
				bool success = defaultInterface.GetUrl(imtcom::IServerConnectionInterface::PT_HTTP, url);
				if (!success){
					return false;
				}

				representation.SetData("DefaultHttpUrl", url.toString());
			}
			break;
			case imtcom::IServerConnectionInterface::PT_WEBSOCKET:
			{
				QUrl url;
				bool success = defaultInterface.GetUrl(imtcom::IServerConnectionInterface::PT_WEBSOCKET, url);
				if (!success){
					return false;
				}

				representation.SetData("DefaultWsUrl", url.toString());
			}
			break;
		}
	}

	imtcom::IServerConnectionInterface::ProtocolTypes protocols = urlConnectionParamPtr->GetSupportedProtocols();
	representation.SetData("Host", defaultInterface.GetHost());

	for (imtcom::IServerConnectionInterface::ProtocolType protocolType : protocols){
		switch (protocolType){
		case imtcom::IServerConnectionInterface::PT_HTTP:
			representation.SetData("HttpPort", defaultInterface.GetPort(imtcom::IServerConnectionInterface::PT_HTTP));
			break;
		case imtcom::IServerConnectionInterface::PT_WEBSOCKET:
			representation.SetData("WsPort", defaultInterface.GetPort(imtcom::IServerConnectionInterface::PT_WEBSOCKET));
			break;
		}
	}

	imtbase::CTreeItemModel* externPortsModelPtr = representation.AddTreeModel("ExternPorts");

	QList<IServiceConnectionParam::IncomingConnectionParam> incomingConnections = urlConnectionParamPtr->GetIncomingConnections();
	for (const IServiceConnectionParam::IncomingConnectionParam& incomingConnection : incomingConnections){
		int externIndex = externPortsModelPtr->InsertNewItem();

		externPortsModelPtr->SetData("Id", incomingConnection.id, externIndex);
		externPortsModelPtr->SetData("Description", incomingConnection.description, externIndex);
	}

	return true;
}


bool CUrlConnectionParamRepresentationController::GetDataModelFromRepresentation(
			const imtbase::CTreeItemModel& representation,
			istd::IChangeable& dataModel) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	CUrlConnectionParam* urlConnectionParamPtr = dynamic_cast<CUrlConnectionParam*>(&dataModel);
	if (urlConnectionParamPtr == nullptr){
		return false;
	}

	urlConnectionParamPtr->SetConnectionType(IServiceConnectionInfo::CT_INPUT);

	if (representation.ContainsKey("Host")){
		QString host = representation.GetData("Host").toString();

		urlConnectionParamPtr->SetHost(host);
	}

	if (representation.ContainsKey("WsPort")){
		int port = representation.GetData("WsPort").toInt();

		urlConnectionParamPtr->SetPort(imtcom::IServerConnectionInterface::PT_WEBSOCKET, port);
	}

	if (representation.ContainsKey("HttpPort")){
		int port = representation.GetData("HttpPort").toInt();

		urlConnectionParamPtr->SetPort(imtcom::IServerConnectionInterface::PT_HTTP, port);
	}

	if (representation.ContainsKey("ExternPorts")){
		imtbase::CTreeItemModel* externPortsModelPtr = representation.GetTreeItemModel("ExternPorts");
		if (externPortsModelPtr != nullptr){
			for (int i = 0; i < externPortsModelPtr->GetItemsCount(); i++){
				IServiceConnectionParam::IncomingConnectionParam incomingConnection;

				if (externPortsModelPtr->ContainsKey("Id", i)){
					QByteArray id = externPortsModelPtr->GetData("Id", i).toByteArray();

					incomingConnection.id = id;
				}

				if (externPortsModelPtr->ContainsKey("Name", i)){
					QString name = externPortsModelPtr->GetData("Name", i).toString();
				}

				if (externPortsModelPtr->ContainsKey("Description", i)){
					QString description = externPortsModelPtr->GetData("Description", i).toString();

					incomingConnection.description = description;
				}

				QUrl externalUrl;
				if (externPortsModelPtr->ContainsKey("Host", i)){
					QString host = externPortsModelPtr->GetData("Host", i).toString();

					externalUrl.setHost(host);
				}

				if (externPortsModelPtr->ContainsKey("Port", i)){
					int port = externPortsModelPtr->GetData("Port", i).toInt();

					externalUrl.setPort(port);
				}

				if (externPortsModelPtr->ContainsKey("Scheme", i)){
					QString scheme = externPortsModelPtr->GetData("Scheme", i).toString();

					externalUrl.setScheme(scheme);
				}

				// incomingConnection.url = externalUrl;

				urlConnectionParamPtr->AddExternConnection(incomingConnection);
			}
		}
	}

	return true;
}


} // namespace imtservice


