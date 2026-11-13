import QtQuick 2.12
import QtTest 1.12
import QtWebSockets 1.2
import imtguigql 1.0

TestCase {
	id: testCase

	name: "SubscriptionManager"

	// Plays the server side the way the ImtCore server does: connection_ack, and silence after a start.
	WebSocketServer {
		id: server

		property var connection: null
		property var receivedTypes: []

		listen: true
		port: 0

		onClientConnected: {
			server.connection = webSocket
			webSocket.onTextMessageReceived.connect(function(message){
				let request = JSON.parse(message)
				let types = server.receivedTypes
				types.push(request.type)
				server.receivedTypes = types

				if (request.type === "connection_init"){
					webSocket.sendTextMessage(JSON.stringify({"type": "connection_ack"}))
				}
			})
		}

		function startCount(){
			let count = 0
			for (let i = 0; i < server.receivedTypes.length; ++i){
				if (server.receivedTypes[i] === "start"){
					count++
				}
			}

			return count
		}
	}

	Component {
		id: managerComp

		SubscriptionManager {
		}
	}

	Component {
		id: clientComp

		SubscriptionClient {
			property int refreshCount: 0
			property int messageCount: 0

			autoSubscribe: false
			gqlCommandId: "OnTestChanged"

			onMessageReceived: {
				messageCount++
			}

			onRefreshRequired: {
				refreshCount++
			}
		}
	}

	function init(){
		server.connection = null
		server.receivedTypes = []
	}

	// A connected manager with one subscription the server has received.
	function connect(){
		let manager = createTemporaryObject(managerComp, testCase, {"url": server.url})
		let client = createTemporaryObject(clientComp, testCase)
		client.registerSubscription()

		tryCompare(manager, "status", WebSocket.Open, 5000)
		tryVerify(function(){ return server.startCount() === 1 }, 5000, "The subscription is started")

		return {"manager": manager, "client": client}
	}

	function dropConnection(context){
		server.connection.active = false
		tryCompare(context.manager, "status", WebSocket.Closed, 5000)
	}

	function sendNotification(context){
		server.connection.sendTextMessage(JSON.stringify({"type": "data", "id": context.client.subscriptionId,
			"payload": {"data": {"OnTestChanged": {"id": "item"}}}}))
	}

	function test_messagesReachAVisibleClient(){
		let context = connect()

		sendNotification(context)

		tryCompare(context.client, "messageCount", 1, 5000)
		compare(context.client.refreshCount, 0)
	}

	function test_hiddenClientGetsOneRefreshWhenShown(){
		let context = connect()
		context.client.deliveryEnabled = false

		sendNotification(context)
		sendNotification(context)
		wait(200)
		compare(context.client.messageCount, 0, "Nothing is delivered while hidden")
		compare(context.client.refreshCount, 0)

		context.client.deliveryEnabled = true

		compare(context.client.refreshCount, 1, "One refresh replaces the dropped messages")
		compare(context.client.messageCount, 0)

		context.client.deliveryEnabled = false
		context.client.deliveryEnabled = true
		compare(context.client.refreshCount, 1, "Nothing was missed the second time")
	}

	function test_reconnectWhileHiddenRefreshesWhenShown(){
		let context = connect()
		context.client.deliveryEnabled = false

		dropConnection(context)
		tryVerify(function(){ return server.startCount() === 2 }, 10000)
		compare(context.client.refreshCount, 0, "Not while hidden")

		context.client.deliveryEnabled = true
		compare(context.client.refreshCount, 1)
	}

	function test_firstRegistrationIsNotAResume(){
		let context = connect()

		wait(100)
		compare(context.client.refreshCount, 0)
	}

	function test_reconnectResumesTheSubscription(){
		let context = connect()

		dropConnection(context)
		compare(context.client.refreshCount, 0, "Not while the connection is down")

		// the manager reconnects on its own after its retry interval
		tryCompare(context.manager, "status", WebSocket.Open, 10000)
		tryVerify(function(){ return server.startCount() === 2 }, 5000, "Started again on the new connection")
		compare(context.client.refreshCount, 1)
	}

	function test_everyReconnectResumes(){
		let context = connect()

		dropConnection(context)
		tryVerify(function(){ return server.startCount() === 2 }, 10000)
		dropConnection(context)
		tryVerify(function(){ return server.startCount() === 3 }, 10000)

		compare(context.client.refreshCount, 2)
	}

	function test_errorThenRegistrationIsAResume(){
		let context = connect()

		server.connection.sendTextMessage(JSON.stringify({"type": "error", "id": context.client.subscriptionId}))
		tryCompare(context.client, "state", "Error", 5000)

		// a refreshed token starts the failed subscription again
		context.manager.onAccessTokenRefreshed({})

		tryVerify(function(){ return server.startCount() === 2 }, 5000)
		compare(context.client.refreshCount, 1)
	}

	function test_tokenRefreshIsNotAResume(){
		let context = connect()

		// stop + start on a live connection: nothing is missed in between
		context.manager.onAccessTokenRefreshed({})

		tryVerify(function(){ return server.startCount() === 2 }, 5000)
		compare(context.client.refreshCount, 0)
	}

	function test_subscriptionRegisteredWhileDisconnectedIsNotAResume(){
		let manager = createTemporaryObject(managerComp, testCase, {"url": server.url, "active": false})
		let client = createTemporaryObject(clientComp, testCase)
		client.registerSubscription()

		manager.active = true

		tryVerify(function(){ return server.startCount() === 1 }, 5000)
		compare(client.refreshCount, 0, "It was never live before, so nothing was missed")
	}
}
