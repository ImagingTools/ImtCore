import QtQuick 2.12
import QtTest 1.12
import imtgui 1.0
import imtdocgui 1.0

TestCase {
	id: testCase

	name: "DocumentServiceBase"
	when: windowShown

	property string typeId: "TestType"
	property string viewTypeId: "TestView"

	Component {
		id: serviceComp

		DocumentServiceBase {
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

			function respond(){
				representationUpdated(documentId, representationModel)
			}

			function fail(){
				updateRepresentationFailed(documentId, "Representation failed")
			}
		}
	}

	Component {
		id: onCreateControllerComp

		DocumentRepresentationController {
			property int requestCount: 0

			requestRepresentationOnCreate: true
			representationModel: QtObject {
				signal modelChanged(var changeSet)
			}

			function updateRepresentationFromDocument(){
				requestCount++
				startUpdateRepresentation(documentId, representationModel)
			}

			function updateDocumentFromRepresentation(){
			}

			function respond(){
				representationUpdated(documentId, representationModel)
			}
		}
	}

	Component {
		id: spyComp

		SignalSpy {
		}
	}

	function createService(controllerFactory){
		let service = createTemporaryObject(serviceComp, testCase)
		service.registerDocumentViewData(typeId, viewTypeId, viewComp, controllerFactory)

		return service
	}

	function createSpy(service, signalName){
		return createTemporaryObject(spyComp, testCase, {"target": service, "signalName": signalName})
	}

	function addView(service, documentId, visible){
		// TestCase itself is invisible, views need a visible parent
		let view = createTemporaryObject(viewComp, testCase.parent, {"visible": visible !== false})
		service.onViewInstanceCreated(documentId, view, viewTypeId)

		return view
	}

	function controllerOf(service, documentId){
		let index = service.getDocumentIndexByDocumentId(documentId)
		let controllers = service.__internal.openedDocuments[index].documentDecorator.registeredRepresentation

		return controllers[0]
	}

	function isBlocked(view){
		return view.internal__.blockingUpdateModel
	}

	function test_openedDocumentReadyAfterRepresentation(){
		let service = createService(controllerComp)
		let readySpy = createSpy(service, "documentReady")
		let loadedSpy = createSpy(service, "documentDataLoaded")

		service.handleDocumentOpened("doc", "obj", typeId, "Name", false, false)
		let view = addView(service, "doc")
		let controller = controllerOf(service, "doc")

		compare(controller.requestCount, 0, "No request while the server is loading")
		compare(readySpy.count, 0)

		service.setDocumentIsLoading("doc", false)

		compare(loadedSpy.count, 1)
		compare(controller.requestCount, 1)
		compare(readySpy.count, 0, "Ready must wait for the representation")
		verify(isBlocked(view))

		controller.respond()

		compare(readySpy.count, 1)
		compare(readySpy.signalArguments[0][0], "doc")
		compare(readySpy.signalArguments[0][1], typeId)
		compare(readySpy.signalArguments[0][2], false)
		verify(readySpy.signalArguments[0][3] === controller)
		verify(!isBlocked(view))
	}

	function test_viewRegisteredAfterDataLoaded(){
		let service = createService(controllerComp)
		let readySpy = createSpy(service, "documentReady")

		service.handleDocumentOpened("doc", "obj", typeId, "Name", false, false)
		service.setDocumentIsLoading("doc", false)

		compare(readySpy.count, 0, "No view registered yet")

		addView(service, "doc")
		let controller = controllerOf(service, "doc")

		compare(controller.requestCount, 1)
		compare(readySpy.count, 0)

		controller.respond()

		compare(readySpy.count, 1)
	}

	function test_dataLoadedNotificationBeforeOpenResponse(){
		let service = createService(controllerComp)
		let readySpy = createSpy(service, "documentReady")

		service.setDocumentIsLoading("doc", false)
		service.handleDocumentOpened("doc", "obj", typeId, "Name", false, false)

		verify(!service.documentIsLoading("doc"))

		addView(service, "doc")
		let controller = controllerOf(service, "doc")

		compare(controller.requestCount, 1)

		controller.respond()

		compare(readySpy.count, 1)
	}

	function test_repeatedDataLoadedIsIgnored(){
		let service = createService(controllerComp)
		let loadedSpy = createSpy(service, "documentDataLoaded")

		service.handleDocumentOpened("doc", "obj", typeId, "Name", false, false)
		addView(service, "doc")
		service.setDocumentIsLoading("doc", false)
		service.setDocumentIsLoading("doc", false)

		compare(loadedSpy.count, 1)
		compare(controllerOf(service, "doc").requestCount, 1)
	}

	function test_newDocumentWithClientRepresentation(){
		let service = createService(controllerComp)
		let readySpy = createSpy(service, "documentReady")
		let view = null

		let createView = function(documentId){
			view = addView(service, documentId)
		}
		service.documentCreated.connect(createView)

		service.handleDocumentCreated("doc", typeId, "", false, "", false, true)
		service.documentCreated.disconnect(createView)

		compare(controllerOf(service, "doc").requestCount, 0)
		compare(readySpy.count, 1)
		compare(readySpy.signalArguments[0][2], true)
		verify(!isBlocked(view))
	}

	function test_newDocumentViewRegisteredLater(){
		let service = createService(controllerComp)
		let readySpy = createSpy(service, "documentReady")

		service.handleDocumentCreated("doc", typeId, "", false, "", false, true)

		compare(readySpy.count, 0)

		let view = addView(service, "doc")

		compare(controllerOf(service, "doc").requestCount, 0)
		compare(readySpy.count, 1)
		verify(!isBlocked(view), "A view added after loading must not stay blocked")
	}

	function test_newDocumentRequestsRepresentationOnCreate(){
		let service = createService(onCreateControllerComp)
		let readySpy = createSpy(service, "documentReady")
		let view = null

		let createView = function(documentId){
			view = addView(service, documentId)
		}
		service.documentCreated.connect(createView)

		service.handleDocumentCreated("doc", typeId, "", false, "", false, true)
		service.documentCreated.disconnect(createView)

		let controller = controllerOf(service, "doc")

		compare(controller.requestCount, 1, "Exactly one request for a view created in documentCreated")
		compare(readySpy.count, 0)
		verify(isBlocked(view))

		controller.respond()

		compare(readySpy.count, 1)
		compare(readySpy.signalArguments[0][2], true)
		verify(!isBlocked(view))
	}

	function test_remoteCreatedDocumentWaitsForDataLoaded(){
		let service = createService(onCreateControllerComp)
		let readySpy = createSpy(service, "documentReady")

		service.reflectRemoteDocumentCreated("doc", "", typeId, "Name", false, false)

		verify(service.documentIsLoading("doc"))

		addView(service, "doc")
		let controller = controllerOf(service, "doc")

		compare(controller.requestCount, 0, "The object may not exist on the server yet")

		service.setDocumentIsLoading("doc", false)

		compare(controller.requestCount, 1)

		// CreateNewDocument response arriving after the notification
		service.handleDocumentCreated("doc", typeId, "Name", false, "", false, true)

		compare(controller.requestCount, 1)

		controller.respond()

		compare(readySpy.count, 1)
	}

	function test_createResponseBeforeDataLoadedNotification(){
		let service = createService(onCreateControllerComp)
		let readySpy = createSpy(service, "documentReady")

		service.reflectRemoteDocumentCreated("doc", "", typeId, "Name", false, false)
		addView(service, "doc")
		service.handleDocumentCreated("doc", typeId, "Name", false, "", false, true)

		let controller = controllerOf(service, "doc")

		compare(controller.requestCount, 1)

		service.setDocumentIsLoading("doc", false)

		compare(controller.requestCount, 1)

		controller.respond()

		compare(readySpy.count, 1)
	}

	function test_failedRepresentationDoesNotEmitReady(){
		let service = createService(controllerComp)
		let readySpy = createSpy(service, "documentReady")
		let failedSpy = createSpy(service, "updateRepresentationFailed")

		service.handleDocumentOpened("doc", "obj", typeId, "Name", false, false)
		let view = addView(service, "doc")
		service.setDocumentIsLoading("doc", false)

		controllerOf(service, "doc").fail()

		compare(failedSpy.count, 1)
		compare(readySpy.count, 0)
		verify(!isBlocked(view))
	}

	function test_hiddenViewDefersReady(){
		let service = createService(controllerComp)
		let readySpy = createSpy(service, "documentReady")

		service.handleDocumentOpened("doc", "obj", typeId, "Name", false, false)
		let view = addView(service, "doc", false)
		service.setDocumentIsLoading("doc", false)

		let controller = controllerOf(service, "doc")

		compare(controller.requestCount, 0)
		compare(readySpy.count, 0)

		view.visible = true

		compare(controller.requestCount, 1)

		controller.respond()

		compare(readySpy.count, 1)
	}

	function test_readyEmittedOnce(){
		let service = createService(controllerComp)
		let readySpy = createSpy(service, "documentReady")

		service.handleDocumentOpened("doc", "obj", typeId, "Name", false, false)
		addView(service, "doc")
		service.setDocumentIsLoading("doc", false)

		let controller = controllerOf(service, "doc")
		controller.respond()

		controller.updateRepresentationFromDocument()
		controller.respond()

		compare(controller.requestCount, 2)
		compare(readySpy.count, 1)
	}
}
