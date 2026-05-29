// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CUrlConnectionParamRepresentationController.h>

// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>


namespace imtserverapp
{


QByteArray CUrlConnectionParamRepresentationController::GetModelId() const
{
	return QByteArray();
}


bool CUrlConnectionParamRepresentationController::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtservice::CUrlConnectionParam* urlConnectionParamPtr = dynamic_cast<const imtservice::CUrlConnectionParam*>(&dataModel);

	return urlConnectionParamPtr != nullptr;
}


bool CUrlConnectionParamRepresentationController::GetRepresentationFromDataModel(
			const istd::IChangeable& dataModel,
			QJsonObject& representation,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	const imtservice::CUrlConnectionParam* urlConnectionParamPtr = dynamic_cast<const imtservice::CUrlConnectionParam*>(&dataModel);
	if (urlConnectionParamPtr == nullptr){
		return false;
	}

	const imtcom::IServerConnectionInterface& defaultInterface  = urlConnectionParamPtr->GetDefaultInterface();
	QByteArray serviceTypeId = urlConnectionParamPtr->GetServiceTypeId();

	representation.insert(QStringLiteral("ServiceTypeName"), QString::fromUtf8(serviceTypeId));

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

				representation.insert(QStringLiteral("DefaultHttpUrl"), url.toString());
			}
			break;
			case imtcom::IServerConnectionInterface::PT_WEBSOCKET:
			{
				QUrl url;
				bool success = defaultInterface.GetUrl(imtcom::IServerConnectionInterface::PT_WEBSOCKET, url);
				if (!success){
					return false;
				}

				representation.insert(QStringLiteral("DefaultWsUrl"), url.toString());
			}
			break;
		}
	}

	imtcom::IServerConnectionInterface::ProtocolTypes protocols = urlConnectionParamPtr->GetSupportedProtocols();
	representation.insert(QStringLiteral("Host"), defaultInterface.GetHost());

	for (imtcom::IServerConnectionInterface::ProtocolType protocolType : protocols){
		switch (protocolType){
		case imtcom::IServerConnectionInterface::PT_HTTP:
			representation.insert(QStringLiteral("HttpPort"), defaultInterface.GetPort(imtcom::IServerConnectionInterface::PT_HTTP));
			break;
		case imtcom::IServerConnectionInterface::PT_WEBSOCKET:
			representation.insert(QStringLiteral("WsPort"), defaultInterface.GetPort(imtcom::IServerConnectionInterface::PT_WEBSOCKET));
			break;
		}
	}

	QJsonArray externPortsArray;
	QList<imtservice::IServiceConnectionParam::IncomingConnectionParam> incomingConnections = urlConnectionParamPtr->GetIncomingConnections();
	for (const imtservice::IServiceConnectionParam::IncomingConnectionParam& incomingConnection : incomingConnections){
		QJsonObject itemObj;
		itemObj.insert(QStringLiteral("Id"), QString::fromUtf8(incomingConnection.GetObjectUuid()));
		externPortsArray.append(itemObj);
	}
	representation.insert(QStringLiteral("ExternPorts"), externPortsArray);

	return true;
}


bool CUrlConnectionParamRepresentationController::GetDataModelFromRepresentation(
			const QJsonObject& representation,
			istd::IChangeable& dataModel) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	imtservice::CUrlConnectionParam* urlConnectionParamPtr = dynamic_cast<imtservice::CUrlConnectionParam*>(&dataModel);
	if (urlConnectionParamPtr == nullptr){
		return false;
	}

	urlConnectionParamPtr->SetConnectionType(imtservice::IServiceConnectionInfo::CT_INPUT);

	if (representation.contains(QStringLiteral("Host"))){
		urlConnectionParamPtr->SetHost(representation.value(QStringLiteral("Host")).toString());
	}

	if (representation.contains(QStringLiteral("WsPort"))){
		urlConnectionParamPtr->SetPort(imtcom::IServerConnectionInterface::PT_WEBSOCKET, representation.value(QStringLiteral("WsPort")).toInt());
	}

	if (representation.contains(QStringLiteral("HttpPort"))){
		urlConnectionParamPtr->SetPort(imtcom::IServerConnectionInterface::PT_HTTP, representation.value(QStringLiteral("HttpPort")).toInt());
	}

	if (representation.contains(QStringLiteral("ExternPorts")) && representation.value(QStringLiteral("ExternPorts")).isArray()){
		QJsonArray externPortsArray = representation.value(QStringLiteral("ExternPorts")).toArray();
		for (const QJsonValue& value : externPortsArray){
			QJsonObject itemObj = value.toObject();
			imtservice::IServiceConnectionParam::IncomingConnectionParam incomingConnection;

			if (itemObj.contains(QStringLiteral("Id"))){
				incomingConnection.SetObjectUuid(itemObj.value(QStringLiteral("Id")).toVariant().toByteArray());
			}

			urlConnectionParamPtr->AddExternConnection(incomingConnection);
		}
	}

	return true;
}


} // namespace imtserverapp
