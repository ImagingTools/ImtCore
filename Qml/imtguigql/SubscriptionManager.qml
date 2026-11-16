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

	property GqlModel __tokenHelper: GqlModel {}
	// Auth HTTP header name (QML-side constant; keep equal to server header id).
	readonly property string authenticationTokenHeaderId: "x-authentication-token"

	Component.onCompleted: {
		Events.subscribeEvent("RegisterSubscription", container.registerSubscriptionEvent);
		Events.subscribeEvent("UnregisterSubscription", container.unRegisterSubscription);
		Events.subscribeEvent("AccessTokenRefreshed", container.onAccessTokenRefreshed);

		// Notify any SubscriptionClient instances that were created before this
		// manager.  Their initial RegisterSubscription events were lost because
		// this handler had not yet subscribed.  The clients listen for this
		// event and re-register themselves.
		Events.sendEvent("SubscriptionManagerReady", {});
	}

	Component.onDestruction: {
		Events.unSubscribeEvent("RegisterSubscription", container.registerSubscriptionEvent);
		Events.unSubscribeEvent("UnregisterSubscription", container.unRegisterSubscription);
		Events.unSubscribeEvent("AccessTokenRefreshed", container.onAccessTokenRefreshed);
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
				container.markInterrupted(index)
				subscriptionModel[index]["status"] = "unregistered"
			}
		}
	}

	function reconnect(){
		active = false;
		active = true;
	}

	// The server answers a start with silence, so a sent one is live; its notifications are lost until it is sent again.
	function markInterrupted(index){
		let status = subscriptionModel[index]["status"]
		if (status === "waiting" || status === "registered"){
			subscriptionModel[index]["interrupted"] = true
		}
	}

	property Timer timer: Timer{
		interval: 4000;
		onTriggered: {
			container.reconnect()
		}
	}

	// A subscription the server could not authorize for now is sent again after this pause, doubled up to the maximum
	property int authorizationRetryInterval: 5000
	property int authorizationRetryMaxInterval: 60000
	property int authorizationRetryDelay: authorizationRetryInterval

	property Timer authorizationRetryTimer: Timer{
		interval: container.authorizationRetryDelay
		onTriggered: {
			for (let index = 0; index < container.subscriptionModel.length; index++){
				if (container.subscriptionModel[index]["status"] === "retrying"){
					container.subscriptionModel[index]["status"] = "unregistered"
				}
			}

			container.authorizationRetryDelay = Math.min(container.authorizationRetryDelay * 2, container.authorizationRetryMaxInterval)
			container.registerSubscriptionToServer()
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
					container.authorizationRetryDelay = container.authorizationRetryInterval
					if (container.subscriptionModel[index]["interrupted"]){
						container.subscriptionModel[index]["interrupted"] = false
						if (subscription && subscription.markStale !== undefined){
							subscription.markStale()
						}
					}
				}
			}
		}
		else if (socketModel.getData("type") === "error"){
			container.handleSubscriptionError(socketModel.getData("id"), message)
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
						// Deliver synchronously, before the next incoming message overwrites
						// this subscription's model. Relying on the (async) onStateChanged
						// signal coalesced back-to-back messages so only the last one of a
						// burst surfaced - see SubscriptionClient.deliverReady().
						if (subscription.deliverReady !== undefined){
							subscription.deliverReady()
						}
					}

					return;
				}
			}
		}
	}

	// The machine code of an error frame of the server; empty for a server that sends none
	function getErrorCode(rawMessage){
		let frame = JSON.parse(rawMessage)
		let errors = frame["payload"]
		if (!errors || errors.length === 0 || !errors[0]["extensions"]){
			return ""
		}

		let code = errors[0]["extensions"]["code"]

		return code ? code : ""
	}

	function handleSubscriptionError(subscriptionId, rawMessage) {
		console.warn("SubscriptionManager: error for id", subscriptionId, rawMessage)

		// The code decides: an authorization unavailable for now is asked again later, changed permissions at once.
		// Not "unregistered" for a refusal: that one is re-sent with every new registration; a refused
		// subscription is re-submitted only by AccessTokenRefreshed or a reconnect.
		let code = container.getErrorCode(rawMessage)
		let isRegistrationNeeded = false
		for (let index = 0; index < subscriptionModel.length; index++){
			if (subscriptionModel[index]["subscriptionId"] == subscriptionId){
				container.markInterrupted(index)
				let subscription = subscriptionModel[index]["subscription"]
				if (code === "AuthorizationUnavailable"){
					subscriptionModel[index]["status"] = "retrying"
					container.authorizationRetryTimer.start()
				}
				else if (code === "PermissionsChanged"){
					subscriptionModel[index]["status"] = "unregistered"
					isRegistrationNeeded = true
				}
				else{
					subscriptionModel[index]["status"] = "rejected"
				}

				if (subscription){
					subscription.state = "Error"
				}
			}
		}

		if (isRegistrationNeeded){
			container.registerSubscriptionToServer()
		}
	}

	function onAccessTokenRefreshed(parameters) {
		for (let index = 0; index < container.subscriptionModel.length; index++){
			// The "stop" is sent first, because server side controllers append a new
			// registration for a repeated "start" instead of replacing the old one.
			// The socket keeps the message order, so the server drops the previous
			// registration before the new one arrives.
			container.unregisterSubscriptionOnServer(index)
			container.subscriptionModel[index]["status"] = "unregistered"
		}

		registerSubscriptionToServer()
	}

	function unregisterSubscriptionOnServer(index){
		if (container.status != WebSocket.Open){
			return;
		}

		let request = {}
		request["id"] = container.subscriptionModel[index]["subscriptionId"]
		request["headers"] = container.subscriptionModel[index]["headers"]
		request["type"] = "stop"
		let payload = {}
		request["payload"] = payload

		container.sendTextMessage(JSON.stringify(request))
	}

	function registerSubscriptionEvent(parameters){
		let query = parameters["Query"];
		let client = parameters["Client"];
		let headers = parameters["Headers"];

		registerSubscription(query, client, headers);
	}

	function clear(){
		container.subscriptionModel = []
		// Auto-subscribe clients (e.g. PumaWsConnection) were wiped with the model;
		// ask them to re-register so connection status keeps working after logout.
		Events.sendEvent("SubscriptionManagerReady", {});
	}

	function registerSubscriptionToServer(){
		if (container.status != WebSocket.Open){
			return;
		}

		for (let index = 0; index < subscriptionModel.length; index++){
			if (subscriptionModel[index]["status"] === "unregistered"){
				let request = {}
				request["id"] = subscriptionModel[index]["subscriptionId"]

				let headers = subscriptionModel[index]["headers"]
				if (!headers){
					headers = {}
				}

				// Keep auth header in sync with the current global token.
				// - If a token is present: always overwrite (stale JWT caused refresh loops).
				// - If empty: remove header so pre-login / public subs (e.g. PumaWsConnection)
				//   can still register with an anonymous server context.
				// Do NOT block registration when token is empty.
				let accessToken = container.__tokenHelper.GetGlobalAccessToken()
				if (accessToken && accessToken !== ""){
					headers[container.authenticationTokenHeaderId] = accessToken
				}
				else if (headers[container.authenticationTokenHeaderId] !== undefined){
					delete headers[container.authenticationTokenHeaderId]
				}

				let productId = container.__tokenHelper.GetProductId()
				if (productId){
					headers["productId"] = productId
				}

				subscriptionModel[index]["headers"] = headers

				request["headers"] = headers
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

		if (!subscriptionClient || subscriptionClient.subscriptionId === ""){
			console.error("Unable to register subscription with empty subscription-ID. Command-ID:", commandId)
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
