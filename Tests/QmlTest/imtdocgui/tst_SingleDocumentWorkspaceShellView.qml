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
			function openDocument(objectTypeId, objectId){
				handleDocumentOpened("doc", objectId, objectTypeId, "Name", false, false)
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
			representationModel: QtObject {
				signal modelChanged(var changeSet)
			}

			function updateRepresentationFromDocument(){
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

	function openShell(){
		let service = createTemporaryObject(serviceComp, testCase)
		service.registerDocumentViewData(typeId, "TestView", viewComp, controllerComp)

		// TestCase itself is invisible, the document view needs a visible parent
		let shell = createTemporaryObject(shellComp, testCase.parent)
		shell.objectTypeId = typeId
		shell.documentManager = service
		shell.objectId = "obj"

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
}
