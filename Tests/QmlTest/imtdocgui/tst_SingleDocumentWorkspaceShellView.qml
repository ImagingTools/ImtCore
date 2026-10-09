import QtQuick 2.12
import QtTest 1.12
import imtgui 1.0
import imtdocgui 1.0

TestCase {
	id: testCase

	name: "SingleDocumentWorkspaceShellView"
	when: windowShown

	property string typeId: "TestType"

	Component {
		id: serviceComp

		DocumentServiceBase {
			property string failure: ""

			function openDocument(objectTypeId, objectId){
				if (failure !== ""){
					openDocumentFailed("", failure)
					return
				}

				handleDocumentOpened("doc", objectId, objectTypeId, "Name", false, false)
			}

			function createDocument(objectTypeId, proposedSourceDocumentId){
				if (failure !== ""){
					createDocumentFailed(objectTypeId, failure)
					return
				}

				handleDocumentCreated("doc", objectTypeId, "", false, proposedSourceDocumentId, false, true)
			}

			function closeDocument(documentId){
				documentClosed(documentId)
			}
		}
	}

	Component {
		id: viewComp

		ViewBase {
		}
	}

	Component {
		id: controllerComp

		DocumentRepresentationController {
			property int requestCount: 0

			representationModel: QtObject {
				signal modelChanged(var changeSet)
			}

			function updateRepresentationFromDocument(){
				requestCount++
				startUpdateRepresentation(documentId, representationModel)
			}

			function updateDocumentFromRepresentation(){
			}
		}
	}

	Component {
		id: shellComp

		SingleDocumentWorkspaceShellView {
			width: 200
			height: 200
		}
	}

	Component {
		id: spyComp

		SignalSpy {
		}
	}

	function openShell(failure, createNew){
		let service = createTemporaryObject(serviceComp, testCase, {"failure": failure || ""})
		service.registerDocumentViewData(typeId, "TestView", viewComp, controllerComp)

		// TestCase itself is invisible, the document view needs a visible parent
		let shell = createTemporaryObject(shellComp, testCase.parent)
		shell.objectTypeId = typeId
		shell.documentManager = service

		// The stub service answers synchronously, so the request is triggered only after the shell is bound to it
		if (createNew === true){
			shell.createNew = true
		}
		else{
			shell.objectId = "obj"
		}

		return shell
	}

	function controllerOf(service){
		let index = service.getDocumentIndexByDocumentId("doc")

		return service.__internal.openedDocuments[index].documentDecorator.registeredRepresentation[0]
	}

	function test_contentAfterRepresentation(){
		let shell = openShell()
		let service = shell.documentManager
		let readySpy = createTemporaryObject(spyComp, testCase, {"target": shell, "signalName": "documentReady"})

		compare(shell.documentId, "doc")
		compare(shell.state, "loading")

		service.setDocumentIsLoading("doc", false)

		compare(shell.state, "loading", "Content must wait for the representation")

		let controller = controllerOf(service)
		controller.representationUpdated("doc", controller.representationModel)

		compare(shell.state, "content")
		compare(readySpy.count, 1)
	}

	function test_errorWhenRepresentationFails(){
		let shell = openShell()
		let service = shell.documentManager

		service.setDocumentIsLoading("doc", false)
		controllerOf(service).updateRepresentationFailed("doc", "Representation failed")

		compare(shell.state, "error")
		compare(shell.lastErrorMessage, "Representation failed")
	}

	function test_retryRequestsRepresentationAgain(){
		let shell = openShell()
		let service = shell.documentManager

		service.setDocumentIsLoading("doc", false)
		let controller = controllerOf(service)
		controller.updateRepresentationFailed("doc", "Representation failed")

		shell.retry()

		compare(controller.requestCount, 2)
		compare(shell.state, "loading")
		compare(shell.lastErrorMessage, "")

		controller.representationUpdated("doc", controller.representationModel)

		compare(shell.state, "content")
	}

	function test_createNewDocument(){
		let shell = openShell("", true)

		compare(shell.documentId, "doc")
		compare(shell.state, "content", "A new document needs no server representation")
		compare(controllerOf(shell.documentManager).requestCount, 0)
	}

	function test_errorWhenCreateFails(){
		let shell = openShell("Create failed", true)

		compare(shell.state, "error")
		compare(shell.lastErrorMessage, "Create failed")
	}

	function test_errorWhenOpenFails(){
		let shell = openShell("Open failed")

		compare(shell.state, "error")
		compare(shell.lastErrorMessage, "Open failed")
	}

	function test_closedDocumentReturnsToEmpty(){
		let shell = openShell()
		let closedSpy = createTemporaryObject(spyComp, testCase, {"target": shell, "signalName": "closed"})

		shell.closeDocument()

		compare(closedSpy.count, 1)
		compare(shell.state, "empty")
		compare(shell.documentId, "")
	}
}
