import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseImtCollectionSdl 1.0

Item {
	id: workspaceView

	property DocumentManager documentManager
	property int popupWidth: Style.sizeHintXXS
	property ObjectVisualStatusProvider visualStatusProvider: ObjectVisualStatusProvider {}
	
	Connections {
		target: workspaceView.visualStatusProvider
		
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

	Connections {
		id: connections
		target: workspaceView.documentManager

		function onDocumentSaved(documentId){
			Events.sendEvent("StopLoading")
			let typeId = workspaceView.documentManager.getDocumentTypeId(documentId)
			workspaceView.visualStatusProvider.getVisualStatus(documentId, typeId)
		}
		
		function onDocumentSavingStarted(documentId){
			Events.sendEvent("StartLoading")
		}
		
		function onDocumentSavingFailed(documentId, message){
			workspaceView.openErrorDialog(message)
		}

		function onDocumentAdded(documentId){
			let typeId = workspaceView.documentManager.getDocumentTypeId(documentId)
			workspaceView.visualStatusProvider.getVisualStatus(documentId, typeId)
			
			let documentData = workspaceView.documentManager.getDocumentDataById(documentId)

			let name = workspaceView.documentManager.defaultDocumentName

			let tabIndex = tabView.getIndexById(documentId)
			if (tabIndex < 0){
				tabView.addTab(documentData.documentId, "", documentData.viewComp, "", "", true)
				tabIndex = tabView.tabModel.count - 1
			}

			tabView.currentIndex = tabIndex
		}

		function onDocumentClosed(documentId){
			let tabIndex = tabView.getIndexById(documentId)
			tabView.removeTab(documentId)
		}

		function onDocumentIsDirtyChanged(documentId, isDirty){
			let tabIndex = tabView.getIndexById(documentId)
			if (tabIndex >= 0){
				let tabName = tabView.getTabName(documentId)
				if (tabName === ""){
					tabName = workspaceView.documentManager.defaultDocumentName
				}

				let dirtyPrefix = "* "

				if (isDirty){
					tabView.setTabName(documentId, dirtyPrefix + tabName)
				}
				else{
					if (tabName.startsWith(dirtyPrefix)){
						tabName = tabName.slice(dirtyPrefix.length)
						tabView.setTabName(documentId, tabName)
					}
				}
			}
		}
		
		function onDocumentOpened(documentId){
			Events.sendEvent("StopLoading")
		}
		
		function onDocumentOpeningStarted(documentId){
			Events.sendEvent("StartLoading")
		}
		
		function onDocumentOpeningFailed(documentId, message){
			Events.sendEvent("StopLoading")
			// workspaceView.openErrorDialog(message)
		}
		
		function onTryCloseDirtyDocument(documentId, callback){
			let dialogCallback = function(result){
				if (result == Enums.yes){
					callback(true)
				}
				else if (result == Enums.no){
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
	
	function openErrorDialog(message){
		ModalDialogManager.showErrorDialog(message)
	}
	
	function setDocumentName(documentId, name){
		tabView.setTabName(documentId, name)
	}

	function addFixedView(viewComp, name, id, forceFocus){
		if (!forceFocus){
			forceFocus = false
		}

		if (!id || id === ""){
			id = UuidGenerator.generateUUID()
		}

		let index = tabView.getIndexById(id)
		if (index >= 0){
			tabView.currentIndex = index
			return
		}

		tabView.addTab(id, name, viewComp)

		if (forceFocus){
			tabView.currentIndex = tabView.tabModel.count - 1
		}
	}

	Rectangle {
		anchors.fill: parent
		color: Style.backgroundColor2
	}

	Component {
		id: popupMenuDialog

		PopupMenuDialog {
			itemWidth: workspaceView.popupWidth
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
			workspaceView.documentManager.setupDocumentView(tabId, tabItem)
		}

		onTabClicked: {
			if (mouse.button === Qt.RightButton && index != 0){
				var point = tabItem.mapToItem(this, 0, 0)
				ModalDialogManager.openDialog(popupMenuDialog, {"x": point.x + workspaceView.popupWidth, "y": point.y, "model": tabContextMenuModel})
			}
		}

		function onCloseTab(index){
			let tabId = getTabIdByIndex(index)
			let documentData = workspaceView.documentManager.getDocumentDataById(tabId)
			if (documentData){
				workspaceView.documentManager.closeDocument(tabId)
			}
			else{
				removeTab(tabId)
			}
		}
	}
}
