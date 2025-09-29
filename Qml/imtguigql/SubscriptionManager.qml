import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import QtWebSockets 1.2
import imtcontrols 1.0

WebSocket {
	id: container;

	active: true

	property TreeItemModel socketModel: TreeItemModel {}

	property var subscriptionModel: []

	Component.onCompleted: {
		Events.subscribeEvent("RegisterSubscription", container.registerSubscriptionEvent);
		Events.subscribeEvent("UnregisterSubscription", container.unRegisterSubscription);
	}

	Component.onDestruction: {
		Events.unSubscribeEvent("RegisterSubscription", container.registerSubscriptionEvent);
		Events.unSubscribeEvent("UnregisterSubscription", container.unRegisterSubscription);
	}
	
	onUrlChanged: {
		console.log("WebSocket onUrlChanged", url)
	}

	onStatusChanged: {
		if (status == WebSocket.Error){
			timer.restart()
			console.error("SubscriptionManager ERROR", errorString)
		}
		else if (status == WebSocket.Open){
			timer.stop()
			
			sendTextMessage("{ \"type\": \"connection_init\" }")

			registerSubscriptionToServer();
		}
		else if (status == WebSocket.Closed){
			timer.restart()
			
			for (let index = 0; index < subscriptionModel.length; index++){
				subscriptionModel[index]["status"] = "unregistered"
			}
		}
	}

	function reconnect(){
		active = false;
		active = true;
	}

	property Timer timer: Timer{
		interval: 4000;
		onTriggered: {
			container.reconnect()
		}
	}

	onTextMessageReceived:{
		socketModel.createFromJson(message)

		if (socketModel.getData("type") === "connection_ack"){
			registerSubscriptionToServer()
		}
		else if (socketModel.getData("type") === "start_ack"){
			for (let index = 0; index < subscriptionModel.length; index++){
				if (subscriptionModel[index]["subscriptionId"] === socketModel.getData("id")){
					let subscription = subscriptionModel[index]["subscription"]
					subscriptionModel[index]["status"] = "registered"
					subscription.state = "Registered"
				}
			}
		}
		else if (socketModel.getData("type") === "data"){
			for (let index = 0; index < subscriptionModel.length; index++){
				if (subscriptionModel[index]["subscriptionId"] == socketModel.getData("id")){
					let subscription = subscriptionModel[index]["subscription"]
					if (!subscription || subscription === undefined){
						continue;
					}

					subscription.state = "Processing"
					let dataModelLocal = socketModel.getData("payload");

					if (subscription && dataModelLocal){
						subscription.copy(dataModelLocal)
						subscription.state = "Ready"
					}

					return;
				}
			}
		}
	}

	function registerSubscriptionEvent(parameters){
		let query = parameters["Query"];
		let client = parameters["Client"];
		let headers = parameters["Headers"];

		registerSubscription(query, client, headers);
	}

	function clear(){
		container.subscriptionModel = []
	}

	function registerSubscriptionToServer(){
		if (container.status != WebSocket.Open){
			return;
		}

		for (let index = 0; index < subscriptionModel.length; index++){
			if (subscriptionModel[index]["status"] === "unregistered"){
				let request = {}
				request["id"] = subscriptionModel[index]["subscriptionId"]
				request["headers"] = subscriptionModel[index]["headers"]
				request["type"] = "start"
				let payload = {}
				let query = subscriptionModel[index]["query"]
				payload["data"] = query.GetQuery()
				request["payload"] = payload

				subscriptionModel[index]["status"] = "waiting";

				container.sendTextMessage(JSON.stringify(request))
			}
		}
	}

	function registerSubscription(query, subscriptionClient, headers){
		let commandId = query.GetCommandId()
		if (commandId === ""){
			console.error("Unable to register subscription with empty command-ID")
			return;
		}

		for (let index = 0; index < subscriptionModel.length; index++){
			if (subscriptionModel[index]["subscription"] === subscriptionClient){
				return;
			}
		}

		subscriptionModel.push(
					{
						"commandId": commandId,
						"subscriptionId": subscriptionClient.subscriptionId,
						"query": query,
						"status": "unregistered",
						"subscription": subscriptionClient,
						"headers": headers
					}
					)

		registerSubscriptionToServer()
	}

	function unRegisterSubscription(subscriptionClient){
		let index
		for (index = 0; index < subscriptionModel.length; index++){
			if (subscriptionModel[index]["subscription"] === subscriptionClient){
				let request = {}
				request["id"] = subscriptionModel[index]["subscriptionId"]
				request["headers"] = subscriptionModel[index]["headers"]
				request["type"] = "stop"
				let payload = {}
				request["payload"] = payload

				subscriptionModel[index]["status"] = "waiting";
				container.sendTextMessage(JSON.stringify(request))

				subscriptionModel.splice(index, 1)

				return;
			}
		}
	}
}
