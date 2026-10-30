import QtQuick 2.12
import QtTest 1.12
import imtgui 1.0
import imtdocgui 1.0
import imtguigql 1.0
import imtqmltest 1.0

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

			property bool accepts: true

			function updateModel(){
				model.modelChanged(null)
			}

			function acceptsSave(){
				return accepts
			}
		}
	}

	Component {
		id: controllerComp

		DocumentRepresentationController {
			property int updateCount: 0
			property int requestCount: 0

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

	Component {
		id: spyComp

		SignalSpy {
		}
	}

	function init(){
		GqlTestTransport.reset()
	}

	function undoInfoResponse(documentId, status, isDirty){
		return JSON.stringify({"data": {"GetUndoInfo": {
			"documentId": documentId,
			"status": {"status": status},
			"isDirty": isDirty,
			"availableUndoSteps": isDirty ? 1 : 0,
			"availableRedoSteps": 0
		}}})
	}

	function documentListResponse(documents){
		return JSON.stringify({"data": {"GetOpenedDocumentList": {"documentList": documents}}})
	}

	function test_resyncClosesDocumentTheServerNoLongerHas(){
		let service = openDocument(true)
		let lostSpy = createTemporaryObject(spyComp, testCase, {"target": service, "signalName": "documentLost"})
		GqlTestTransport.queueResponse("GetUndoInfo", undoInfoResponse("doc", "InvalidDocumentId", false))
		GqlTestTransport.queueResponse("GetOpenedDocumentList", documentListResponse([]))

		service.synchronizeDocuments()

		tryVerify(function(){ return !service.documentIsOpened("doc") }, 5000, "The lost document is closed")
		compare(lostSpy.count, 1)
		compare(Array.prototype.slice.call(lostSpy.signalArguments[0]), ["doc", "Name"])
		compare(service.calls, [], "Closed locally, no CloseDocument for a document the server does not have")
		verify(GqlTestTransport.requestedCommands().indexOf("GetOpenedDocumentList") >= 0)
	}

	function test_resyncReloadsDocumentTheServerStillHas(){
		let service = openDocument(true)
		let controller = controllerOf(service)
		let requestsBefore = controller.requestCount
		GqlTestTransport.queueResponse("GetUndoInfo", undoInfoResponse("doc", "Success", false))
		GqlTestTransport.queueResponse("GetOpenedDocumentList", documentListResponse([
			{"documentId": "doc", "documentName": "Name", "objectTypeId": typeId, "objectId": "obj", "isDirty": false, "hasNameProvider": false, "isLoading": false}
		]))

		service.synchronizeDocuments()

		tryCompare(controller, "requestCount", requestsBefore + 1, 5000, "The representation is read again")
		verify(service.documentIsOpened("doc"))
		verify(!service.documentIsDirty("doc"), "The dirty flag follows the server")
	}

	function test_plainUndoInfoDoesNotReloadViews(){
		let service = openDocument(true)
		let controller = controllerOf(service)
		let requestsBefore = controller.requestCount
		GqlTestTransport.queueResponse("GetUndoInfo", undoInfoResponse("doc", "Success", true))

		service.getUndoInfo("doc")

		tryVerify(function(){ return GqlTestTransport.requestCount === 1 }, 5000)
		wait(50)
		compare(controller.requestCount, requestsBefore, "A plain undo info request does not reload the views")
	}

	function test_saveOnCloseWritesViewsFirst(){
		let service = openDocument(true)
		let controller = controllerOf(service)

		service.closeDocument("doc")

		compare(controller.updateCount, 1, "The GUI state is written to the document before saving")
		compare(service.calls, [["save", "doc"]])

		service.handleSaveDocumentResult("doc", "Success", "", "")

		compare(service.calls, [["save", "doc"], ["close", "doc"]])
	}

	function test_failedSaveKeepsDocumentOpen(){
		let service = openDocument(true)

		service.closeDocument("doc")
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

	// A forced close is the one of an editor taken down: there is nobody to ask
	function test_forcedCloseDoesNotAsk(){
		let service = openDocument(undefined)

		service.closeDocument("doc", true)

		compare(controllerOf(service).updateCount, 0, "Nothing is saved")
		compare(service.calls, [["close", "doc"]])
	}

	// The answers of tryCloseDocument, collected
	function tryClose(service){
		let answers = []
		service.tryCloseDocument("doc", function(closed){
			answers.push(closed)
		})
		return answers
	}

	function test_tryCloseAnswersOnceClosed(){
		let service = openDocument(false)

		let answers = tryClose(service)
		compare(answers, [], "No answer before the server closed it")

		service.handleCloseDocumentResult("doc", "Success")
		compare(answers, [true])
	}

	function test_tryCloseAnswersOnTheCloseAfterTheSave(){
		let service = openDocument(true)

		let answers = tryClose(service)
		service.handleSaveDocumentResult("doc", "Success", "", "")
		compare(answers, [], "Saved, the close is still on the way")

		service.handleCloseDocumentResult("doc", "Success")
		compare(answers, [true])
	}

	// A later close of the same document is not taken for the answer of the cancelled one
	function test_tryCloseCancelledAnswersOnce(){
		let service = openDocument(undefined)

		let answers = tryClose(service)
		compare(answers, [false])

		service.handleCloseDocumentResult("doc", "Success")
		compare(answers, [false])
	}

	function test_tryCloseKeepsDocumentWhenSaveFails(){
		let service = openDocument(true)

		let answers = tryClose(service)
		service.handleSaveDocumentResult("doc", "Failed", "Disk full", "")

		compare(answers, [false])
		compare(service.calls, [["save", "doc"]])
		verify(service.documentIsOpened("doc"))
	}

	function test_tryCloseKeepsDocumentWhenTheViewRefusesTheSave(){
		let service = openDocument(true)
		let index = service.getDocumentIndexByDocumentId("doc")
		service.__internal.openedDocuments[index].documentDecorator.registeredViews[0].accepts = false

		let answers = tryClose(service)

		compare(answers, [false])
		compare(service.calls, [])
	}

	function test_tryCloseKeepsDocumentWhenTheCloseFails(){
		let service = openDocument(false)

		let answers = tryClose(service)
		service.handleCloseDocumentResult("doc", "Failed")

		compare(answers, [false])
	}

	function test_tryCloseOfANotOpenedDocument(){
		let service = createTemporaryObject(serviceComp, testCase)
		let answers = []

		service.tryCloseDocument("doc", function(closed){
			answers.push(closed)
		})

		compare(answers, [true])
		compare(service.calls, [])
	}
}
