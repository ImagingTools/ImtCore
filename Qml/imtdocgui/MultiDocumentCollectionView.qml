import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtbaseImtCollectionSdl 1.0

Item {
	id: workspaceView

	property CollectionView collectionView: null
	property DocumentManagerBase documentManager

	property ObjectVisualStatusProvider visualStatusProvider: null

	Connections {
		target: workspaceView.visualStatusProvider ? workspaceView.visualStatusProvider : undefined
		
		function onVisualStatusReceived(objectId, icon, text, description){
			let name = text
			if (name === ""){
				name = workspaceView.documentManager.defaultDocumentName
			}

			tabView.setTabName(objectId, name)
			tabView.setTabDescription(objectId, description)
			tabView.setTabIcon(objectId, icon)
		}
		
		function onVisualStatusReceiveFailed(objectId, errorMessage){
			tabView.setTabName(objectId, workspaceView.documentManager.defaultDocumentName)
			tabView.setTabDescription(objectId, "")
			tabView.setTabIcon(objectId, "")
		}
	}

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

	Connections {
		id: connections
		target: workspaceView.documentManager

		// Open document signals
		function onStartOpenDocument(documentId, typeId){
			loading.start()

			if (workspaceView.visualStatusProvider){
				workspaceView.visualStatusProvider.getVisualStatus(documentId, typeId)
			}
		}

		function onDocumentOpened(documentId, typeId){
			loading.stop()

			let documentEditorComp = workspaceView.documentManager.getDocumentEditorFactory(typeId)
			let defaultName = workspaceView.documentManager.getDefaultDocumentName()
			tabView.addTab(documentId, "no name", documentEditorComp, "", "", false)

			let view = tabView.getTabById(documentId)

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
			if (workspaceView.visualStatusProvider){
				let typeId = workspaceView.documentManager.getDocumentTypeId(documentId)
				workspaceView.visualStatusProvider.getVisualStatus(documentId, typeId)
			}

			loading.stop()
			workspaceView.documentManager.documentModelChanged(documentId)
			workspaceView.documentManager.getUndoInfo(documentId)
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
			tabView.addTab(documentId, defaultName, documentEditorComp, "", "", false)

			tabView.currentIndex = tabView.tabModel.count - 1

			loading.stop()
		}

		// Undo info signals
		function onStartUndoInfoReceive(documentId){
			loading.start()
		}

		function onUndoInfoReceived(documentId, availableUndoSteps, availableRedoSteps){
			loading.stop()
		}

		function onUndoInfoReceiveFailed(documentId, message){
			loading.stop()
			ModalDialogManager.showErrorDialog(message)
		}

		// Undo signals
		function onStartUndo(documentId, steps){
			loading.start()
		}

		function onUndoDone(documentId){
			loading.stop()
			workspaceView.documentManager.getUndoInfo(documentId)
		}

		function onUndoFailed(documentId, message){
			loading.stop()
			ModalDialogManager.showErrorDialog(message)
		}

		// Redo signals
		function onStartRedo(documentId, steps){
			loading.start()
		}

		function onRedoDone(documentId){
			loading.stop()
			workspaceView.documentManager.getUndoInfo(documentId)
		}

		function onRedoFailed(documentId, message){
			loading.stop()
			ModalDialogManager.showErrorDialog(message)
		}
	}

	function setDocumentName(documentId, name){
		tabView.setTabName(documentId, name)
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
						workspaceView.documentManager.closeDocument(documentIds[i], true)
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
