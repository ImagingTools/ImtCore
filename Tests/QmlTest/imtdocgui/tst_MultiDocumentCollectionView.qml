import QtQuick 2.12
import QtTest 1.12
import imtgui 1.0
import imtdocgui 1.0
import imtguigql 1.0
import imtqmltest 1.0

TestCase {
	id: testCase

	name: "MultiDocumentCollectionView"
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

		MultiDocumentCollectionView {
			width: 400
			height: 300
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

		return {"service": service, "workspace": workspace, "tabView": findTabView(workspace)}
	}

	function findTabView(item){
		if (item.tabModel !== undefined && item.currentIndex !== undefined){
			return item
		}

		for (let i = 0; i < item.children.length; ++i){
			let found = findTabView(item.children[i])
			if (found){
				return found
			}
		}

		return null
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

	function test_listDoesNotRefocusOpenDocuments(){
		let context = createWorkspace()
		context.service.handleDocumentOpened("doc1", "obj-doc1", typeId, "doc1", false, false)
		context.service.handleDocumentOpened("doc2", "obj-doc2", typeId, "doc2", false, false)
		let tabCount = context.tabView.tabModel.count
		context.tabView.currentIndex = context.tabView.getIndexById("doc1")

		receiveDocumentList(context, [documentInfo("doc1", false), documentInfo("doc2", false)])

		compare(context.tabView.tabModel.count, tabCount, "No tab is added for an open document")
		compare(context.tabView.currentIndex, context.tabView.getIndexById("doc1"), "The selected tab stays selected")
	}

	function test_listCompletesMissedDataLoading(){
		let context = createWorkspace()
		context.service.handleDocumentOpened("doc", "obj-doc", typeId, "doc", false, false)
		verify(context.service.documentIsLoading("doc"), "Loading until DocumentDataLoaded")

		receiveDocumentList(context, [documentInfo("doc", false)])

		verify(!context.service.documentIsLoading("doc"), "The server says it is loaded")
	}

	function test_listKeepsDocumentLoadingOnTheServerLoading(){
		let context = createWorkspace()
		context.service.handleDocumentOpened("doc", "obj-doc", typeId, "doc", false, false)

		receiveDocumentList(context, [documentInfo("doc", true)])

		verify(context.service.documentIsLoading("doc"))
	}

	function test_listOpensDocumentOpenedElsewhere(){
		let context = createWorkspace()
		context.service.handleDocumentOpened("doc", "obj-doc", typeId, "doc", false, false)
		let tabCount = context.tabView.tabModel.count

		receiveDocumentList(context, [documentInfo("doc", false), documentInfo("other", false)])

		verify(context.service.documentIsOpened("other"))
		compare(context.tabView.tabModel.count, tabCount + 1)
		verify(context.tabView.getIndexById("other") >= 0)
	}

	function test_lostDocumentLosesItsTab(){
		let context = createWorkspace()
		context.service.handleDocumentOpened("doc", "obj-doc", typeId, "doc", false, false)
		verify(context.tabView.getIndexById("doc") >= 0)

		context.service.handleDocumentLost("doc")

		verify(!context.service.documentIsOpened("doc"))
		compare(context.tabView.getIndexById("doc"), -1)
	}
}
