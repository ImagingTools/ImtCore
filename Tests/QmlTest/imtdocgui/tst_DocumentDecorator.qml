import QtQuick 2.12
import QtTest 1.12
import imtcontrols 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtguigql 1.0
import imtqmltest 1.0
import imtbaseCollectionDocumentServiceSdl 1.0

TestCase {
	id: testCase

	name: "DocumentDecorator"
	when: windowShown

	property string typeId: "TestType"
	property string viewTypeId: "TestView"

	Component {
		id: serviceComp

		DocumentServiceBase {
			property var calls: []

			function saveDocument(documentId, documentName){
				calls.push(["save", documentId, documentName])
			}

			function doUndo(documentId, steps){
				calls.push(["undo", documentId, steps])
			}

			function doRedo(documentId, steps){
				calls.push(["redo", documentId, steps])
			}
		}
	}

	// Saves as GqlBasedCollectionDocumentService does: after the pending updates
	Component {
		id: waitingServiceComp

		DocumentServiceBase {
			property var calls: []

			function saveDocument(documentId, documentName){
				if (documentHasPendingUpdates(documentId)){
					saveDocumentAfterUpdates(documentId, documentName)
					return
				}

				calls.push(["save", documentId, documentName])
			}
		}
	}

	Component {
		id: viewComp

		ViewBase {
			width: 10
			height: 10

			property bool hasChanges: true
			property int savedCount: 0
			property bool inputAcceptable: true

			function acceptsSave(){
				return inputAcceptable
			}

			function updateModel(){
				if (hasChanges){
					model.modelChanged(null)
				}
			}

			function documentSaved(){
				savedCount++
			}
		}
	}

	Component {
		id: controllerComp

		DocumentRepresentationController {
			property int requestCount: 0
			property int updateCount: 0
			property var lastChangeSet: null
			property var changeIds: []

			representationModel: QtObject {
				signal modelChanged(var changeSet)
			}

			function updateRepresentationFromDocument(){
				requestCount++
				startUpdateRepresentation(documentId, representationModel)
			}

			// Edits the model again while it sends the update, as the role controller does with the object-ID
			property bool editsWhileSending: false
			property string changeIdWhileSending: ""

			function updateDocumentFromRepresentation(changeSet){
				updateCount++
				lastChangeSet = changeSet
				let ids = changeIds
				ids.push(changeId)
				changeIds = ids
				startUpdateDocument(documentId)
				if (editsWhileSending){
					editsWhileSending = false
					representationModel.modelChanged([{"path": ["id"], "operation": "set", "name": "m_id", "sender": representationModel}])
				}
				changeIdWhileSending = changeId
			}

			function respond(){
				representationUpdated(documentId, representationModel)
			}

			// The server's answer to an own update
			function answer(succeeded){
				if (succeeded){
					documentUpdated(documentId)
				}
				else{
					updateDocumentFailed(documentId, "Refused")
				}
			}
		}
	}

	// Sends its updates with DocumentUpdateRequestSender to the test transport
	Component {
		id: gqlControllerComp

		DocumentRepresentationController {
			id: gqlController

			property int requestCount: 0

			representationModel: QtObject {
				signal modelChanged(var changeSet)
			}

			function updateRepresentationFromDocument(){
				requestCount++
				startUpdateRepresentation(documentId, representationModel)
			}

			function updateDocumentFromRepresentation(changeSet){
				startUpdateDocument(documentId)
				updateRequest.send()
			}

			function respond(){
				representationUpdated(documentId, representationModel)
			}

			property DocumentUpdateRequestSender updateRequest: DocumentUpdateRequestSender {
				representationController: gqlController
				gqlCommandId: "UpdateDocumentFromRepresentation"
			}
		}
	}

	Component {
		id: modelComp

		QtObject {
			signal modelChanged(var changeSet)
		}
	}

	Component {
		id: spyComp

		SignalSpy {
		}
	}

	function createSpy(target, signalName){
		return createTemporaryObject(spyComp, testCase, {"target": target, "signalName": signalName})
	}

	function addView(service){
		// TestCase itself is invisible, views need a visible parent
		let view = createTemporaryObject(viewComp, testCase.parent)
		service.onViewInstanceCreated("doc", view, viewTypeId)

		return view
	}

	function controllerOf(service, viewIndex){
		let index = service.getDocumentIndexByDocumentId("doc")

		return service.__internal.openedDocuments[index].documentDecorator.registeredRepresentation[viewIndex || 0]
	}

	// Opens a document whose representation is loaded, so its views accept edits.
	function openDocument(documentName, hasNameProvider, viewCount){
		let service = createTemporaryObject(serviceComp, testCase)
		service.registerDocumentViewData(typeId, viewTypeId, viewComp, controllerComp)
		service.handleDocumentOpened("doc", "obj", typeId, documentName, hasNameProvider, false)

		let views = []
		for (let i = 0; i < (viewCount || 1); ++i){
			views.push(addView(service))
		}

		service.setDocumentIsLoading("doc", false)
		for (let i = 0; i < views.length; ++i){
			controllerOf(service, i).respond()
		}

		return {"service": service, "views": views}
	}

	function test_saveWithoutChanges(){
		let document = openDocument("Name", false)
		document.views[0].hasChanges = false

		document.views[0].commandActivated("Save")

		compare(document.service.calls, [["save", "doc", "Name"]])
		compare(controllerOf(document.service).updateCount, 0)
	}

	function test_viewRefusingSaveBlocksTheSave(){
		let document = openDocument("Name", false, 2)
		document.views[1].inputAcceptable = false

		document.views[0].commandActivated("Save")

		compare(document.service.calls, [], "A view with unacceptable input refuses the save")
		compare(controllerOf(document.service, 0).updateCount, 0, "Nothing is written to the document either")

		document.views[1].inputAcceptable = true
		document.views[0].commandActivated("Save")

		compare(document.service.calls, [["save", "doc", "Name"]])
	}

	function test_saveFlushesGuiOfAllViews(){
		let document = openDocument("Name", false, 2)

		document.views[0].commandActivated("Save")

		compare(controllerOf(document.service, 0).updateCount, 1, "Save first writes the GUI state to the document")
		compare(controllerOf(document.service, 1).updateCount, 1)
		compare(document.service.calls, [["save", "doc", "Name"]])
	}

	function test_saveWithoutNameRequestsName(){
		let document = openDocument("", false)
		let nameSpy = createSpy(document.service, "requestDocumentName")
		document.views[0].hasChanges = false

		document.views[0].commandActivated("Save")

		compare(nameSpy.count, 1)
		compare(Array.prototype.slice.call(nameSpy.signalArguments[0]), ["doc", typeId])
		compare(document.service.calls, [])

		document.service.setDocumentName("doc", "Entered")

		compare(document.service.calls, [["save", "doc", "Entered"]])
	}

	function test_cancelledNameRequestDoesNotSaveLater(){
		let document = openDocument("", false)
		document.views[0].hasChanges = false

		document.views[0].commandActivated("Save")
		document.service.startSaveDocument("doc")
		document.service.setDocumentName("doc", "Renamed")

		compare(document.service.calls, [])
	}

	function test_saveWithNameProvider(){
		let document = openDocument("", true)
		document.views[0].hasChanges = false

		document.views[0].commandActivated("Save")

		compare(document.service.calls, [["save", "doc", ""]])
	}

	function test_undoRedoCommands(){
		let document = openDocument("Name", false)

		document.views[0].commandActivated("Undo")
		document.views[0].commandActivated("Redo")

		compare(document.service.calls, [["undo", "doc", 1], ["redo", "doc", 1]])
	}

	function test_documentChangedReloadsOtherViews(){
		let document = openDocument("Name", false, 2)
		let service = document.service
		edit(document.views[0])

		notifyChanged(service, ownChangeId(service, 0))
		compare(controllerOf(service, 0).requestCount, 1, "The view that made the change is not reloaded")
		compare(controllerOf(service, 1).requestCount, 2)

		answer(controllerOf(service, 0), true)
		notifyChanged(service, "")
		compare(controllerOf(service, 0).requestCount, 2)
		compare(controllerOf(service, 1).requestCount, 3)
	}

	// --- Own updates and the notifications of the server

	function decoratorOf(service){
		return service.__internal.openedDocuments[service.getDocumentIndexByDocumentId("doc")].documentDecorator
	}

	function notifyChanged(service, changeId){
		service.documentManagerChanged(EDocumentOperationEnum.s_documentChanged, "obj", "doc", "Name", changeId)
	}

	function edit(view){
		view.model.modelChanged([])
	}

	// Change-ID the view sent its last update with
	function ownChangeId(service, viewIndex){
		let changeIds = controllerOf(service, viewIndex).changeIds

		return changeIds[changeIds.length - 1]
	}

	function change(path){
		return {"path": path, "operation": "set", "name": "", "sender": null}
	}

	// The server answers the update in flight; the decorator goes on after the answer is handled
	function answer(controller, succeeded){
		controller.answer(succeeded)
		wait(0)
	}

	function test_updateIsSentWithChangeId(){
		let document = openDocument("Name", false)
		let controller = controllerOf(document.service)
		edit(document.views[0])

		compare(controller.changeIds.length, 1)
		verify(controller.changeIds[0] !== "")
		compare(controller.changeIdWhileSending, controller.changeIds[0])
		compare(controller.changeId, "", "The ID is set only while the update is sent")
	}

	function test_ownEchoAfterAnswerIsSkipped(){
		let document = openDocument("Name", false, 2)
		let service = document.service
		edit(document.views[0])
		answer(controllerOf(service, 0), true)

		notifyChanged(service, ownChangeId(service, 0))
		compare(controllerOf(service, 0).requestCount, 1, "The view keeps its own change")
		compare(controllerOf(service, 1).requestCount, 2)
	}

	// A change of somebody else is read even while the own notification is awaited
	function test_otherChangeIsReadWhileEchoIsAwaited(){
		let document = openDocument("Name", false)
		let service = document.service
		let controller = controllerOf(service)
		edit(document.views[0])
		answer(controller, true)

		notifyChanged(service, "change-of-another-client")
		compare(controller.requestCount, 2)

		notifyChanged(service, ownChangeId(service, 0))
		compare(controller.requestCount, 2, "The own notification is still known")
	}

	// One update of a view at a time: the edits made meanwhile go together after the answer
	function test_editsDuringUpdateAreSentAfterAnswer(){
		let document = openDocument("Name", false)
		let controller = controllerOf(document.service)
		let model = document.views[0].model
		model.modelChanged([change(["name"])])
		model.modelChanged([change(["permissions", 1, "direct"])])
		model.modelChanged([change(["description"])])
		compare(controller.updateCount, 1)

		answer(controller, true)
		compare(controller.updateCount, 2)
		compare(controller.lastChangeSet.length, 2)
		compare(controller.lastChangeSet[0].path, ["permissions", 1, "direct"])
		compare(controller.lastChangeSet[1].path, ["description"])
		verify(controller.changeIds[1] !== controller.changeIds[0])
	}

	function test_queuedEditWithoutChangeSetSendsAll(){
		let document = openDocument("Name", false)
		let controller = controllerOf(document.service)
		let model = document.views[0].model
		model.modelChanged([change(["name"])])
		model.modelChanged([change(["description"])])
		model.modelChanged([])
		model.modelChanged([change(["roleId"])])

		answer(controller, true)
		compare(controller.lastChangeSet.length, 0, "An edit that does not tell what changed makes the update unknown")
	}

	// A controller that keeps a refused change in the editor sends the edits made meanwhile
	function test_queuedEditsAreSentAfterRefusalWithoutReadBack(){
		let document = openDocument("Name", false)
		let controller = controllerOf(document.service)
		controller.readBackOnRefusal = false
		edit(document.views[0])
		edit(document.views[0])

		answer(controller, false)
		compare(controller.updateCount, 2)
		compare(controller.requestCount, 1)
	}

	// The edits queued over a refused change are dropped, the view is read again once
	function test_refusalReadsBackAndDropsQueuedEdits(){
		let document = openDocument("Name", false)
		let controller = controllerOf(document.service)
		edit(document.views[0])
		edit(document.views[0])

		controller.answer(false)
		compare(controller.updateCount, 1, "Nothing is sent from inside the answer")
		compare(controller.requestCount, 1)

		wait(0)
		compare(controller.updateCount, 1)
		compare(controller.requestCount, 2)
	}

	// A read giving the view a new model: the view gets it, is released, and the edits queued on the old model are dropped
	function test_newModelOfAReadReachesTheView(){
		let document = openDocument("Name", false)
		let view = document.views[0]
		let controller = controllerOf(document.service)
		controller.view = null
		edit(view)
		edit(view)

		let newModel = createTemporaryObject(modelComp, testCase)
		controller.updateRepresentationFromDocument()
		controller.representationModel = newModel
		controller.representationUpdated(controller.documentId, newModel)

		compare(view.model, newModel)
		newModel.modelChanged([])
		compare(controller.updateCount, 1, "An edit of the new model waits for the update in flight")

		answer(controller, true)
		compare(controller.updateCount, 2)
		compare(controller.lastChangeSet.length, 0)
	}

	// --- Reads and saves wait for own edits

	// A read put off by a notification waits for the queued edits too: the read would replace them
	function test_deferredReadWaitsForQueuedEdits(){
		let document = openDocument("Name", false)
		let service = document.service
		let controller = controllerOf(service)
		edit(document.views[0])
		edit(document.views[0])

		notifyChanged(service, "")
		notifyChanged(service, ownChangeId(service, 0))
		compare(controller.requestCount, 1, "The first update is applied, the queued edit is not sent yet")

		answer(controller, true)
		compare(controller.updateCount, 2)
		compare(controller.requestCount, 1, "The queued edit is in flight")

		answer(controller, true)
		compare(controller.requestCount, 2)
	}

	function test_ownUpdateReadWaitsForQueuedEdits(){
		let document = openDocument("Name", false)
		let service = document.service
		let controller = controllerOf(service)
		controller.requestRepresentationOnUpdate = true
		edit(document.views[0])
		edit(document.views[0])

		notifyChanged(service, ownChangeId(service, 0))
		compare(controller.requestCount, 1)

		answer(controller, true)
		answer(controller, true)
		compare(controller.updateCount, 2)
		compare(controller.requestCount, 2)
	}

	function openWaitingDocument(){
		let service = createTemporaryObject(waitingServiceComp, testCase)
		service.registerDocumentViewData(typeId, viewTypeId, viewComp, controllerComp)
		service.handleDocumentOpened("doc", "obj", typeId, "Name", false, false)
		let view = addView(service)
		service.setDocumentIsLoading("doc", false)
		controllerOf(service).respond()

		return {"service": service, "view": view, "controller": controllerOf(service)}
	}

	// Save sends the GUI state as an update: the document is saved after the updates are answered
	function test_saveWaitsForPendingUpdates(){
		let document = openWaitingDocument()
		edit(document.view)
		edit(document.view)

		decoratorOf(document.service).onSave()
		compare(document.service.calls, [])

		answer(document.controller, true)
		compare(document.controller.updateCount, 2)
		compare(document.service.calls, [], "The queued edits are in flight")

		answer(document.controller, true)
		compare(document.service.calls, [["save", "doc", "Name"]])
	}

	function test_refusedUpdateCancelsPendingSave(){
		let document = openWaitingDocument()
		let failedSpy = createSpy(document.service, "saveDocumentFailed")
		edit(document.view)

		decoratorOf(document.service).onSave()
		answer(document.controller, false)
		wait(0)

		compare(failedSpy.count, 1)
		compare(failedSpy.signalArguments[0][0], "doc")
		compare(document.service.calls, [])
	}

	// An undo before the answer would undo the step before the edit, and the queued edit would bring the step back
	function test_undoAndRedoWaitForPendingUpdates(){
		let document = openDocument("Name", false)
		let service = document.service
		let controller = controllerOf(service)
		edit(document.views[0])

		decoratorOf(service).onUndo()
		decoratorOf(service).onRedo()
		compare(service.calls, [])

		answer(controller, true)
		compare(service.calls, [["undo", "doc", 1], ["redo", "doc", 1]])
	}

	function test_refusedUpdateCancelsPendingUndo(){
		let document = openDocument("Name", false)
		let service = document.service
		let controller = controllerOf(service)
		edit(document.views[0])

		decoratorOf(service).onUndo()
		answer(controller, false)
		wait(0)

		compare(service.calls, [])
	}

	function test_undoWithoutPendingUpdatesIsAtOnce(){
		let document = openDocument("Name", false)

		decoratorOf(document.service).onUndo()
		compare(document.service.calls, [["undo", "doc", 1]])
	}

	function test_saveWithoutPendingUpdatesIsAtOnce(){
		let document = openWaitingDocument()
		document.view.hasChanges = false

		decoratorOf(document.service).onSave()
		compare(document.service.calls, [["save", "doc", "Name"]])
	}

	// --- DocumentUpdateRequestSender answering the decorator

	function openGqlDocument(){
		GqlTestTransport.reset()
		GqlTestTransport.responseStatus = 200
		let service = createTemporaryObject(serviceComp, testCase)
		service.registerDocumentViewData("GqlType", viewTypeId, viewComp, gqlControllerComp)
		service.handleDocumentOpened("doc", "obj", "GqlType", "Name", false, false)
		let view = addView(service)
		service.setDocumentIsLoading("doc", false)
		controllerOf(service).respond()

		return {"service": service, "view": view, "controller": controllerOf(service)}
	}

	function test_refusalOfTheServerIsReadBackOnce(){
		let document = openGqlDocument()
		let failedSpy = createSpy(document.controller, "updateDocumentFailed")
		GqlTestTransport.responseBody = JSON.stringify({data: {UpdateDocumentFromRepresentation: {status: "Failed", message: "Name is taken"}}})

		edit(document.view)
		edit(document.view)
		tryCompare(failedSpy, "count", 1, 3000)
		wait(0)

		compare(failedSpy.signalArguments[0][1], "Name is taken")
		compare(GqlTestTransport.requestCount, 1, "The edit queued over the refused change is not sent")
		compare(document.controller.requestCount, 2, "The view is read again once")
	}

	function test_forbiddenUpdateReleasesTheView(){
		let document = openGqlDocument()
		let failedSpy = createSpy(document.controller, "updateDocumentFailed")
		GqlTestTransport.responseStatus = 403

		edit(document.view)
		tryCompare(failedSpy, "count", 1, 3000)
		wait(0)

		compare(document.controller.requestCount, 2)
		GqlTestTransport.responseStatus = 200
		GqlTestTransport.responseBody = JSON.stringify({data: {UpdateDocumentFromRepresentation: {status: "Success", message: ""}}})
		document.controller.respond()
		edit(document.view)
		compare(GqlTestTransport.requestCount, 2, "The next edit is sent at once, nothing waits for a lost answer")
	}

	function test_newModelDropsQueuedEdits(){
		let document = openDocument("Name", false)
		let view = document.views[0]
		let controller = controllerOf(document.service)
		edit(view)
		edit(view)

		let newModel = createTemporaryObject(modelComp, testCase)
		controller.updateRepresentationFromDocument()
		controller.representationModel = newModel
		controller.representationUpdated(controller.documentId, newModel)

		answer(controller, true)
		compare(controller.updateCount, 1)
	}

	// An edit made by the controller while it sends waits too, and the update keeps its own change-ID
	function test_editWhileSendingIsQueued(){
		let document = openDocument("Name", false)
		let controller = controllerOf(document.service)
		controller.editsWhileSending = true
		edit(document.views[0])

		compare(controller.updateCount, 1)
		compare(controller.changeIdWhileSending, controller.changeIds[0])

		answer(controller, true)
		compare(controller.updateCount, 2)
		compare(controller.lastChangeSet[0].path, ["id"])
	}

	function test_resyncWaitsForUpdateInFlight(){
		let document = openDocument("Name", false, 2)
		let service = document.service
		edit(document.views[0])

		service.updateDocumentRepresentation("doc")
		compare(controllerOf(service, 0).requestCount, 1, "The view with an update in flight is read after the answer")
		compare(controllerOf(service, 1).requestCount, 2)

		answer(controllerOf(service, 0), true)
		compare(controllerOf(service, 0).requestCount, 2)
	}

	function test_failedUpdateIsReadBack(){
		let document = openDocument("Name", false)
		let service = document.service
		let controller = controllerOf(service)
		controller.readBackOnRefusal = false
		edit(document.views[0])
		answer(controller, false)

		notifyChanged(service, ownChangeId(service, 0))
		compare(controller.requestCount, 2, "A refused update leaves what the server has, the view reads it")
	}

	// The server applied the change, then rolled it back without the change-ID: the rollback is read
	function test_rollbackAfterOwnWriteIsReadBack(){
		let document = openDocument("Name", false)
		let service = document.service
		let controller = controllerOf(service)
		controller.readBackOnRefusal = false
		edit(document.views[0])

		notifyChanged(service, ownChangeId(service, 0))
		compare(controller.requestCount, 1)

		notifyChanged(service, "")
		compare(controller.requestCount, 2)

		answer(controller, false)
		compare(controller.requestCount, 2)
	}

	function test_otherChangeDuringOwnUpdateIsReadAfterAnswer(){
		let document = openDocument("Name", false, 2)
		let service = document.service
		edit(document.views[0])

		notifyChanged(service, "")
		compare(controllerOf(service, 0).requestCount, 1, "Not read while the own update is in flight")
		compare(controllerOf(service, 1).requestCount, 2)

		answer(controllerOf(service, 0), true)
		compare(controllerOf(service, 0).requestCount, 2)
	}

	// The own notification shows that the server has the change: the put off read runs then
	function test_ownEchoReleasesReadPutOff(){
		let document = openDocument("Name", false)
		let service = document.service
		let controller = controllerOf(service)
		edit(document.views[0])

		notifyChanged(service, "")
		compare(controller.requestCount, 1)

		notifyChanged(service, ownChangeId(service, 0))
		compare(controller.requestCount, 2)
	}

	function test_requestRepresentationOnUpdateReadsOwnUpdate(){
		let document = openDocument("Name", false)
		let service = document.service
		let controller = controllerOf(service)
		controller.requestRepresentationOnUpdate = true
		edit(document.views[0])

		notifyChanged(service, ownChangeId(service, 0))
		compare(controller.requestCount, 2)
	}

	function test_lateOwnEchoIsReadOnce(){
		let document = openDocument("Name", false)
		let service = document.service
		let controller = controllerOf(service)
		decoratorOf(service).ownEchoTimeout = 50
		edit(document.views[0])
		answer(controller, true)
		wait(100)

		notifyChanged(service, ownChangeId(service, 0))
		compare(controller.requestCount, 2, "The update is forgotten, its late notification only reads the view once more")
	}

	function test_lostAnswerReleasesTheView(){
		let document = openDocument("Name", false)
		let service = document.service
		let controller = controllerOf(service)
		decoratorOf(service).ownUpdateTimeout = 100
		edit(document.views[0])

		notifyChanged(service, "")
		edit(document.views[0])
		compare(controller.requestCount, 1)
		compare(controller.updateCount, 1)

		tryCompare(controller, "requestCount", 2, 3000)
		compare(controller.updateCount, 2, "The queued edit is sent once the lost answer expires")
	}

	function test_updateOfOtherDocumentIsNotTracked(){
		let document = openDocument("Name", false)
		let service = document.service
		let controller = controllerOf(service)
		controller.startUpdateDocument("otherDoc")

		notifyChanged(service, "")
		compare(controller.requestCount, 2)
	}

	function test_unregisteredViewForgetsItsUpdates(){
		let document = openDocument("Name", false, 2)
		let service = document.service
		let secondController = controllerOf(service, 1)
		edit(document.views[0])
		let changeId = ownChangeId(service, 0)
		document.views[0].destroy()
		wait(0)

		notifyChanged(service, changeId)
		compare(secondController.requestCount, 2)
	}

	function test_notificationsForOtherDocumentsAreIgnored(){
		let document = openDocument("Name", false)

		document.service.documentManagerChanged(EDocumentOperationEnum.s_documentChanged, "other", "otherDoc", "Other", "")
		document.service.documentManagerChanged(EDocumentOperationEnum.s_documentSaved, "other", "otherDoc", "Other", "")

		compare(controllerOf(document.service).requestCount, 1)
		compare(document.views[0].savedCount, 0)
	}

	function test_documentSavedNotificationReachesViews(){
		let document = openDocument("Name", false, 2)

		document.service.documentManagerChanged(EDocumentOperationEnum.s_documentSaved, "obj", "doc", "Name", "")

		compare(document.views[0].savedCount, 1)
		compare(document.views[1].savedCount, 1)
	}

	function test_representationLoadDoesNotWriteBack(){
		let document = openDocument("Name", false)
		let controller = controllerOf(document.service)

		document.service.updateDocumentRepresentation("doc")
		document.views[0].model.modelChanged(null)

		compare(controller.updateCount, 0)

		controller.respond()
		document.views[0].model.modelChanged(null)

		compare(controller.updateCount, 1)
	}

	function test_changeSetReachesController(){
		let document = openDocument("Name", false)
		let controller = controllerOf(document.service)
		let changeSet = [{"path": ["permissions", 1, "direct"], "operation": "set", "name": "m_direct", "sender": null}]

		document.views[0].model.modelChanged(changeSet)

		compare(controller.updateCount, 1)
		compare(controller.lastChangeSet.length, 1)
		compare(controller.lastChangeSet[0].path, ["permissions", 1, "direct"])
		compare(controller.lastChangeSet[0].operation, "set")
	}

	function test_hiddenViewIsLoadedWhenShown(){
		let document = openDocument("Name", false)
		let view = document.views[0]
		let controller = controllerOf(document.service)

		view.visible = false
		document.service.documentManagerChanged(EDocumentOperationEnum.s_documentChanged, "obj", "doc", "Name", "")

		compare(controller.requestCount, 1)

		view.visible = true

		compare(controller.requestCount, 2)
	}

	function test_commitChanges(){
		let document = openDocument("Name", false, 2)

		document.views[1].visible = false
		document.service.commitDocumentChanges("doc")
		document.service.commitDocumentChanges("unknown")

		compare(controllerOf(document.service, 0).updateCount, 1)
		compare(controllerOf(document.service, 1).updateCount, 0, "Hidden views are not written")
		compare(document.service.calls, [])
	}
}
