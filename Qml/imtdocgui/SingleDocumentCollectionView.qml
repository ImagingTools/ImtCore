import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtbaseImtCollectionSdl 1.0
import imtbaseCollectionDocumentServiceSdl 1.0

Item {
	id: workspaceView

	property bool showStandardLoading: true
	property CollectionView collectionView: null
	property DocumentServiceBase documentManager
	property string reopenAfterCloseDocumentId: ""
	property string reopenAfterCloseTypeId: ""
	property string reopenAfterCloseObjectId: ""

	signal startLoading(string documentId)
	signal stopLoading(string documentId)

	onCollectionViewChanged: {
		if (collectionView){
			navigableItem.parentSegment = collectionView.collectionId
		}
	}

	onDocumentManagerChanged: {
		if (documentManager){
			documentManager.setDocumentServiceActiveView(workspaceView)
		}
	}

	NavigableItem {
		id: firstTabNavigation
		forwardRest: false
		onActivated: {
			if (restPath.length <= 1){
				if (stackView.count > 1){
					stackView.removePage(stackView.count - 1)
					headersModel.removeAllAfterFirst()
				}
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
					workspaceView.documentManager.openDocument(documentTypeId, documentId)
				}
			}
		}
	}

	function setCollectionViewComp(name, collectionViewComp){
		stackView.addPage(collectionViewComp)
		headersModel.addHeader(UuidGenerator.generateUUID(), name)
		stackView.setCurrentIndex(0)
	}

	function __showSingleDocument(documentId, name){
		if (headersModel.count > 1){
			let currentHeader = headersModel.get(headersModel.count - 1)
			if (currentHeader.headerId === documentId){
				headersModel.setHeaderName(documentId, name)
				workspaceView.__closeOtherOpenedDocuments(documentId)
				if (stackView.count > 1){
					stackView.setCurrentIndex(stackView.count - 1)
				}
				return
			}

			while (stackView.count > 1){
				stackView.removePage(stackView.count - 1)
			}
			headersModel.removeAllAfterFirst()
		}

		headersModel.addHeader(documentId, name)
		stackView.addPage(documentViewComp)
		stackView.setCurrentIndex(stackView.count - 1)
		workspaceView.__closeOtherOpenedDocuments(documentId)
	}

	function __closeOtherOpenedDocuments(activeDocumentId){
		if (!workspaceView.documentManager)
			return

		let openedDocumentIds = workspaceView.documentManager.getOpenedDocumentIds()
		for (let i = 0; i < openedDocumentIds.length; ++i){
			let openedId = openedDocumentIds[i]
			if (openedId !== activeDocumentId)
				workspaceView.documentManager.closeDocument(openedId)
		}
	}

	Connections {
		target: workspaceView.collectionView
		function onCollectionIdChanged(){
			navigableItem.parentSegment = target.collectionId
		}
	}

	Connections {
		id: connections
		target: workspaceView.documentManager

		function onDocumentViewRegistered(documentTypeId, viewTypeId){
			navigableItem.paths = target.getSupportedDocumentTypeIds()
		}

		function onRequestDocumentName(documentId, documentTypeId){
			if (workspaceView.documentManager && workspaceView.documentManager.hasDocumentNameProvider(documentTypeId)){
				return
			}
			ModalDialogManager.openDialog(inputDialogComp, {documentId: documentId})
		}

		function onStartGetOpenedDocumentList(){
			globalLoading.start()
		}

		function onOpenedDocumentListReceived(documentInfoList){
			globalLoading.stop()
		}

		function onOpenedDocumentListReceiveFailed(message){
			globalLoading.stop()
			ModalDialogManager.showErrorDialog(message)
		}

		function onDocumentGuiUpdated(documentId, representation){
			if (!workspaceView.documentManager.documentIsLoading(documentId)){
				workspaceView.stopLoading(documentId)
			}
		}

		function onDocumentDataLoaded(documentId){
			workspaceView.stopLoading(documentId)
		}

		function onStartUpdateRepresentation(documentId, representation){
			workspaceView.startLoading(documentId)
		}

		function onDocumentRepresentationUpdated(documentId, representation){
		}

		function onUpdateRepresentationFailed(documentId, message){
			ModalDialogManager.showErrorDialog(message)
			if (!workspaceView.documentManager.documentIsLoading(documentId)){
				workspaceView.stopLoading(documentId)
			}
		}

		function onDocumentManagerChanged(typeOperation, objectId, documentId, documentName){
			if (typeOperation === EDocumentOperationEnum.s_newDocumentCreated ||
					typeOperation === EDocumentOperationEnum.s_documentOpened){
				workspaceView.documentManager.setDocumentName(documentId, documentName)
			}
		}

		function onDocumentNameChanged(documentId, oldName, newName){
			if (headersModel.count > 1){
				let isDirty = workspaceView.documentManager.documentIsDirty(documentId)
				headersModel.setHeaderName(documentId, isDirty ? "* " + newName : newName)
			}
		}

		function onDocumentIsDirtyChanged(documentId, isDirty){
			if (headersModel.count > 1){
				let name = workspaceView.documentManager.getDocumentName(documentId)
				if (name === ""){
					name = workspaceView.documentManager.getDefaultDocumentName()
				}
				headersModel.setHeaderName(documentId, isDirty ? "* " + name : name)
			}
		}

		// Open document signals
		function onStartOpenDocument(documentId, typeId){
			workspaceView.startLoading(documentId)
		}

		function onDocumentOpened(documentId, typeId){
			let name = workspaceView.documentManager.getDocumentName(documentId)
			if (name === ""){
				name = workspaceView.documentManager.getDefaultDocumentName()
			}
			workspaceView.reopenAfterCloseDocumentId = ""
			workspaceView.reopenAfterCloseTypeId = ""
			workspaceView.reopenAfterCloseObjectId = ""
			workspaceView.__showSingleDocument(documentId, name)
			workspaceView.startLoading(documentId)
		}

		function onDocumentAlreadyOpened(documentId, typeId){
			let name = workspaceView.documentManager.getDocumentName(documentId)
			if (name === ""){
				name = workspaceView.documentManager.getDefaultDocumentName()
			}
			workspaceView.reopenAfterCloseDocumentId = documentId
			workspaceView.reopenAfterCloseTypeId = typeId
			workspaceView.reopenAfterCloseObjectId = workspaceView.documentManager.getDocumentObjectId
				? workspaceView.documentManager.getDocumentObjectId(documentId)
				: ""
			workspaceView.__showSingleDocument(documentId, name)
		}

		function onOpenDocumentFailed(documentId, message){
			workspaceView.reopenAfterCloseDocumentId = ""
			workspaceView.reopenAfterCloseTypeId = ""
			workspaceView.reopenAfterCloseObjectId = ""
			workspaceView.stopLoading(documentId)
			ModalDialogManager.showErrorDialog(message)
		}

		// Close document signals
		function onStartCloseDocument(documentId){
			workspaceView.startLoading(documentId)
		}

		function onDocumentClosed(documentId){
			let shouldReopen = workspaceView.reopenAfterCloseDocumentId === documentId
			let reopenTypeId = workspaceView.reopenAfterCloseTypeId
			let reopenObjectId = workspaceView.reopenAfterCloseObjectId

			workspaceView.reopenAfterCloseDocumentId = ""
			workspaceView.reopenAfterCloseTypeId = ""
			workspaceView.reopenAfterCloseObjectId = ""

			if (headersModel.count > 1){
				let currentHeader = headersModel.get(headersModel.count - 1)
				if (currentHeader.headerId === documentId && stackView.count > 1){
					stackView.removePage(stackView.count - 1)
					headersModel.removeAllAfterFirst()
				}
			}
			workspaceView.stopLoading(documentId)

			if (shouldReopen && reopenTypeId !== "" && reopenObjectId !== ""){
				workspaceView.documentManager.openDocument(reopenTypeId, reopenObjectId)
			}
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
			let name = workspaceView.documentManager.getDocumentName(documentId)
			if (name === ""){
				name = workspaceView.documentManager.getDefaultDocumentName()
			}
			workspaceView.reopenAfterCloseDocumentId = ""
			workspaceView.reopenAfterCloseTypeId = ""
			workspaceView.reopenAfterCloseObjectId = ""
			workspaceView.__showSingleDocument(documentId, name)
			workspaceView.startLoading(documentId)
		}

		// Undo info signals
		function onStartUndoInfoReceive(documentId){
		}

		function onUndoInfoReceived(documentId, availableUndoSteps, availableRedoSteps){
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

	Rectangle {
		anchors.fill: parent
		color: Style.baseColor
	}

	ListModel {
		id: headersModel

		function addHeader(id, name){
			let existingIndex = headersModel.getIndexById(id)
			if (existingIndex >= 0){
				headersModel.setProperty(existingIndex, "headerName", name)
				return false
			}
			headersModel.append({headerId: id, headerName: name})
			return true
		}

		function getIndexById(id){
			for (let i = 0; i < headersModel.count; i++){
				if (headersModel.get(i).headerId === id){
					return i
				}
			}
			return -1
		}

		function removeAllAfterFirst(){
			while (headersModel.count > 1){
				headersModel.remove(headersModel.count - 1)
			}
		}

		function setHeaderName(id, name){
			for (let i = 0; i < headersModel.count; i++){
				if (headersModel.get(i).headerId === id){
					headersModel.setProperty(i, "headerName", name)
					return
				}
			}
		}
	}

	Item {
		id: headerBar
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		height: visible ? Style.controlHeightL : 0
		visible: headersModel.count > 1

		ToolButton {
			id: backButton
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: parent.left
			anchors.leftMargin: Style.marginM
			width: Style.buttonWidthM
			height: width
			iconSource: "../../../" + Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal)
			onClicked: {
				if (workspaceView.documentManager){
					let openedDocumentIds = workspaceView.documentManager.getOpenedDocumentIds()
					if (openedDocumentIds.length > 0){
						workspaceView.documentManager.closeDocument(openedDocumentIds[openedDocumentIds.length - 1])
					}
				}
			}
		}

		ListView {
			id: headersListView
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: backButton.right
			anchors.leftMargin: Style.marginM
			anchors.right: parent.right
			height: parent.height
			orientation: ListView.Horizontal
			boundsBehavior: Flickable.StopAtBounds
			model: headersModel
			delegate: Row {
				height: headersListView.height
				spacing: Style.marginM

				Text {
					anchors.verticalCenter: parent.verticalCenter
					font.pixelSize: Style.fontSizeXXL
					font.family: Style.fontFamily
					color: Style.titleColor
					text: "/"
					visible: model.index > 0
				}

				Text {
					anchors.verticalCenter: parent.verticalCenter
					font.pixelSize: Style.fontSizeXXL
					font.family: Style.fontFamily
					color: Style.titleColor
					text: model.headerName
				}
			}
		}
	}

	Component {
		id: documentViewComp
		Item {
			id: documentViewContainer
			anchors.fill: parent

			property string documentId
			property string documentTypeId

			function initialize(id, typeId){
				documentId = id
				documentTypeId = typeId

				let viewTypeIds = workspaceView.documentManager.getSupportedDocumentViewTypeIds(documentTypeId)
				if (viewTypeIds.length > 0){
					let viewComp = workspaceView.documentManager.getDocumentEditorFactory(documentTypeId, viewTypeIds[0])
					if (viewComp){
						let item = viewComp.createObject(documentViewContainer)
						item.anchors.fill = documentViewContainer

						if (item.objectName === "DocumentViewBase"){
							if (item.documentManagerView !== undefined){
								item.documentManagerView = workspaceView
							}
							if (item.documentManager !== undefined){
								item.documentManager = workspaceView.documentManager
							}
							if (item.documentId !== undefined){
								item.documentId = documentViewContainer.documentId
							}
							if (item.documentTypeId !== undefined){
								item.documentTypeId = documentViewContainer.documentTypeId
							}
						}

						workspaceView.documentManager.onViewInstanceCreated(documentId, item, viewTypeIds[0])
					}
				}
			}

			Connections {
				target: workspaceView
				function onStartLoading(documentId){
					if (documentId !== documentViewContainer.documentId){
						return
					}
					loading.start()
				}
				function onStopLoading(documentId){
					if (documentId !== documentViewContainer.documentId){
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
				opacity: workspaceView.showStandardLoading ? 1 : 0
				background.opacity: 0
			}
		}
	}

	StackView {
		id: stackView
		anchors.top: headerBar.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		onPageAdded: {
			if (index === 0){
				workspaceView.collectionView = item
			}
			else{
				let openedDocumentIds = workspaceView.documentManager.getOpenedDocumentIds()
				if (openedDocumentIds.length > 0){
					let documentId = openedDocumentIds[openedDocumentIds.length - 1]
					let typeId = workspaceView.documentManager.getDocumentTypeId(documentId)
					item.initialize(documentId, typeId)
				}
			}
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
