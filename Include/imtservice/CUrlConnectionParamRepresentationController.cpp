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
	if (urlConnectionParamPtr != nullptr) {
		return true;
	}

	return false;
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
	if (urlConnectionParamPtr == nullptr) {
		return false;
	}

	QUrl defaultUrl  = urlConnectionParamPtr->GetDefaultUrl();
	QByteArray usageId = urlConnectionParamPtr->GetUsageId();
	QByteArray serviceTypeName = urlConnectionParamPtr->GetServiceTypeName();

	representation.SetData("Id", usageId);
	representation.SetData("UsageId", usageId);
	representation.SetData("ServiceTypeName", serviceTypeName);
	representation.SetData("DefaultUrl", defaultUrl.toString());

	QUrl url = urlConnectionParamPtr->GetUrl();
	representation.SetData("Host", url.host());
	representation.SetData("Port", url.port());

	imtbase::CTreeItemModel* externPortsModelPtr = representation.AddTreeModel("ExternPorts");

	QList<imtservice::IServiceConnectionParam::IncomingConnectionParam> incomingConnections = urlConnectionParamPtr->GetIncomingConnections();
	for (const imtservice::IServiceConnectionParam::IncomingConnectionParam& incomingConnection : incomingConnections){
		int externIndex = externPortsModelPtr->InsertNewItem();

		externPortsModelPtr->SetData("Id", incomingConnection.id, externIndex);
		externPortsModelPtr->SetData("Name", incomingConnection.name, externIndex);
		externPortsModelPtr->SetData("Description", incomingConnection.description, externIndex);
		externPortsModelPtr->SetData("Host", incomingConnection.url.host(), externIndex);
		externPortsModelPtr->SetData("Port", incomingConnection.url.port(), externIndex);
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
	if (urlConnectionParamPtr == nullptr) {
		return false;
	}

	urlConnectionParamPtr->SetConnectionType(IServiceConnectionInfo::CT_INPUT);

	if (representation.ContainsKey("UsageId")){
		QByteArray usageId = representation.GetData("UsageId").toByteArray();

		urlConnectionParamPtr->SetUsageId(usageId);
	}

	if (representation.ContainsKey("ServiceTypeName")){
		QByteArray serviceTypeName = representation.GetData("ServiceTypeName").toByteArray();

		urlConnectionParamPtr->SetServiceTypeName(serviceTypeName);
	}

	QUrl url;
	if (representation.ContainsKey("Host")){
		QString host = representation.GetData("Host").toString();

		url.setHost(host);
	}

	if (representation.ContainsKey("Port")){
		int port = representation.GetData("Port").toInt();

		url.setPort(port);
	}

	urlConnectionParamPtr->SetUrl(url);

	if (representation.ContainsKey("ExternPorts")){
		imtbase::CTreeItemModel* externPortsModelPtr = representation.GetTreeItemModel("ExternPorts");
		if (externPortsModelPtr != nullptr){
			for (int i = 0; i < externPortsModelPtr->GetItemsCount(); i++){
				imtservice::IServiceConnectionParam::IncomingConnectionParam incomingConnection;

				if (externPortsModelPtr->ContainsKey("Id", i)){
					QByteArray id = externPortsModelPtr->GetData("Id", i).toByteArray();

					incomingConnection.id = id;
				}

				if (externPortsModelPtr->ContainsKey("Name", i)){
					QString name = externPortsModelPtr->GetData("Name", i).toString();

					incomingConnection.name = name;
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

				incomingConnection.url = externalUrl;

				urlConnectionParamPtr->AddExternConnection(incomingConnection);
			}
		}
	}

	return true;
}


} // namespace imtservice


