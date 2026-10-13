import QtQuick 2.12
import QtTest 1.12
import imtgui 1.0
import imtdocgui 1.0
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

	Component {
		id: viewComp

		ViewBase {
			width: 10
			height: 10

			property bool hasChanges: true
			property int savedCount: 0

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

			representationModel: QtObject {
				signal modelChanged(var changeSet)
			}

			function updateRepresentationFromDocument(){
				requestCount++
				startUpdateRepresentation(documentId, representationModel)
			}

			function updateDocumentFromRepresentation(){
				updateCount++
				startUpdateDocument(documentId)
			}

			function respond(){
				representationUpdated(documentId, representationModel)
			}

			function finishUpdate(){
				documentUpdated(documentId)
			}

			function failUpdate(){
				updateDocumentFailed(documentId, "Update failed")
			}
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

	function test_saveFlushesGuiAndWaitsForUpdate(){
		let document = openDocument("Name", false)
		let controller = controllerOf(document.service)

		document.views[0].commandActivated("Save")

		compare(controller.updateCount, 1, "Save first writes the GUI state to the document")
		compare(document.service.calls, [], "SaveDocument must not overtake the update")

		controller.finishUpdate()

		compare(document.service.calls, [["save", "doc", "Name"]])
	}

	function test_saveWaitsForEarlierEdit(){
		let document = openDocument("Name", false)
		let view = document.views[0]
		let controller = controllerOf(document.service)

		view.model.modelChanged(null)
		view.hasChanges = false
		view.commandActivated("Save")

		compare(controller.updateCount, 1)
		compare(document.service.calls, [])

		controller.finishUpdate()

		compare(document.service.calls, [["save", "doc", "Name"]])
	}

	function test_saveWaitsForAllUpdates(){
		let document = openDocument("Name", false, 2)

		document.views[0].commandActivated("Save")

		compare(controllerOf(document.service, 0).updateCount, 1)
		compare(controllerOf(document.service, 1).updateCount, 1)

		controllerOf(document.service, 0).finishUpdate()

		compare(document.service.calls, [])

		controllerOf(document.service, 1).finishUpdate()

		compare(document.service.calls, [["save", "doc", "Name"]])
	}

	function test_failedUpdateCancelsSave(){
		let document = openDocument("Name", false)
		let failedSpy = createSpy(document.service, "updateDocumentFailed")
		let controller = controllerOf(document.service)

		document.views[0].commandActivated("Save")
		controller.failUpdate()

		compare(failedSpy.count, 1)
		compare(document.service.calls, [])

		document.views[0].hasChanges = false
		document.views[0].commandActivated("Save")

		compare(document.service.calls, [["save", "doc", "Name"]], "The next Save works normally")
	}

	function test_repeatedSaveDoesNotWaitAgain(){
		let document = openDocument("Name", false)

		document.views[0].commandActivated("Save")
		document.views[0].commandActivated("Save")

		compare(document.service.calls, [["save", "doc", "Name"]])

		controllerOf(document.service).finishUpdate()

		compare(document.service.calls.length, 1, "A late update reply does not save again")
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

		document.views[0].model.modelChanged(null)
		service.documentManagerChanged(EDocumentOperationEnum.s_documentChanged, "obj", "doc", "Name")

		compare(controllerOf(service, 0).requestCount, 1, "The view that made the change is not reloaded")
		compare(controllerOf(service, 1).requestCount, 2)

		service.documentManagerChanged(EDocumentOperationEnum.s_documentChanged, "obj", "doc", "Name")

		compare(controllerOf(service, 0).requestCount, 2)
		compare(controllerOf(service, 1).requestCount, 3)
	}

	function test_notificationsForOtherDocumentsAreIgnored(){
		let document = openDocument("Name", false)

		document.service.documentManagerChanged(EDocumentOperationEnum.s_documentChanged, "other", "otherDoc", "Other")
		document.service.documentManagerChanged(EDocumentOperationEnum.s_documentSaved, "other", "otherDoc", "Other")

		compare(controllerOf(document.service).requestCount, 1)
		compare(document.views[0].savedCount, 0)
	}

	function test_documentSavedNotificationReachesViews(){
		let document = openDocument("Name", false, 2)

		document.service.documentManagerChanged(EDocumentOperationEnum.s_documentSaved, "obj", "doc", "Name")

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

	function test_hiddenViewIsLoadedWhenShown(){
		let document = openDocument("Name", false)
		let view = document.views[0]
		let controller = controllerOf(document.service)

		view.visible = false
		document.service.documentManagerChanged(EDocumentOperationEnum.s_documentChanged, "obj", "doc", "Name")

		compare(controller.requestCount, 1)

		view.visible = true

		compare(controller.requestCount, 2)
	}

	function test_commitChanges(){
		let document = openDocument("Name", false)
		let controller = controllerOf(document.service)
		let results = []

		document.service.commitDocumentChanges("doc", function(committed){ results.push(committed) })

		compare(controller.updateCount, 1)
		compare(results, [])

		controller.finishUpdate()

		compare(results, [true])

		document.service.commitDocumentChanges("doc", function(committed){ results.push(committed) })
		controller.failUpdate()

		compare(results, [true, false])

		document.views[0].hasChanges = false
		document.service.commitDocumentChanges("doc", function(committed){ results.push(committed) })
		document.service.commitDocumentChanges("unknown", function(committed){ results.push(committed) })

		compare(results, [true, false, true, true], "Nothing to wait for")
	}
}
