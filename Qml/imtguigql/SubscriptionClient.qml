import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0

GqlModel {
    id: container;
    property string subscriptionId;
	property string gqlCommandId;
	property string state;

	signal messageReceived(var data);

    Component.onCompleted: {
        subscriptionId = UuidGenerator.generateUUID();
    }

	Component.onDestruction: {
		unRegisterSubscription();
	}

	onStateChanged: {
		if (container.state === "Ready"){
			console.log("SubscriptionClient.qml onStateChanged", toJson());
			if (container.containsKey("data")){
				let dataModelLocal = container.getData("data")
				container.messageReceived(dataModelLocal);
			}
		}
	}

	function registerSubscription(){
		console.log("SubscriptionClient.qml registerSubscription");

		var query = Gql.GqlRequest("subscription", gqlCommandId);
		var inputParams = Gql.GqlObject("input");
		query.AddParam(inputParams);
		var queryFields = Gql.GqlObject("notification");
		queryFields.InsertField("Id");
		query.AddField(queryFields);

		Events.sendEvent("RegisterSubscription", {"Query": query, "Client": container, "Headers": container.getHeaders()});
	}

	function unRegisterSubscription(){
		Events.sendEvent("UnRegisterSubscription", container);
	}

	function getHeaders(){
		return {};
	}
}
