import QtQuick 2.12
import QtTest 1.12
import imtgui 1.0
import imtdocgui 1.0
import imtguigql 1.0

TestCase {
	id: testCase

	name: "GqlBasedCollectionDocumentService"
	when: windowShown

	property string typeId: "TestType"
	property string viewTypeId: "TestView"

	Component {
		id: serviceComp

		GqlBasedCollectionDocumentService {
			property var calls: []
			property var closeAnswer

			saveDocumentOverride: function(documentId, documentName){
				calls.push(["save", documentId])
			}

			closeDocumentOverride: function(documentId){
				calls.push(["close", documentId])
			}

			onTryCloseDirtyDocument: {
				callback(closeAnswer)
			}
		}
	}

	Component {
		id: viewComp

		ViewBase {
			width: 10
			height: 10

			function updateModel(){
				model.modelChanged(null)
			}
		}
	}

	Component {
		id: controllerComp

		DocumentRepresentationController {
			property int updateCount: 0

			representationModel: QtObject {
				signal modelChanged(var changeSet)
			}

			function updateRepresentationFromDocument(){
				startUpdateRepresentation(documentId, representationModel)
			}

			function updateDocumentFromRepresentation(){
				updateCount++
				startUpdateDocument(documentId)
			}

			function respond(){
				representationUpdated(documentId, representationModel)
			}
		}
	}

	// Opens a dirty document whose single view is loaded and accepts edits.
	function openDocument(closeAnswer){
		let service = createTemporaryObject(serviceComp, testCase, {"closeAnswer": closeAnswer})
		service.registerDocumentViewData(typeId, viewTypeId, viewComp, controllerComp)
		service.handleDocumentOpened("doc", "obj", typeId, "Name", false, true)

		// TestCase itself is invisible, views need a visible parent
		let view = createTemporaryObject(viewComp, testCase.parent)
		service.onViewInstanceCreated("doc", view, viewTypeId)
		service.setDocumentIsLoading("doc", false)
		controllerOf(service).respond()

		return service
	}

	function controllerOf(service){
		let index = service.getDocumentIndexByDocumentId("doc")

		return service.__internal.openedDocuments[index].documentDecorator.registeredRepresentation[0]
	}

	function test_saveOnCloseWaitsForViewUpdates(){
		let service = openDocument(true)
		let controller = controllerOf(service)

		service.closeDocument("doc")

		compare(controller.updateCount, 1, "The GUI state is written to the document before saving")
		compare(service.calls, [], "SaveDocument must not overtake the update")

		controller.documentUpdated("doc")

		compare(service.calls, [["save", "doc"]])

		service.handleSaveDocumentResult("doc", "Success", "", "")

		compare(service.calls, [["save", "doc"], ["close", "doc"]])
	}

	function test_failedUpdateKeepsDocumentOpen(){
		let service = openDocument(true)
		let controller = controllerOf(service)

		service.closeDocument("doc")
		controller.updateDocumentFailed("doc", "Update failed")

		compare(service.calls, [])

		// A later regular save must not close the document
		service.handleSaveDocumentResult("doc", "Success", "", "")

		compare(service.calls, [])
	}

	function test_failedSaveKeepsDocumentOpen(){
		let service = openDocument(true)

		service.closeDocument("doc")
		controllerOf(service).documentUpdated("doc")
		service.handleSaveDocumentResult("doc", "Failed", "Disk full", "")
		service.handleSaveDocumentResult("doc", "Success", "", "")

		compare(service.calls, [["save", "doc"]])
	}

	function test_closeWithoutSaving(){
		let service = openDocument(false)

		service.closeDocument("doc")

		compare(controllerOf(service).updateCount, 0)
		compare(service.calls, [["close", "doc"]])
	}

	function test_cancelledClose(){
		let service = openDocument(undefined)

		service.closeDocument("doc")

		compare(service.calls, [])
		verify(service.documentIsOpened("doc"))
	}

	function test_cleanDocumentClosesImmediately(){
		let service = openDocument(true)
		service.setDocumentIsDirty("doc", false)

		service.closeDocument("doc")

		compare(service.calls, [["close", "doc"]])
	}
}
