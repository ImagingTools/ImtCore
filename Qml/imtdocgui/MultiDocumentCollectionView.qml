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

	signal startLoading(string documentId)
	signal stopLoading(string documentId)

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

	Component {
		id: inputDialogComp
		InputDialog {
			title: qsTr("Document Name")
			placeHolderText: qsTr("Enter the document name")
			property string documentId
			onFinished: {
				if (buttonId === Enums.ok){
					workspaceView.documentManager.setDocumentName(documentId, inputValue)
				}
			}
		}
	}

	Connections {
		id: connections
		target: workspaceView.documentManager

		function onRequestDocumentName(documentId, documentTypeId){
			ModalDialogManager.openDialog(inputDialogComp, {documentId: documentId})
		}

		function onStartGetOpenedDocumentList(){
			globalLoading.start()
		}

		function onOpenedDocumentListReceived(documentInfoList){
			for (let i = 0; i < documentInfoList.count; ++i){
				let documentInfo = documentInfoList.get(i).item
				let objectId = documentInfo.m_objectId
				let documentId = documentInfo.m_documentId
				let documentName = documentInfo.m_documentName
				let objectTypeId = documentInfo.m_objectTypeId
				let isDirty = documentInfo.m_isDirty

				if (objectId === ""){
					workspaceView.documentManager.documentCreated(documentId, objectTypeId)
				}
				else{
					workspaceView.documentManager.setDocumentName(documentId, documentName)
					workspaceView.documentManager.documentOpened(documentId, objectTypeId)
				}

				workspaceView.documentManager.getUndoInfo(documentId)
				workspaceView.documentManager.documentManagerChanged(EDocumentOperationEnum.s_documentChanged, objectId, documentId, documentName)
			}

			globalLoading.stop()
		}

		function onOpenedDocumentListReceiveFailed(message){
			globalLoading.stop()
			ModalDialogManager.showErrorDialog(message)
		}

		function onDocumentGuiUpdated(documentId, representation){
			workspaceView.stopLoading(documentId)
		}

		function onStartUpdateRepresentation(documentId, representation){
			workspaceView.startLoading(documentId)
		}

		function onDocumentRepresentationUpdated(documentId, representation){
			// loading.stop()
		}

		function onDocumentManagerChanged(typeOperation, objectId, documentId, documentName){
			if (typeOperation === EDocumentOperationEnum.s_documentClosed){
				tabView.removeTab(documentId)
			}
			else if (typeOperation === EDocumentOperationEnum.s_documentOpened || typeOperation === EDocumentOperationEnum.s_documentSaved){
				workspaceView.documentManager.setDocumentName(documentId, documentName)
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
			workspaceView.startLoading(documentId)
		}

		function onDocumentOpened(documentId, typeId){
			tabView.addTab(documentId, "", stackViewComp, "", "", false)
			tabView.currentIndex = tabView.tabModel.count - 1
			workspaceView.updateTabName(documentId)
		}

		function onOpenDocumentFailed(documentId, message){
			workspaceView.stopLoading(documentId)
			ModalDialogManager.showErrorDialog(message)
		}

		// Close document signals
		function onStartCloseDocument(documentId){
			workspaceView.startLoading(documentId)
		}

		function onDocumentClosed(documentId){
			tabView.removeTab(documentId)
			workspaceView.stopLoading(documentId)
		}

		function onCloseDocumentFailed(documentId, message){
			workspaceView.stopLoading(documentId)
			ModalDialogManager.showErrorDialog(message)
			onDocumentClosed(documentId)
		}

		// Save document signals
		function onStartSaveDocument(documentId){
			workspaceView.startLoading(documentId)
		}

		function onDocumentSaved(documentId){
			workspaceView.stopLoading(documentId)
		}

		function onSaveDocumentFailed(documentId, message){
			workspaceView.stopLoading(documentId)
			ModalDialogManager.showErrorDialog(message)
		}

		// Create document signals
		function onStartCreateDocument(documentTypeId){
		}

		function onCreateDocumentFailed(documentTypeId, message){
			ModalDialogManager.showErrorDialog(message)
		}

		function onDocumentCreated(documentId, documentTypeId){
			tabView.addTab(documentId, "", stackViewComp, "", "", false)

			tabView.currentIndex = tabView.tabModel.count - 1

			workspaceView.stopLoading(documentId)
		}

		// Undo info signals
		function onStartUndoInfoReceive(documentId){
			// loading.start()
		}

		function onUndoInfoReceived(documentId, availableUndoSteps, availableRedoSteps){
			// loading.stop()
		}

		function onUndoInfoReceiveFailed(documentId, message){
			workspaceView.stopLoading(documentId)
			ModalDialogManager.showErrorDialog(message)
		}

		// Undo signals
		function onStartUndo(documentId, steps){
			workspaceView.startLoading(documentId)
		}

		function onUndoDone(documentId){
			// loading.stop()
		}

		function onUndoFailed(documentId, message){
			workspaceView.stopLoading(documentId)
			ModalDialogManager.showErrorDialog(message)
		}

		// Redo signals
		function onStartRedo(documentId, steps){
			workspaceView.startLoading(documentId)
		}

		function onRedoDone(documentId){
			// loading.stop()
		}

		function onRedoFailed(documentId, message){
			workspaceView.stopLoading(documentId)
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
		tabView.addTab(UuidGenerator.generateUUID(), name, collectionViewComp, false)
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

	Component {
		id: stackViewComp
		StackView {
			id: stackView
			anchors.fill: parent

			property string documentId
			property string documentTypeId

			property var itemViewTypes: ({}) // ViewTypeId -> Item
			property var viewTypeIds: []

			function initialize(id, typeId){
				documentId = id
				documentTypeId = typeId

				viewTypeIds = workspaceView.documentManager.getSupportedDocumentViewTypeIds(documentTypeId)

				for (let i = 0; i < viewTypeIds.length; ++i){
					let viewComp = workspaceView.documentManager.getDocumentEditorFactory(documentTypeId, viewTypeIds[i])
					addPage(viewComp)
				}

				setCurrentIndex(0)
			}

			onPageAdded: {
				let comp = getComponent(index)
				let viewTypeId = viewTypeIds[index]

				itemViewTypes[viewTypeId] = item

				item.commandActivated.connect(onCommandActivated)
				if (item.commandsController){
					item.commandsController.setIsToggleable(viewTypeId, true)
				}

				if (index === 0){
					if (item.commandsController){
						item.commandsController.setToggled(viewTypeId, true)
					}
				}

				workspaceView.documentManager.onViewInstanceCreated(documentId, item, viewTypeId)
			}

			onCurrentPageChanged: {
				for (let viewTypeId in itemViewTypes){
					if (itemViewTypes[viewTypeId] === item){
						onCommandActivated(viewTypeId)
						break
					}
				}
			}

			function onCommandActivated(commandId){
				let viewTypeIds = Object.keys(itemViewTypes)
				if (!viewTypeIds.includes(commandId)){
					console.error("Unable to command handle. Error: Command-ID: '" + commandId + "' does not found")
					return
				}

				for (let i = 0; i < viewTypeIds.length; ++i){
					let viewTypeId = viewTypeIds[i]
					if (viewTypeId === commandId && currentIndex !== i){
						setCurrentIndex(i)
						break
					}

					let currentItem = itemViewTypes[viewTypeId]
					if (currentItem.commandsController){
						for (let j = 0; j < viewTypeIds.length; ++j){
							currentItem.commandsController.setToggled(viewTypeIds[j], viewTypeIds[j] === commandId)
						}
					}
				}
			}

			Connections {
				target: workspaceView
				function onStartLoading(documentId){
					if (documentId !== stackView.documentId){
						return
					}

					loading.start()
				}
				function onStopLoading(documentId){
					if (documentId !== stackView.documentId){
						return
					}

					loading.stop()
				}
			}

			Loading {
				id: loading
				z: parent.z + 1
				anchors.fill: parent
				visible: false

				background.opacity: 0
			}
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
				// Stack View loaded
				let typeId = workspaceView.documentManager.getDocumentTypeId(tabId)
				tabItem.initialize(tabId, typeId)
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
		id: globalLoading
		anchors.top: parent.top
		anchors.topMargin: Style.controlHeightL
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		visible: false
	}
}
