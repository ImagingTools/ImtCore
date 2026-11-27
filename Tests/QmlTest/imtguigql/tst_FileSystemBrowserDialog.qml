import QtQuick 2.12
import QtTest 1.12
import QtWebSockets 1.2
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtqmltest 1.0

TestCase {
	id: testCase

	name: "FileSystemBrowserDialog"
	when: windowShown

	// The listings of the server: the dialog opens in "C:/Data", which holds the folder "C:/Data/b"
	readonly property string dataListing: '{"data":{"GetFileSystemEntries":{"path":"C:/Data","parentPath":"C:/","entries":[{"name":"b","path":"C:/Data/b","entryType":"Directory"}],"totalCount":1,"hasMore":false}}}'
	readonly property string subfolderListing: '{"data":{"GetFileSystemEntries":{"path":"C:/Data/b","parentPath":"C:/Data","entries":[],"totalCount":0,"hasMore":false}}}'

	// The application installs the button decorator; the test host has none
	property var previousButtonDecorator: null

	Component {
		id: buttonDecoratorComp

		ButtonDecorator {
		}
	}

	// Plays the server of the subscriptions the way the ImtCore server does: connection_ack, and start_ack after a start.
	// Only the connection of the current test is recorded: a dialog of the previous one may still send its stop.
	WebSocketServer {
		id: server

		property var connection: null
		property var starts: []
		property var stoppedIds: []

		listen: true
		port: 0

		onClientConnected: function(webSocket){
			server.connection = webSocket
			webSocket.onTextMessageReceived.connect(function(message){
				if (webSocket !== server.connection){
					return
				}

				let request = JSON.parse(message)
				if (request.type === "connection_init"){
					webSocket.sendTextMessage(JSON.stringify({"type": "connection_ack"}))
				}
				else if (request.type === "start"){
					let starts = server.starts
					starts.push(request)
					server.starts = starts
					webSocket.sendTextMessage(JSON.stringify({"type": "start_ack", "id": request.id}))
				}
				else if (request.type === "stop"){
					let stopped = server.stoppedIds
					stopped.push(request.id)
					server.stoppedIds = stopped
				}
			})
		}

		function sendChange(subscriptionId, path){
			server.connection.sendTextMessage(JSON.stringify({"type": "data", "id": subscriptionId,
				"payload": {"data": {"OnFileSystemChanged": {"path": path}}}}))
		}
	}

	Component {
		id: managerComp

		SubscriptionManager {
		}
	}

	Component {
		id: dialogViewComp

		DialogManagerView {
			width: 1000
			height: 800
		}
	}

	Component {
		id: dialogComp

		FileSystemBrowserDialog {
		}
	}

	function initTestCase(){
		previousButtonDecorator = Style.buttonDecorator
		Style.buttonDecorator = buttonDecoratorComp
	}

	function cleanupTestCase(){
		Style.buttonDecorator = previousButtonDecorator
	}

	function init(){
		server.connection = null
		server.starts = []
		server.stoppedIds = []
		GqlTestTransport.reset()
		GqlTestTransport.responseBody = testCase.dataListing
	}

	// A failed test must not leave its dialog to the next one
	function cleanup(){
		while (ModalDialogManager.count > 0){
			ModalDialogManager.closeDialog()
		}
	}

	function findItem(item, check){
		if (check(item)){
			return item
		}

		for (let i = 0; i < item.children.length; ++i){
			let found = findItem(item.children[i], check)
			if (found){
				return found
			}
		}

		return null
	}

	// The dialog shows the listing of "C:/Data"; with a connected subscription manager
	function openDialog(watchChanges){
		let manager = createTemporaryObject(managerComp, testCase, {"url": server.url})
		tryCompare(manager, "status", WebSocket.Open, 5000)

		let dialogView = createTemporaryObject(dialogViewComp, testCase.parent)
		ModalDialogManager.openDialog(dialogComp, {"startPath": "C:/Data/a.txt", "watchChanges": watchChanges})

		let dialog = null
		tryVerify(function(){
			dialog = findItem(dialogView, function(item){ return item.watchChanges !== undefined && item.browse !== undefined })
			return dialog !== null
		}, 3000, "The dialog opens")

		tryCompare(dialog, "currentPath", "C:/Data", 5000)
		tryCompare(dialog, "isLoading", false, 5000)

		return dialog
	}

	// The subscription of a start message, parsed by the server-side parser
	function parseStart(start){
		let request = GqlTestTransport.parseRequest(start.payload.data)
		verify(request.parsed, "server-side parser rejected: " + start.payload.data)

		return request
	}

	function lastListedPath(){
		let request = GqlTestTransport.parseLastRequest()
		verify(request.parsed, "server-side parser rejected: " + GqlTestTransport.lastRequestBody)
		compare(request.commandId, "GetFileSystemEntries")

		return request.params.input.path
	}

	// The open folder is watched, and a change of it lists it again
	function test_changeListsTheFolderAgain(){
		let dialog = openDialog(true)

		tryVerify(function(){ return server.starts.length === 1 }, 5000)
		let subscription = parseStart(server.starts[0])
		compare(subscription.requestType, "subscription")
		compare(subscription.commandId, "OnFileSystemChanged")
		compare(subscription.params.input.path, "C:/Data")
		compare(GqlTestTransport.requestCount, 1)

		server.sendChange(server.starts[0].id, "C:/Data")

		tryCompare(GqlTestTransport, "requestCount", 2, 5000)
		compare(lastListedPath(), "C:/Data")
		tryCompare(dialog, "isLoading", false, 5000)
		compare(dialog.currentPath, "C:/Data")
	}

	// The subscription follows the navigation: the old one is stopped, the new folder is watched
	function test_navigationMovesTheSubscription(){
		let dialog = openDialog(true)
		tryVerify(function(){ return server.starts.length === 1 }, 5000)
		let firstSubscriptionId = server.starts[0].id

		GqlTestTransport.queueResponse("GetFileSystemEntries", testCase.subfolderListing)
		dialog.browse("C:/Data/b", false)
		tryCompare(dialog, "currentPath", "C:/Data/b", 5000)

		tryVerify(function(){ return server.starts.length === 2 }, 5000)
		compare(server.stoppedIds, [firstSubscriptionId])
		compare(parseStart(server.starts[1]).params.input.path, "C:/Data/b")

		// A reload of the same folder keeps its subscription
		GqlTestTransport.queueResponse("GetFileSystemEntries", testCase.subfolderListing)
		dialog.browse("C:/Data/b", false)
		tryCompare(GqlTestTransport, "requestCount", 3, 5000)
		tryCompare(dialog, "isLoading", false, 5000)
		wait(200)
		compare(server.starts.length, 2)

		GqlTestTransport.queueResponse("GetFileSystemEntries", testCase.subfolderListing)
		server.sendChange(server.starts[1].id, "C:/Data/b")
		tryCompare(GqlTestTransport, "requestCount", 4, 5000)
		compare(lastListedPath(), "C:/Data/b")
	}

	// A change during a listing lists the folder once more after it
	function test_changeDuringAListingListsOnceMore(){
		let dialog = openDialog(true)
		tryVerify(function(){ return server.starts.length === 1 }, 5000)

		GqlTestTransport.queueDelay("GetFileSystemEntries", 500)
		dialog.browse("C:/Data", false)
		verify(dialog.isLoading)
		server.sendChange(server.starts[0].id, "C:/Data")
		server.sendChange(server.starts[0].id, "C:/Data")

		tryCompare(GqlTestTransport, "requestCount", 3, 5000)
		tryCompare(dialog, "isLoading", false, 5000)
		wait(300)
		compare(GqlTestTransport.requestCount, 3)
	}

	// Without the option nothing is watched
	function test_noSubscriptionWithoutTheOption(){
		let dialog = openDialog(false)

		wait(300)
		compare(server.starts.length, 0)
		compare(dialog.currentPath, "C:/Data")
	}

	// Closing the dialog stops its subscription
	function test_closedDialogStopsItsSubscription(){
		openDialog(true)
		tryVerify(function(){ return server.starts.length === 1 }, 5000)

		ModalDialogManager.closeDialog()

		tryVerify(function(){ return server.stoppedIds.length === 1 }, 5000)
		compare(server.stoppedIds[0], server.starts[0].id)
	}
}
