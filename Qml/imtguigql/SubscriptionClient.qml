import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0

GqlModel {
	id: container;
	property string subscriptionId;
	property string gqlCommandId;
	property string state;

	signal messageReceived(var data);

	property bool ok: subscriptionId !== "" && gqlCommandId !== "";
	onOkChanged: {
		if (ok){
			registerSubscription();
		}
	}

	Component.onCompleted: {
		subscriptionId = UuidGenerator.generateUUID();
	}

	Component.onDestruction: {
		unRegisterSubscription();
	}

	onStateChanged: {
		if (container.state === "Ready"){
			if (container.containsKey("data")){
				let dataModelLocal = container.getData("data")
				
				if (dataModelLocal.containsKey(gqlCommandId)){
					dataModelLocal = dataModelLocal.getData(gqlCommandId)
				}

				container.messageReceived(dataModelLocal);
			}
		}
	}

	function getGqlQuery(){
		var query = Gql.GqlRequest("subscription", gqlCommandId);
		var inputParams = Gql.GqlObject("input");
		query.AddParam(inputParams);
		var queryFields = Gql.GqlObject("notification");
		queryFields.InsertField("id");
		query.AddField(queryFields);

		return query;
	}

	function registerSubscription(){
		let query = getGqlQuery();

		Events.sendEvent("RegisterSubscription", {"Query": query, "Client": container, "Headers": container.getHeaders()});
	}

	function unRegisterSubscription(){
		Events.sendEvent("UnregisterSubscription", container);
	}

	function getHeaders(){
		return {};
	}
}
