import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtbaseImtCollectionSdl 1.0
import imtbaseCollectionDocumentManagerSdl 1.0

Item {
	id: workspaceView

	property CollectionView collectionView: null
	property DocumentManagerBase documentManager

	property ObjectVisualStatusProvider visualStatusProvider: null

	onCollectionViewChanged: {
		if (collectionView){
			collectionView.documentManager = documentManager
		}
	}

	NavigableItem {
		id: firstTabNavigation
		forwardRest: false
		onActivated: {
			if (restPath.length <= 1){
				tabView.currentIndex = 0
			}
		}
	}

	NavigableItem {
		id: navigableItem
		onActivated: {
			if (workspaceView.documentManager){
				if (restPath.length >= 1){
					let documentTypeId = matchedPath
					let documentId = restPath[0]
					workspaceView.documentManager.openDocument(documentId, documentTypeId)
				}
			}
		}
	}

	function updateDocumentName(objectId, documentId){
		if (!documentManager){
			console.error("Unable to update document name for '"+documentId+"'. Error: Document manager is invalid")
			return
		}

		if (!workspaceView.visualStatusProvider){
			console.error("Unable to update document name for '"+documentId+"'. Error: Visual status provider is invalid")
			return
		}

		let callbackOk = function(objectId2, icon, text, description){
			if (objectId2 === objectId){
				let documentName = text
				if (documentName === ""){
					documentName = workspaceView.documentManager.getDefaultDocumentName()
				}

				workspaceView.documentManager.setDocumentName(documentId, text)
				workspaceView.visualStatusProvider.visualStatusReceived.disconnect(callbackOk)
				workspaceView.visualStatusProvider.visualStatusReceiveFailed.disconnect(cbFailed)
			}
		}

		let cbFailed = function(objectId2, errorMessage){
			if (objectId2 === objectId){
				let defaultName = workspaceView.documentManager.getDefaultDocumentName()
				workspaceView.documentManager.setDocumentName(documentId, defaultName)
				workspaceView.visualStatusProvider.visualStatusReceived.disconnect(callbackOk)
				workspaceView.visualStatusProvider.visualStatusReceiveFailed.disconnect(cbFailed)
			}
		}

		if (objectId === ""){
			callbackOk("", "", "", "")
		}
		else{
			let documentTypeId = documentManager.getDocumentTypeId(documentId)
			workspaceView.visualStatusProvider.visualStatusReceived.connect(callbackOk)
			workspaceView.visualStatusProvider.visualStatusReceiveFailed.connect(cbFailed)
			workspaceView.visualStatusProvider.getVisualStatus(objectId, documentTypeId)
		}
	}

	function updateTabName(documentId){
		if (!documentManager){
			console.error("Unable to update tab name for document '"+documentId+"'. Error: Document manager is invalid")
			return
		}

		let documentName = documentManager.getDocumentName(documentId)
		if (documentName === ""){
			documentName = documentManager.getDefaultDocumentName()
		}

		let isDirty = documentManager.documentIsDirty(documentId)
		if (isDirty){
			tabView.setTabName(documentId, "* " + documentName)
		}
		else{
			tabView.setTabName(documentId, documentName)
		}
	}

	Connections {
		id: connections
		target: workspaceView.documentManager

		function onStartGetOpenedDocumentList(){
			loading.start()
		}

		function onOpenedDocumentListReceived(documentInfoList){
			for (let i = 0; i < documentInfoList.count; ++i){
				let documentInfo = documentInfoList.get(i).item
				let objectId = documentInfo.m_objectId
				let documentId = documentInfo.m_documentId
				let objectTypeId = documentInfo.m_objectTypeId
				let hasChanges = documentInfo.m_hasChanges

				if (objectId === ""){
					workspaceView.documentManager.documentCreated(documentId, objectTypeId)
				}
				else{
					workspaceView.updateDocumentName(objectId, documentId)
					workspaceView.documentManager.documentOpened(documentId, objectTypeId)
				}

				workspaceView.documentManager.getUndoInfo(documentId)
				workspaceView.documentManager.documentManagerChanged(EDocumentOperationEnum.s_documentChanged, objectId, documentId, hasChanges)
			}

			loading.stop()
		}

		function onDocumentGuiUpdated(documentId, representation){
			loading.stop()
		}

		function onDocumentRepresentationUpdated(documentId, representation){
			// loading.stop()
		}

		function onDocumentManagerChanged(typeOperation, objectId, documentId, hasChanges){
			if (typeOperation === EDocumentOperationEnum.s_documentClosed){
				tabView.removeTab(documentId)
			}
			else if (typeOperation === EDocumentOperationEnum.s_documentOpened || typeOperation === EDocumentOperationEnum.s_documentSaved){
				workspaceView.updateDocumentName(objectId, documentId)
			}
		}

		function onDocumentNameChanged(documentId, oldName, newName){
			workspaceView.updateTabName(documentId)
		}

		function onDocumentIsDirtyChanged(documentId, isDirty){
			workspaceView.updateTabName(documentId)
		}

		// Open document signals
		function onStartOpenDocument(documentId, typeId){
			loading.start()
		}

		function onDocumentOpened(documentId, typeId){
			// loading.stop()

			let documentEditorComp = workspaceView.documentManager.getDocumentEditorFactory(typeId)
			tabView.addTab(documentId, "", documentEditorComp, "", "", true)
			tabView.currentIndex = tabView.tabModel.count - 1
		}

		function onOpenDocumentFailed(documentId, message){
			loading.stop()
			ModalDialogManager.showErrorDialog(message)
		}

		// Close document signals
		function onStartCloseDocument(documentId){
			loading.start()
		}

		function onDocumentClosed(documentId){
			tabView.removeTab(documentId)
			loading.stop()
		}

		function onCloseDocumentFailed(documentId, message){
			loading.stop()
			ModalDialogManager.showErrorDialog(message)
		}

		// Save document signals
		function onStartSaveDocument(documentId){
			loading.start()
		}

		function onDocumentSaved(documentId){
			loading.stop()
		}

		function onSaveDocumentFailed(documentId, message){
			loading.stop()
			ModalDialogManager.showErrorDialog(message)
		}

		// Create document signals
		function onStartCreateDocument(documentTypeId){
			loading.start()
		}

		function onCreateDocumentFailed(documentTypeId, message){
			loading.stop()
		}

		function onDocumentCreated(documentId, documentTypeId){
			let documentEditorComp = workspaceView.documentManager.getDocumentEditorFactory(documentTypeId)
			let defaultName = workspaceView.documentManager.getDefaultDocumentName()
			workspaceView.documentManager.setDocumentName(documentId, defaultName)
			tabView.addTab(documentId, defaultName, documentEditorComp, "", "", false)

			tabView.currentIndex = tabView.tabModel.count - 1

			loading.stop()
		}

		// Undo info signals
		function onStartUndoInfoReceive(documentId){
			// loading.start()
		}

		function onUndoInfoReceived(documentId, availableUndoSteps, availableRedoSteps){
			// loading.stop()
		}

		function onUndoInfoReceiveFailed(documentId, message){
			loading.stop()
			ModalDialogManager.showErrorDialog(message)
		}

		// Undo signals
		function onStartUndo(documentId, steps){
			// loading.start()
		}

		function onUndoDone(documentId){
			// loading.stop()
		}

		function onUndoFailed(documentId, message){
			loading.stop()
			ModalDialogManager.showErrorDialog(message)
		}

		// Redo signals
		function onStartRedo(documentId, steps){
			// loading.start()
		}

		function onRedoDone(documentId){
			// loading.stop()
		}

		function onRedoFailed(documentId, message){
			loading.stop()
			ModalDialogManager.showErrorDialog(message)
		}

		function onTryCloseDirtyDocument(documentId, callback){
			if (!workspaceView.documentManager.documentIsDirty(documentId)){
				callback(false)
				return
			}

			let dialogCallback = function(result){
				if (result === Enums.yes){
					callback(true)
				}
				else if (result === Enums.no){
					callback(false)
				}
				else{
					callback(undefined)
				}
			}

			ModalDialogManager.showConfirmationDialog(
						qsTr("Save document"),
						qsTr("Save all changes ?"),
						dialogCallback)
		}
	}

	function setCollectionViewComp(name, collectionViewComp){
		tabView.addTab(UuidGenerator.generateUUID(), name, collectionViewComp)
		tabView.currentIndex = 0
	}

	Rectangle {
		anchors.fill: parent
		color: Style.backgroundColor2
	}

	Component {
		id: popupMenuDialog

		PopupMenuDialog {
			itemWidth: Style.sizeHintXXS
			onFinished: {
				if (commandId === "Close"){
					if (tabView.currentIndex > 0){
						let tabId = tabView.getTabIdByIndex(tabView.currentIndex)
						workspaceView.documentManager.closeDocument(tabId)
					}
				}
				else if (commandId === "CloseAll"){
					let documentIds = workspaceView.documentManager.getOpenedDocumentIds()

					for (let i = 0; i < documentIds.length; i++){
						workspaceView.documentManager.closeDocument(documentIds[i])
					}
				}
			}
		}
	}

	LocalizationEvent {
		onLocalizationChanged: {
			tabContextMenuModel.fillModel()
		}
	}

	PopupMenuModel {
		id: tabContextMenuModel
		Component.onCompleted: {
			fillModel()
		}

		function fillModel(){
			tabContextMenuModel.clear()
			tabContextMenuModel.addItem("Close", qsTr("Close document"), "", true)
			tabContextMenuModel.addItem("CloseAll", qsTr("Close all documents"), "", true)
		}
	}

	TabView {
		id: tabView
		anchors.fill: parent
		closable: true

		onTabLoaded: {
			if (index === 0){
				workspaceView.collectionView = tabItem
			}
			else{
				workspaceView.documentManager.onViewInstanceCreated(tabId, tabItem)
			}
		}

		onTabClicked: {
			if (mouse.button === Qt.RightButton && index != 0){
				var point = tabItem.mapToItem(this, 0, 0)
				ModalDialogManager.openDialog(popupMenuDialog, {"x": point.x + Style.sizeHintXXS, "y": point.y, "model": tabContextMenuModel})
			}
		}

		function onCloseTab(index){
			let tabId = getTabIdByIndex(index)
			workspaceView.documentManager.closeDocument(tabId)
		}
	}

	Loading {
		id: loading
		anchors.fill: parent
		visible: false
	}
}
