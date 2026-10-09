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

	function addView(service, documentId, visible, viewType){
		// TestCase itself is invisible, views need a visible parent
		let view = createTemporaryObject(viewComp, testCase.parent, {"visible": visible !== false})
		service.onViewInstanceCreated(documentId, view, viewType || viewTypeId)

		return view
	}

	function controllerOf(service, documentId, viewIndex){
		let index = service.getDocumentIndexByDocumentId(documentId)
		let controllers = service.__internal.openedDocuments[index].documentDecorator.registeredRepresentation

		return controllers[viewIndex || 0]
	}

	function isBlocked(view){
		return view.internal__.blockingUpdateModel
	}

	function argumentsOf(spy, index){
		return Array.prototype.slice.call(spy.signalArguments[index])
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

	function test_hiddenViewRequestingOnCreateDoesNotBlockReady(){
		let service = createService(controllerComp)
		service.registerDocumentViewData(typeId, "OnCreateView", viewComp, onCreateControllerComp)
		let readySpy = createSpy(service, "documentReady")

		service.handleDocumentCreated("doc", typeId, "", false, "", false, true)
		let visibleView = addView(service, "doc")
		let hiddenView = addView(service, "doc", false, "OnCreateView")

		compare(readySpy.count, 1, "The visible view does not need a server representation")
		verify(!isBlocked(visibleView))
		compare(controllerOf(service, "doc", 1).requestCount, 0)

		hiddenView.visible = true

		compare(controllerOf(service, "doc", 1).requestCount, 1)
	}

	function test_readyAfterShowingViewThatNeedsNoRepresentation(){
		let service = createService(controllerComp)
		service.registerDocumentViewData(typeId, "OnCreateView", viewComp, onCreateControllerComp)
		let readySpy = createSpy(service, "documentReady")

		service.handleDocumentCreated("doc", typeId, "", false, "", false, true)
		addView(service, "doc", false, "OnCreateView")
		let plainView = addView(service, "doc", false)

		compare(readySpy.count, 0, "With no visible view every view counts")

		plainView.visible = true

		compare(readySpy.count, 1)
	}

	function test_representationRequestedAgainAfterFailure(){
		let service = createService(controllerComp)
		let readySpy = createSpy(service, "documentReady")

		service.handleDocumentOpened("doc", "obj", typeId, "Name", false, false)
		addView(service, "doc")
		service.setDocumentIsLoading("doc", false)

		let controller = controllerOf(service, "doc")
		controller.fail()

		service.updateDocumentRepresentation("doc")

		compare(controller.requestCount, 2)
		compare(readySpy.count, 0)

		controller.respond()

		compare(readySpy.count, 1)
	}

	// ---- Several visible views ----

	function openWithThreeViews(service){
		service.registerDocumentViewData(typeId, "V2", viewComp, controllerComp)
		service.registerDocumentViewData(typeId, "V3", viewComp, controllerComp)
		service.handleDocumentOpened("doc", "obj", typeId, "Name", false, false)

		return [addView(service, "doc"), addView(service, "doc", true, "V2"), addView(service, "doc", true, "V3")]
	}

	function test_readyWaitsForAllVisibleViews(){
		let service = createService(controllerComp)
		let readySpy = createSpy(service, "documentReady")
		openWithThreeViews(service)
		service.setDocumentIsLoading("doc", false)

		for (let i = 0; i < 3; ++i){
			compare(controllerOf(service, "doc", i).requestCount, 1)
		}

		controllerOf(service, "doc", 0).respond()
		controllerOf(service, "doc", 1).respond()

		compare(readySpy.count, 0)

		controllerOf(service, "doc", 2).respond()

		compare(readySpy.count, 1)
	}

	function test_failedViewKeepsOtherViewsLoading(){
		let service = createService(controllerComp)
		let readySpy = createSpy(service, "documentReady")
		let views = openWithThreeViews(service)
		service.setDocumentIsLoading("doc", false)

		controllerOf(service, "doc", 0).fail()

		verify(!isBlocked(views[0]))
		verify(isBlocked(views[1]), "Only the failed view is released")
		verify(isBlocked(views[2]))

		controllerOf(service, "doc", 1).respond()
		controllerOf(service, "doc", 2).respond()

		compare(readySpy.count, 0, "A failed view keeps the document not ready")

		service.updateDocumentRepresentation("doc")

		for (let i = 0; i < 3; ++i){
			compare(controllerOf(service, "doc", i).requestCount, 2)
			controllerOf(service, "doc", i).respond()
		}

		compare(readySpy.count, 1)
	}

	// ---- View registration ----

	function test_viewDataRegistration(){
		let service = createTemporaryObject(serviceComp, testCase)
		let registeredSpy = createSpy(service, "documentViewRegistered")

		service.registerDocumentViewData("A", "V1", viewComp, controllerComp)
		service.registerDocumentViewData("A", "V2", viewComp, onCreateControllerComp)
		service.registerDocumentViewData("B", "V1", viewComp, controllerComp)
		service.registerDocumentViewData("A", "V1", viewComp, onCreateControllerComp)

		compare(registeredSpy.count, 3, "A duplicate view type is rejected")
		compare(service.getSupportedDocumentTypeIds().sort(), ["A", "B"])
		compare(service.getSupportedDocumentViewTypeIds("A"), ["V1", "V2"])
		compare(service.getSupportedDocumentViewTypeIds("C"), [])
		verify(service.getDocumentRepresentationControllerFactory("A", "V1") === controllerComp)
		verify(service.getDocumentRepresentationControllerFactory("A", "V2") === onCreateControllerComp)
		verify(service.getDocumentRepresentationControllerFactory("A", "") === controllerComp)
		verify(service.getDocumentRepresentationControllerFactory("C", "V1") === null)
		verify(service.getDocumentEditorFactory("A") === viewComp)
		verify(service.getDocumentEditorFactory("C", "V1") === null)
		compare(service.getViewTypeIdByViewFactory("A", viewComp), "V1")
		compare(service.getViewTypeIdByViewFactory("C", viewComp), "")
	}

	function test_viewLookup(){
		let service = createService(controllerComp)
		service.registerDocumentViewData(typeId, "V2", viewComp, controllerComp)
		service.handleDocumentOpened("doc", "obj", typeId, "Name", false, false)

		let firstView = createTemporaryObject(viewComp, testCase.parent)
		service.onViewInstanceCreated("doc", firstView, "")
		let secondView = addView(service, "doc", true, "V2")

		verify(service.getDocumentViewInstance("doc", viewTypeId) === firstView, "An empty view type falls back to the first registered one")
		verify(service.getDocumentViewInstance("doc", "") === firstView)
		verify(service.getDocumentViewInstance("doc", "V2") === secondView)
		compare(service.getDocumentIdByView(secondView), "doc")
		compare(service.getDocumentIdByView(testCase), "")
	}

	function test_viewWithoutControllerFactory(){
		let service = createService(null)
		let readySpy = createSpy(service, "documentReady")

		service.handleDocumentOpened("doc", "obj", typeId, "Name", false, false)
		addView(service, "doc")
		service.setDocumentIsLoading("doc", false)

		compare(readySpy.count, 1)
		verify(readySpy.signalArguments[0][3] === null)
	}

	function test_destroyedViewIsUnregistered(){
		let service = createService(controllerComp)
		let readySpy = createSpy(service, "documentReady")

		service.handleDocumentOpened("doc", "obj", typeId, "Name", false, false)
		let view = addView(service, "doc")
		service.setDocumentIsLoading("doc", false)
		controllerOf(service, "doc").respond()

		view.destroy()
		wait(0)

		let index = service.getDocumentIndexByDocumentId("doc")
		let decorator = service.__internal.openedDocuments[index].documentDecorator

		compare(decorator.registeredViews.length, 0)
		compare(service.getDocumentIdByView(view), "")

		// Must not touch the destroyed view
		decorator.updateRepresentationForAllViews()
		verify(!decorator.isUpdatingRepresentation())

		let newView = addView(service, "doc")

		compare(controllerOf(service, "doc").requestCount, 1, "A new view loads the representation again")
		verify(service.getDocumentViewInstance("doc", viewTypeId) === newView)
		compare(readySpy.count, 1)
	}

	// ---- Document state ----

	function test_documentStateQueries(){
		let service = createService(controllerComp)

		service.handleDocumentOpened("doc", "obj", typeId, "Name", false, true)

		verify(service.documentIsOpened("doc"))
		compare(service.getOpenedDocumentIds(), ["doc"])
		compare(service.getDocumentTypeId("doc"), typeId)
		compare(service.getDocumentObjectId("doc"), "obj")
		compare(service.getDocumentIdByObjectId("obj"), "doc")
		compare(service.getDocumentName("doc"), "Name")
		verify(service.documentIsDirty("doc"))
		verify(!service.documentIsNew("doc"))
		verify(service.documentIsLoading("doc"))

		verify(!service.documentIsOpened("unknown"))
		compare(service.getDocumentTypeId("unknown"), "")
		compare(service.getDocumentObjectId("unknown"), "")
		compare(service.getDocumentIdByObjectId("unknown"), "")
		compare(service.getDocumentName("unknown"), "")
		verify(!service.documentIsDirty("unknown"))
		verify(!service.documentIsLoading("unknown"))
	}

	function test_nameAndObjectIdCachedBeforeDocumentExists(){
		let service = createService(controllerComp)
		let nameSpy = createSpy(service, "documentNameChanged")

		service.setDocumentName("doc", "Cached")
		service.setDocumentObjectId("doc", "obj")
		service.documentOpened("doc", typeId)

		compare(service.getDocumentName("doc"), "Cached")
		compare(service.getDocumentObjectId("doc"), "obj")
		compare(nameSpy.count, 0)

		service.setDocumentName("doc", "New")

		compare(nameSpy.count, 1)
		compare(argumentsOf(nameSpy, 0), ["doc", "Cached", "New"])
	}

	function test_dirtyState(){
		let service = createService(controllerComp)
		let dirtySpy = createSpy(service, "documentIsDirtyChanged")

		service.handleDocumentOpened("doc", "obj", typeId, "Name", false, false)
		service.setDocumentIsDirty("doc", true)

		verify(service.documentIsDirty("doc"))
		compare(argumentsOf(dirtySpy, 0), ["doc", true])

		service.undoInfoReceived("doc", 0, 0, false)

		verify(!service.documentIsDirty("doc"))
		compare(dirtySpy.count, 2)
	}

	function test_newDocumentStopsBeingNewWhenSaved(){
		let service = createService(controllerComp)
		let savedSpy = createSpy(service, "documentSaved")

		service.handleDocumentCreated("doc", typeId, "", false, "", false, true)

		verify(service.documentIsNew("doc"))

		service.handleSaveDocumentResult("doc", "Success", "", "Saved name")

		compare(savedSpy.count, 1)
		verify(!service.documentIsNew("doc"))
		compare(service.getDocumentName("doc"), "Saved name")
	}

	function test_saveFailureMessage(){
		let service = createService(controllerComp)
		let failedSpy = createSpy(service, "saveDocumentFailed")

		service.handleSaveDocumentResult("doc", "Failed", "Disk full", "")
		service.handleSaveDocumentResult("doc", "Failed", "", "")

		compare(argumentsOf(failedSpy, 0), ["doc", "Disk full"])
		compare(argumentsOf(failedSpy, 1), ["doc", "Failed"])
	}

	function test_operationResult_data(){
		return [
			{"tag": "close success", "operation": "Close", "status": "Success", "signalName": "documentClosed", "message": ""},
			{"tag": "close invalid user", "operation": "Close", "status": "InvalidUserId", "signalName": "closeDocumentFailed", "message": "Invalid user-ID"},
			{"tag": "close invalid document", "operation": "Close", "status": "InvalidDocumentId", "signalName": "closeDocumentFailed", "message": "Invalid document-ID"},
			{"tag": "close failed", "operation": "Close", "status": "Failed", "signalName": "closeDocumentFailed", "message": "Close document failed"},
			{"tag": "undo success", "operation": "Undo", "status": "Success", "signalName": "undoDone", "message": ""},
			{"tag": "undo failed", "operation": "Undo", "status": "Failed", "signalName": "undoFailed", "message": "Undo failed"},
			{"tag": "undo steps", "operation": "Undo", "status": "InvalidStepCount", "signalName": "undoFailed", "message": "Invalid step count"},
			{"tag": "redo success", "operation": "Redo", "status": "Success", "signalName": "redoDone", "message": ""},
			{"tag": "redo failed", "operation": "Redo", "status": "Failed", "signalName": "redoFailed", "message": "Redo failed"},
			{"tag": "redo unknown", "operation": "Redo", "status": "Unexpected", "signalName": "redoFailed", "message": "Unexpected"}
		]
	}

	function test_operationResult(data){
		let service = createService(controllerComp)
		let spy = createSpy(service, data.signalName)

		service.handleDocumentOpened("doc", "obj", typeId, "Name", false, false)
		service["handle" + data.operation + (data.operation === "Close" ? "DocumentResult" : "Result")]("doc", data.status)

		compare(spy.count, 1)
		compare(spy.signalArguments[0][0], "doc")
		if (data.message !== ""){
			compare(spy.signalArguments[0][1], data.message)
		}
	}

	function test_openedTwiceEmitsOnce(){
		let service = createService(controllerComp)
		let openedSpy = createSpy(service, "documentOpened")

		service.handleDocumentOpened("doc", "obj", typeId, "Name", false, false)
		service.handleDocumentOpened("doc", "obj", typeId, "Name", false, true)

		compare(openedSpy.count, 1)
		verify(service.documentIsDirty("doc"), "The second call still refreshes the state")
	}

	function test_createdWithProposedObjectId(){
		let service = createService(controllerComp)
		let createdSpy = createSpy(service, "documentCreated")

		service.handleDocumentCreated("doc", typeId, "Name", true, "proposed", true, true)

		compare(createdSpy.count, 1)
		compare(service.getDocumentObjectId("doc"), "proposed")
		verify(service.documentIsDirty("doc"))
		verify(service.hasDocumentNameProvider(typeId))
		verify(!service.documentIsLoading("doc"))
	}

	function test_remoteNotificationsIgnoreKnownAndInvalidDocuments(){
		let service = createService(controllerComp)
		let openedSpy = createSpy(service, "documentOpened")
		let createdSpy = createSpy(service, "documentCreated")

		service.reflectRemoteDocumentOpened("", "obj", typeId, "Name", false, false)
		service.reflectRemoteDocumentOpened("doc", "obj", "", "Name", false, false)
		service.reflectRemoteDocumentCreated("", "", typeId, "Name", false, false)

		compare(openedSpy.count, 0)
		compare(createdSpy.count, 0)

		service.reflectRemoteDocumentOpened("doc", "obj", typeId, "Name", undefined, false)
		service.reflectRemoteDocumentOpened("doc", "obj", typeId, "Name", false, false)
		service.reflectRemoteDocumentCreated("doc", "", typeId, "Name", false, false)

		compare(openedSpy.count, 1)
		compare(createdSpy.count, 0)
	}

	function test_saveNameResolver(){
		let service = createService(controllerComp)

		compare(service.resolveDocumentNameForSave("doc", "Fallback"), "Fallback")

		let resolvedName = "Resolved"
		service.setDocumentSaveNameResolver("doc", function(documentId){ return resolvedName })

		compare(service.resolveDocumentNameForSave("doc", "Fallback"), "Resolved")
		compare(service.resolveDocumentNameForSave("", "Fallback"), "Fallback")

		resolvedName = ""

		compare(service.resolveDocumentNameForSave("doc", "Fallback"), "Fallback", "An empty result keeps the fallback")

		resolvedName = "Resolved"
		service.clearDocumentSaveNameResolver("doc")

		compare(service.resolveDocumentNameForSave("doc", "Fallback"), "Fallback")

		service.setDocumentSaveNameResolver("doc", function(documentId){ return resolvedName })
		service.setDocumentSaveNameResolver("doc", null)

		compare(service.resolveDocumentNameForSave("doc", "Fallback"), "Fallback")
	}

	// ---- Closing ----

	function test_closingDocumentIgnoresLoadingUpdates(){
		let service = createService(controllerComp)
		let loadedSpy = createSpy(service, "documentDataLoaded")

		service.handleDocumentOpened("doc", "obj", typeId, "Name", false, false)
		service.setDocumentSaveNameResolver("doc", function(documentId){ return "Resolved" })
		service.startCloseDocument("doc")
		service.setDocumentIsLoading("doc", false)

		compare(loadedSpy.count, 0)

		service.documentClosed("doc")

		verify(!service.documentIsOpened("doc"))
		compare(service.getOpenedDocumentIds(), [])
		compare(service.resolveDocumentNameForSave("doc", "Fallback"), "Fallback")
	}

	function test_failedCloseKeepsDocumentWorking(){
		let service = createService(controllerComp)
		let loadedSpy = createSpy(service, "documentDataLoaded")

		service.handleDocumentOpened("doc", "obj", typeId, "Name", false, false)
		service.startCloseDocument("doc")
		service.closeDocumentFailed("doc", "Close document failed")
		service.setDocumentIsLoading("doc", false)

		verify(service.documentIsOpened("doc"))
		compare(loadedSpy.count, 1)
	}

	function test_reopenedDocumentIsReadyAgain(){
		let service = createService(controllerComp)
		let readySpy = createSpy(service, "documentReady")

		service.handleDocumentOpened("doc", "obj", typeId, "Name", false, false)
		addView(service, "doc")
		service.setDocumentIsLoading("doc", false)
		controllerOf(service, "doc").respond()
		service.documentClosed("doc")

		service.handleDocumentOpened("doc", "obj", typeId, "Name", false, false)
		addView(service, "doc")
		service.setDocumentIsLoading("doc", false)
		controllerOf(service, "doc").respond()

		compare(readySpy.count, 2)
	}

	function test_serviceActivatedOnce(){
		let service = createService(controllerComp)
		let activatedSpy = createSpy(service, "documentServiceActivated")

		service.setDocumentServiceActiveView(testCase)
		service.setDocumentServiceActiveView(testCase)

		compare(activatedSpy.count, 1)
		verify(service.getDocumentServiceActiveView() === testCase)
	}
}
