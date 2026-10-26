import QtQuick 2.12
import QtTest 1.12
import imtgui 1.0
import imtdocgui 1.0
import imtguigql 1.0
import imtcolgui 1.0
import imtqmltest 1.0

TestCase {
	id: testCase

	name: "SingleDocumentCollectionView"
	when: windowShown

	property string typeId: "TestType"
	property string viewTypeId: "TestView"

	Component {
		id: serviceComp

		GqlBasedCollectionDocumentService {
			closeDocumentOverride: function(documentId){
				handleCloseDocumentResult(documentId, "Success")
			}
		}
	}

	Component {
		id: workspaceComp

		SingleDocumentCollectionView {
			width: 400
			height: 300
		}
	}

	Component {
		id: collectionComp

		CollectionView {
		}
	}

	Component {
		id: viewComp

		ViewBase {
			width: 10
			height: 10
		}
	}

	Component {
		id: controllerComp

		DocumentRepresentationController {
			representationModel: QtObject {
				signal modelChanged(var changeSet)
			}

			function updateRepresentationFromDocument(){
				startUpdateRepresentation(documentId, representationModel)
				representationUpdated(documentId, representationModel)
			}

			function updateDocumentFromRepresentation(){
			}
		}
	}

	function init(){
		GqlTestTransport.reset()
	}

	function createWorkspace(){
		let service = createTemporaryObject(serviceComp, testCase)
		service.registerDocumentViewData(typeId, viewTypeId, viewComp, controllerComp)

		// TestCase itself is invisible, the workspace needs a visible parent
		let workspace = createTemporaryObject(workspaceComp, testCase.parent, {"documentManager": service})
		workspace.setCollectionViewComp("Collection", collectionComp)

		return {"service": service, "workspace": workspace}
	}

	// Loading overlays are the items with start/stop and a caption
	function visibleLoadingCount(item){
		let count = 0
		if (item.start !== undefined && item.stop !== undefined && item.captionVisible !== undefined && item.visible){
			count++
		}

		for (let i = 0; i < item.children.length; ++i){
			count += visibleLoadingCount(item.children[i])
		}

		return count
	}

	function documentInfo(documentId, isLoading){
		return {"documentId": documentId, "documentName": documentId, "objectTypeId": typeId, "objectId": "obj-" + documentId,
			"isDirty": false, "hasNameProvider": false, "isLoading": isLoading}
	}

	function receiveDocumentList(context, documents){
		GqlTestTransport.queueResponse("GetOpenedDocumentList", JSON.stringify({"data": {"GetOpenedDocumentList": {"documentList": documents}}}))
		let requestsBefore = GqlTestTransport.requestCount

		context.service.getOpenedDocumentList()

		tryVerify(function(){ return GqlTestTransport.requestCount > requestsBefore }, 5000)
		// the reply is delivered on the next event loop turns
		wait(100)
	}

	// A DocumentDataLoaded lost while resubscribing, e.g. on a tenant switch, must not leave the overlay on
	function test_listCompletesMissedDataLoading(){
		let context = createWorkspace()
		context.service.handleDocumentOpened("doc", "obj-doc", typeId, "doc", false, false)
		verify(context.service.documentIsLoading("doc"), "Loading until DocumentDataLoaded")
		tryVerify(function(){ return visibleLoadingCount(context.workspace) > 0 }, 3000, "The overlay is shown")

		receiveDocumentList(context, [documentInfo("doc", false)])

		verify(!context.service.documentIsLoading("doc"), "The server says it is loaded")
		compare(visibleLoadingCount(context.workspace), 0, "No overlay stays")
		compare(GqlTestTransport.requestedCommands().filter(function(command){ return command === "GetOpenedDocumentList" }).length, 1)
	}

	function test_listKeepsDocumentLoadingOnTheServerLoading(){
		let context = createWorkspace()
		context.service.handleDocumentOpened("doc", "obj-doc", typeId, "doc", false, false)

		receiveDocumentList(context, [documentInfo("doc", true)])

		verify(context.service.documentIsLoading("doc"))
		verify(visibleLoadingCount(context.workspace) > 0)
	}

	function openAnswer(isLoading){
		return JSON.stringify({"data": {"OpenDocument": {"documentId": "doc", "documentName": "doc", "hasNameProvider": false,
			"isDirty": false, "isLoading": isLoading, "objectId": "obj-doc", "objectTypeId": typeId}}})
	}

	function listRequestCount(){
		return GqlTestTransport.requestedCommands().filter(function(command){ return command === "GetOpenedDocumentList" }).length
	}

	function openFromServer(context, isLoading){
		GqlTestTransport.queueResponse("OpenDocument", openAnswer(isLoading))
		context.service.openDocument(typeId, "obj-doc")
		tryVerify(function(){ return context.service.documentIsOpened("doc") }, 5000, "The document is opened")
		wait(100)
	}

	// The server already has the data: nothing waits for a DocumentDataLoaded that may never come
	function test_openAnswerWithLoadedDataEndsLoading(){
		let context = createWorkspace()

		openFromServer(context, false)

		verify(!context.service.documentIsLoading("doc"))
		compare(visibleLoadingCount(context.workspace), 0)
		verify(!context.service.loadingResyncTimer.running, "No resync is planned")
	}

	// A DocumentDataLoaded lost on the way, e.g. a relay subscription refused with an expired token
	function test_lostDataLoadedIsResyncedOnce(){
		let context = createWorkspace()
		context.service.loadingResyncTimer.interval = 200
		let listsBefore = listRequestCount()

		openFromServer(context, true)
		verify(context.service.documentIsLoading("doc"), "The server is still loading")

		GqlTestTransport.queueResponse("GetOpenedDocumentList", JSON.stringify({"data": {"GetOpenedDocumentList": {"documentList": [documentInfo("doc", false)]}}}))
		tryVerify(function(){ return !context.service.documentIsLoading("doc") }, 3000, "The opened document list ends the loading")
		compare(visibleLoadingCount(context.workspace), 0)

		wait(500)
		compare(listRequestCount() - listsBefore, 1, "One request")
	}

	function test_dataLoadedInTimeSendsNoResync(){
		let context = createWorkspace()
		context.service.loadingResyncTimer.interval = 200
		let listsBefore = listRequestCount()

		openFromServer(context, true)
		context.service.setDocumentIsLoading("doc", false)
		wait(500)

		compare(listRequestCount(), listsBefore)
	}
}