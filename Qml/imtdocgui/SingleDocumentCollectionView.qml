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
	// Header of the open document after the collection header, empty while the collection is shown
	property string __documentHeaderId: ""
	property string __dirtyDocumentId: ""

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
					workspaceView.__removeDocumentHeader()
				}
			}
		}
	}

	NavigableItem {
		id: navigableItem
		// The rest of the path is the document id, consumed here.
		forwardRest: false
		onActivated: {
			if (workspaceView.documentManager){
				if (params && params.createNew){
					workspaceView.documentManager.createDocument(matchedPath)
				}
				else if (restPath.length >= 1){
					let documentTypeId = matchedPath
					let documentId = restPath[0]
					if (params && params.readOnly !== undefined){
						workspaceView.documentManager.setObjectReadOnly(documentId, params.readOnly === true)
					}

					workspaceView.documentManager.openDocument(documentTypeId, documentId)
				}
			}
		}
	}

	function setCollectionViewComp(name, collectionViewComp){
		stackView.addPage(collectionViewComp)
		stackViewHeader.addHeader(UuidGenerator.generateUUID(), name)
		stackView.setCurrentIndex(0)
	}

	function __removeDocumentHeader(){
		if (workspaceView.__documentHeaderId !== ""){
			stackViewHeader.removeHeader(workspaceView.__documentHeaderId)
			workspaceView.__documentHeaderId = ""
		}
	}

	function __closeCurrentDocument(){
		if (workspaceView.documentManager){
			let openedDocumentIds = workspaceView.documentManager.getOpenedDocumentIds()
			if (openedDocumentIds.length > 0){
				workspaceView.documentManager.closeDocument(openedDocumentIds[openedDocumentIds.length - 1])
			}
		}
	}

	function __showSingleDocument(documentId, name){
		if (workspaceView.__documentHeaderId !== ""){
			if (workspaceView.__documentHeaderId === documentId){
				stackViewHeader.setHeaderName(documentId, name)
				workspaceView.__closeOtherOpenedDocuments(documentId)
				if (stackView.count > 1){
					stackView.setCurrentIndex(stackView.count - 1)
				}
				return
			}

			while (stackView.count > 1){
				stackView.removePage(stackView.count - 1)
			}
			workspaceView.__removeDocumentHeader()
		}

		stackViewHeader.addHeader(documentId, name)
		workspaceView.__documentHeaderId = documentId
		stackView.addPage(documentViewComp)
		stackView.setCurrentIndex(stackView.count - 1)
		workspaceView.__closeOtherOpenedDocuments(documentId)
	}

	// The name of the document in the header, with "* " while it has unsaved changes
	function __updateDocumentHeader(documentId){
		if (workspaceView.__documentHeaderId === ""){
			return
		}

		let name = workspaceView.documentManager.getDocumentName(documentId)
		if (name === ""){
			name = workspaceView.documentManager.getDefaultDocumentName()
		}
		let isDirty = workspaceView.documentManager.documentIsDirty(documentId) && !dirtyMarkTimer.running
		stackViewHeader.setHeaderName(documentId, isDirty ? "* " + name : name)
	}

	// A document saved with each change is dirty only for the moment of the save: the mark waits, so the name does not blink
	Timer {
		id: dirtyMarkTimer

		interval: 300

		// Stopped first: the header reads running, which the web runtime still reports inside onTriggered
		onTriggered: {
			dirtyMarkTimer.stop()
			workspaceView.__updateDocumentHeader(workspaceView.__dirtyDocumentId)
		}
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
			PopupManager.addErrorMessage(message, true)
		}

		function onDocumentLost(documentId, documentName){
			PopupManager.addInfoMessage(qsTr("The document \"%1\" is no longer open on the server and was closed").arg(documentName), true)
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
			PopupManager.addErrorMessage(message, true)
			if (!workspaceView.documentManager.documentIsLoading(documentId)){
				workspaceView.stopLoading(documentId)
			}
		}

		// A refused change is undone by reading the document again: the reason is told here
		function onUpdateDocumentFailed(documentId, message){
			if (message !== ""){
				PopupManager.addErrorMessage(message, true)
			}
		}

		function onDocumentManagerChanged(typeOperation, objectId, documentId, documentName){
			if (typeOperation === EDocumentOperationEnum.s_newDocumentCreated ||
					typeOperation === EDocumentOperationEnum.s_documentOpened){
				workspaceView.documentManager.setDocumentName(documentId, documentName)
			}
		}

		function onDocumentNameChanged(documentId, oldName, newName){
			workspaceView.__updateDocumentHeader(documentId)
		}

		function onDocumentIsDirtyChanged(documentId, isDirty){
			if (isDirty){
				workspaceView.__dirtyDocumentId = documentId
				dirtyMarkTimer.restart()
				return
			}

			if (workspaceView.__dirtyDocumentId === documentId){
				dirtyMarkTimer.stop()
			}
			workspaceView.__updateDocumentHeader(documentId)
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
			// The data can be loaded already: DocumentDataLoaded came before the document was shown
			if (workspaceView.documentManager.documentIsLoading(documentId)){
				workspaceView.startLoading(documentId)
			}
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
			PopupManager.addErrorMessage(message, true)
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

			if (workspaceView.__documentHeaderId === documentId && stackView.count > 1){
				stackView.removePage(stackView.count - 1)
				workspaceView.__removeDocumentHeader()
			}
			workspaceView.stopLoading(documentId)

			if (shouldReopen && reopenTypeId !== "" && reopenObjectId !== ""){
				workspaceView.documentManager.openDocument(reopenTypeId, reopenObjectId)
			}
		}

		function onCloseDocumentFailed(documentId, message){
			workspaceView.stopLoading(documentId)
			PopupManager.addErrorMessage(message, true)
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
			PopupManager.addErrorMessage(message, true)
		}

		// Create document signals
		function onStartCreateDocument(documentTypeId){
		}

		function onCreateDocumentFailed(documentTypeId, message){
			PopupManager.addErrorMessage(message, true)
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
			PopupManager.addErrorMessage(message, true)
		}

		// Undo signals
		function onStartUndo(documentId, steps){
			workspaceView.startLoading(documentId)
		}

		// A representation update that follows starts the indicator again; waiting for it left it on when none came.
		function onUndoDone(documentId){
			workspaceView.stopLoading(documentId)
		}

		function onUndoFailed(documentId, message){
			workspaceView.stopLoading(documentId)
			PopupManager.addErrorMessage(message, true)
		}

		// Redo signals
		function onStartRedo(documentId, steps){
			workspaceView.startLoading(documentId)
		}

		function onRedoDone(documentId){
			workspaceView.stopLoading(documentId)
		}

		function onRedoFailed(documentId, message){
			workspaceView.stopLoading(documentId)
			PopupManager.addErrorMessage(message, true)
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

	// The headers as in SimpleCollectionPage: the collection, then the open document;
	// the back button or the collection header closes the document
	StackViewHeader {
		id: stackViewHeader
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.leftMargin: Style.marginM
		anchors.right: parent.right
		height: visible ? Style.controlHeightL : 0
		visible: workspaceView.__documentHeaderId !== ""
		initialItemTitleVisible: true

		onCloseClicked: {
			workspaceView.__closeCurrentDocument()
		}

		onHeaderItemClicked: {
			workspaceView.__closeCurrentDocument()
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
		anchors.top: stackViewHeader.bottom
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
