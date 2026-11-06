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
			// false: the update tells no source, e.g. a controller that does not return one
			property bool hasSource: true
			property string sentSourceId: ""

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
				sentSourceId = hasSource ? documentId + "-update-" + updateCount + "-" + Math.random() : ""

				return sentSourceId
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

	function notifyChanged(service, sourceId, changeIds){
		service.documentManagerChanged(EDocumentOperationEnum.s_documentChanged, "obj", "doc", "Name", sourceId, changeIds)
	}

	function test_ownChangeIsNotReadBack(){
		let document = openDocument("Name", false, 2)
		let service = document.service

		document.views[0].model.modelChanged(null)
		let sourceId = controllerOf(service, 0).sentSourceId
		notifyChanged(service, sourceId, [])

		compare(controllerOf(service, 0).requestCount, 1, "The view whose update made the change is not read")
		compare(controllerOf(service, 1).requestCount, 2, "The change of another view is read")

		notifyChanged(service, sourceId, [])

		compare(controllerOf(service, 0).requestCount, 1, "Every change of the own update is own")
	}

	function test_foreignChangeIsRead(){
		let document = openDocument("Name", false, 2)
		let service = document.service

		document.views[0].model.modelChanged(null)
		notifyChanged(service, "", [])

		compare(controllerOf(service, 0).requestCount, 2, "A change without a source is read by every view")
		compare(controllerOf(service, 1).requestCount, 2)

		notifyChanged(service, "1b4e28ba-2fa1-11d2-883f-0016d3cca427", [])

		compare(controllerOf(service, 0).requestCount, 3, "A change of another session is read")
		compare(controllerOf(service, 1).requestCount, 3)
	}

	function test_updateWithoutSourceIsNotOwn(){
		let document = openDocument("Name", false)
		let service = document.service
		let controller = controllerOf(service)
		controller.hasSource = false

		document.views[0].model.modelChanged(null)
		notifyChanged(service, "1b4e28ba-2fa1-11d2-883f-0016d3cca427", [])

		compare(controller.requestCount, 2, "A controller that returns no source has no own changes")
	}

	function test_observedChangeIds(){
		let document = openDocument("Name", false, 3)
		let service = document.service
		controllerOf(service, 0).observedChangeIds = ["licenses"]
		controllerOf(service, 1).observedChangeIds = ["name"]

		notifyChanged(service, "", ["name", "comments"])

		compare(controllerOf(service, 0).requestCount, 1, "A change of parts the controller does not observe is not read")
		compare(controllerOf(service, 1).requestCount, 2, "A change of an observed part is read")
		compare(controllerOf(service, 2).requestCount, 2, "A controller without observedChangeIds reads every change")

		notifyChanged(service, "", [])

		compare(controllerOf(service, 0).requestCount, 2, "Unknown changeIds are read by every controller")
	}

	function test_ownChangeIsNotReadEvenIfObserved(){
		let document = openDocument("Name", false)
		let service = document.service
		let controller = controllerOf(service)
		controller.observedChangeIds = ["name"]

		document.views[0].model.modelChanged(null)
		notifyChanged(service, controller.sentSourceId, ["name"])

		compare(controller.requestCount, 1)
	}

	function test_eachViewKeepsItsLastSources(){
		let document = openDocument("Name", false, 2)
		let service = document.service

		document.views[0].model.modelChanged(null)
		let firstSourceId = controllerOf(service, 0).sentSourceId
		for (let i = 0; i < 16; ++i){
			document.views[1].model.modelChanged(null)
		}

		notifyChanged(service, firstSourceId, [])

		compare(controllerOf(service, 0).requestCount, 1, "Updates of another view do not push out the sources of this one")

		for (let i = 0; i < 16; ++i){
			document.views[0].model.modelChanged(null)
		}

		notifyChanged(service, firstSourceId, [])

		compare(controllerOf(service, 0).requestCount, 2, "Only the last 16 sources of a view are kept")
	}

	function test_failedViewIsReadOnAnyChange(){
		let document = openDocument("Name", false)
		let service = document.service
		let controller = controllerOf(service)
		controller.observedChangeIds = ["licenses"]

		service.updateDocumentRepresentation("doc")
		controller.updateRepresentationFailed("doc", "Error")
		notifyChanged(service, "", ["name"])

		compare(controller.requestCount, 3, "A view whose read failed is read again")
	}

	function test_notificationsForOtherDocumentsAreIgnored(){
		let document = openDocument("Name", false)

		document.service.documentManagerChanged(EDocumentOperationEnum.s_documentChanged, "other", "otherDoc", "Other", "", [])
		document.service.documentManagerChanged(EDocumentOperationEnum.s_documentSaved, "other", "otherDoc", "Other", "", [])

		compare(controllerOf(document.service).requestCount, 1)
		compare(document.views[0].savedCount, 0)
	}

	function test_documentSavedNotificationReachesViews(){
		let document = openDocument("Name", false, 2)

		document.service.documentManagerChanged(EDocumentOperationEnum.s_documentSaved, "obj", "doc", "Name", "", [])

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
		notifyChanged(document.service, "", [])

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
