import QtQuick
import imtcontrols 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtbaseCollectionDocumentServiceSdl 1.0

// Own updates of a document view and the notifications of the server, the same in Qt and on the web
Item {
	id: root

	width: 100
	height: 100

	property int failures: 0

	Component {
		id: serviceComp

		DocumentServiceBase {}
	}

	Component {
		id: viewComp

		ViewBase {
			width: 10
			height: 10

			// The base one logs with console.debug, which only the desktop prints
			function doUpdateGui(){
			}
		}
	}

	Component {
		id: controllerComp

		DocumentRepresentationController {
			property int requestCount: 0
			property var changeIds: []
			property int updateCount: 0

			representationModel: QtObject {
				signal modelChanged(var changeSet)
			}

			function updateRepresentationFromDocument(){
				requestCount = requestCount + 1
				startUpdateRepresentation(documentId, representationModel)
			}

			function updateDocumentFromRepresentation(changeSet){
				updateCount = updateCount + 1
				let ids = changeIds
				ids.push(changeId)
				changeIds = ids
				startUpdateDocument(documentId)
			}

			function respond(){
				representationUpdated(documentId, representationModel)
			}

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

	function check(name, actual, expected){
		if (actual === expected){
			console.log(name + " ok " + actual)
		}
		else{
			root.failures = root.failures + 1
			console.log(name + " MISMATCH " + actual + " expected " + expected)
		}
	}

	function openDocument(viewCount){
		let service = serviceComp.createObject(root)
		service.registerDocumentViewData("TestType", "TestView", viewComp, controllerComp)
		service.handleDocumentOpened("doc", "obj", "TestType", "Name", false, false)

		let views = []
		for (let i = 0; i < viewCount; ++i){
			let view = viewComp.createObject(root)
			service.onViewInstanceCreated("doc", view, "TestView")
			views.push(view)
		}

		service.setDocumentIsLoading("doc", false)
		for (let i = 0; i < viewCount; ++i){
			root.controllerOf(service, i).respond()
		}

		return {"service": service, "views": views}
	}

	function controllerOf(service, viewIndex){
		let index = service.getDocumentIndexByDocumentId("doc")

		return service.__internal.openedDocuments[index].documentDecorator.registeredRepresentation[viewIndex]
	}

	function notifyChanged(service, changeId){
		service.documentManagerChanged(EDocumentOperationEnum.s_documentChanged, "obj", "doc", "Name", changeId)
	}

	function ownChangeId(service, viewIndex){
		let changeIds = root.controllerOf(service, viewIndex).changeIds

		return changeIds[changeIds.length - 1]
	}

	function edit(view){
		view.model.modelChanged([])
	}

	Component.onCompleted: {
		let document = root.openDocument(2)
		let service = document.service
		root.edit(document.views[0])
		root.check("changeIdSent", root.ownChangeId(service, 0) !== "" && root.controllerOf(service, 0).changeId === "", true)
		root.controllerOf(service, 0).answer(true)
		root.notifyChanged(service, root.ownChangeId(service, 0))
		root.check("ownEcho.own", root.controllerOf(service, 0).requestCount, 1)
		root.check("ownEcho.other", root.controllerOf(service, 1).requestCount, 2)
		root.notifyChanged(service, "")
		root.check("ownEcho.next", root.controllerOf(service, 0).requestCount, 2)

		document = root.openDocument(1)
		service = document.service
		root.edit(document.views[0])
		root.controllerOf(service, 0).answer(true)
		root.notifyChanged(service, "change-of-another-client")
		root.check("otherWhileAwaited", root.controllerOf(service, 0).requestCount, 2)

		document = root.openDocument(1)
		service = document.service
		root.controllerOf(service, 0).requestRepresentationOnUpdate = true
		root.edit(document.views[0])
		root.notifyChanged(service, root.ownChangeId(service, 0))
		root.check("requestOnUpdate", root.controllerOf(service, 0).requestCount, 2)

		root.startAnsweredSteps()
	}

	// The decorator goes on after an answer is handled: each step checks the previous answer one tick later
	property int step: 0
	property var stepDocument: null

	function startAnsweredSteps(){
		stepDocument = root.openDocument(1)
		root.edit(stepDocument.views[0])
		root.edit(stepDocument.views[0])
		root.edit(stepDocument.views[0])
		root.check("queue.oneInFlight", root.controllerOf(stepDocument.service, 0).updateCount, 1)
		root.controllerOf(stepDocument.service, 0).answer(true)
		root.check("queue.notFromTheAnswer", root.controllerOf(stepDocument.service, 0).updateCount, 1)
		stepTimer.start()
	}

	function runStep(){
		step++
		let service = stepDocument.service
		let controller = root.controllerOf(service, 0)
		if (step === 1){
			root.check("queue.sentAfterAnswer", controller.updateCount, 2)
			root.notifyChanged(service, controller.changeIds[1])
			root.notifyChanged(service, controller.changeIds[0])
			root.check("queue.echoes", controller.requestCount, 1)

			stepDocument = root.openDocument(1)
			root.controllerOf(stepDocument.service, 0).readBackOnRefusal = false
			root.edit(stepDocument.views[0])
			root.controllerOf(stepDocument.service, 0).answer(false)
			stepTimer.start()
		}
		else if (step === 2){
			root.notifyChanged(service, root.ownChangeId(service, 0))
			root.check("failedIsReadBack", controller.requestCount, 2)

			stepDocument = root.openDocument(1)
			root.edit(stepDocument.views[0])
			root.edit(stepDocument.views[0])
			root.controllerOf(stepDocument.service, 0).answer(false)
			stepTimer.start()
		}
		else if (step === 3){
			root.check("refusal.readBack", controller.requestCount, 2)
			root.check("refusal.queueDropped", controller.updateCount, 1)

			stepDocument = root.openDocument(1)
			root.edit(stepDocument.views[0])
			root.notifyChanged(stepDocument.service, "")
			root.check("deferred.inFlight", root.controllerOf(stepDocument.service, 0).requestCount, 1)
			root.controllerOf(stepDocument.service, 0).answer(true)
			stepTimer.start()
		}
		else{
			root.check("deferred.afterAnswer", controller.requestCount, 2)

			console.log("failures " + root.failures)
			Qt.quit()
		}
	}

	Timer {
		id: stepTimer

		interval: 10
		repeat: false
		onTriggered: {
			root.runStep()
		}
	}
}
