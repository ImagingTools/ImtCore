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
	property string collectionTabId: ""
	property color contentColor: Style.baseColor
	// Path its tabs are recorded under in the navigation history; empty uses the collection id.
	property string historyPath: ""
	// Icon of the page hosting this view; every tab carries it.
	property string pageIcon: ""

	property bool tabVisible: true

	signal startLoading(string documentId)
	signal stopLoading(string documentId)

	onCollectionViewChanged: {
		if (collectionView){
			navigableItem.parentSegment = collectionView.collectionId
		}
	}

	onPageIconChanged: {
		for (let i = 0; i < tabView.tabModel.count; i++){
			tabView.tabModel.setProperty(i, "icon", workspaceView.pageIcon)
		}
	}

	onDocumentManagerChanged: {
		if (documentManager){
			documentManager.setDocumentServiceActiveView(workspaceView)
		}
	}

	NavigableItem {
		id: navigableItem
		// The rest of the path is the document id, consumed here.
		forwardRest: false
		// A history entry for the page itself returns to the collection tab.
		onParentActivated: {
			if (restPath.length === 0){
				tabView.currentIndex = 0
			}
		}
		onActivated: {
			if (workspaceView.documentManager){
				if (restPath.length >= 1){
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

	function updateTabName(documentId){
		if (!documentManager){
			console.error("Unable to update tab name for document '"+documentId+"'. Error: Document manager is invalid")
			return
		}

		let documentName = documentManager.getDocumentName(documentId)
		if (documentName === ""){
			documentName = documentManager.getDefaultDocumentName()
		}

		tabView.setTabName(documentId, documentName)
		tabView.setTabDirty(documentId, documentManager.documentIsDirty(documentId))
	}

	function setCurrentTabIndex(index){
		tabView.currentIndex = index
	}

	// A tab opens as soon as a document is requested; it is rekeyed to the document id the server assigns.
	readonly property string openingTabPrefix: "opening:"
	// Object ids whose placeholder tab was closed before the server answered; the document is closed on arrival.
	property var cancelledOpenings: ({})
	// Object id -> type id of the placeholder tabs, so their history entry can be written before the document id exists.
	property var openingTypeIds: ({})

	// Every tab the user lands on is a history entry, as an editor is in VS Code: <page>/<type>/<object id> or <page>.
	function recordTabHistory(){
		let segment = workspaceView.historyPath !== "" ? workspaceView.historyPath : navigableItem.parentSegment
		if (!segment || !workspaceView.visible){
			return
		}

		let tabId = tabView.getTabIdByIndex(tabView.currentIndex)
		if (tabId === ""){
			return
		}

		if (tabId === workspaceView.collectionTabId){
			NavigationController.push(segment)
			return
		}

		let objectId = ""
		let typeId = ""
		if (workspaceView.isOpeningTab(tabId)){
			objectId = tabId.slice(workspaceView.openingTabPrefix.length)
			typeId = workspaceView.openingTypeIds[objectId] || ""
		}
		else if (workspaceView.documentManager){
			objectId = workspaceView.documentManager.getDocumentObjectId(tabId)
			typeId = workspaceView.documentManager.getDocumentTypeId(tabId)
		}

		if (objectId && typeId){
			NavigationController.push(segment + "/" + typeId + "/" + objectId)
		}
	}

	function isOpeningTab(tabId){
		return tabId.indexOf(workspaceView.openingTabPrefix) === 0
	}

	function addOpeningTab(objectId, typeId){
		workspaceView.openingTypeIds[objectId] = typeId
		let tabId = workspaceView.openingTabPrefix + objectId
		let index = tabView.getIndexById(tabId)
		if (index < 0){
			tabView.addTab(tabId, "", stackViewComp, workspaceView.pageIcon, "", false)
			tabView.setTabLoading(tabId, true)
			index = tabView.tabModel.count - 1
		}

		tabView.currentIndex = index
	}

	function removeOpeningTabs(){
		for (let i = tabView.tabModel.count - 1; i >= 0; i--){
			let tabId = tabView.getTabIdByIndex(i)
			if (workspaceView.isOpeningTab(tabId)){
				tabView.removeTab(tabId)
			}
		}
	}

	function closeTab(tabId){
		if (tabId === workspaceView.collectionTabId){
			return
		}

		if (workspaceView.isOpeningTab(tabId)){
			workspaceView.cancelledOpenings[tabId.slice(workspaceView.openingTabPrefix.length)] = true
			tabView.removeTab(tabId)
			return
		}

		workspaceView.documentManager.closeDocument(tabId)
	}

	// The opened-document list replays raw create/open signals, which bypass the service's "already open" check.
	function focusExistingTab(documentId){
		let index = tabView.getIndexById(documentId)
		if (index < 0){
			return false
		}

		tabView.currentIndex = index
		return true
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
		target: workspaceView.collectionView
		function onCollectionIdChanged(){
			navigableItem.parentSegment = target.collectionId
		}
	}

	// Next tick: setting paths delivers a pending path, which must not add a tab while the collection tab is being inserted.
	Timer {
		id: pathsUpdateTimer
		interval: 0
		onTriggered: {
			if (workspaceView.documentManager){
				navigableItem.paths = workspaceView.documentManager.getSupportedDocumentTypeIds()
			}
		}
	}

	Connections {
		id: connections
		target: workspaceView.documentManager

		function onDocumentViewRegistered(documentTypeId, viewTypeId){
			pathsUpdateTimer.restart()
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
			for (let i = 0; i < documentInfoList.count; ++i){
				let documentInfo = documentInfoList.get(i).item
				let objectId = documentInfo.m_objectId
				let documentId = documentInfo.m_documentId
				let documentName = documentInfo.m_documentName
				let objectTypeId = documentInfo.m_objectTypeId
				let isDirty = documentInfo.m_isDirty
				let hasNameProvider = documentInfo.m_hasNameProvider

				// Resync: the service refreshes open documents; a missed DocumentDataLoaded must not keep one loading.
				if (workspaceView.documentManager.documentIsOpened(String(documentId))){
					if (documentInfo.m_isLoading === false){
						workspaceView.documentManager.setDocumentIsLoading(String(documentId), false)
					}
					continue
				}

				workspaceView.documentManager.setAutoNamedTypeId(objectTypeId, hasNameProvider)

				if (objectId === ""){
					workspaceView.documentManager.documentCreated(documentId, objectTypeId)
				}
				else{
					workspaceView.documentManager.setDocumentName(documentId, documentName)
					workspaceView.documentManager.documentOpened(documentId, objectTypeId)
					workspaceView.documentManager.setDocumentObjectId(documentId, objectId)
				}

				workspaceView.documentManager.getUndoInfo(documentId)
				workspaceView.documentManager.documentManagerChanged(EDocumentOperationEnum.s_documentChanged, objectId, documentId, documentName)
			}

			globalLoading.stop()
		}

		function onOpenedDocumentListReceiveFailed(message){
			globalLoading.stop()
			if(message !==""){
				PopupManager.addErrorMessage(message, true)
			}
		}

		function onDocumentGuiUpdated(documentId, representation){
			if (!workspaceView.documentManager.documentIsLoading(documentId)){
				workspaceView.stopLoading(documentId)
			}
		}

		function onDocumentDataLoaded(documentId){
			tabView.setTabLoading(documentId, false)
			workspaceView.stopLoading(documentId)
		}

		function onStartUpdateRepresentation(documentId, representation){
			workspaceView.startLoading(documentId)
		}

		function onDocumentRepresentationUpdated(documentId, representation){
			// loading.stop()
		}

		function onUpdateRepresentationFailed(documentId, message){
			tabView.setTabLoading(documentId, false)
			if(message !==""){
				PopupManager.addErrorMessage(message, true)
			}
			if (!workspaceView.documentManager.documentIsLoading(documentId)){
				workspaceView.stopLoading(documentId)
			}
		}

		function onDocumentManagerChanged(typeOperation, objectId, documentId, documentName){
			if (typeOperation === EDocumentOperationEnum.s_documentClosed){
				tabView.removeTab(documentId)
			}
			else if (typeOperation === EDocumentOperationEnum.s_newDocumentCreated ||
					typeOperation === EDocumentOperationEnum.s_documentOpened){
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
		// documentId is the object id here: the document id arrives with the answer.
		function onStartOpenDocument(documentId, typeId){
			workspaceView.startLoading(documentId)
			workspaceView.addOpeningTab(documentId, typeId)
		}

		function onDocumentOpened(documentId, typeId){
			if (workspaceView.focusExistingTab(documentId)){
				return
			}

			let objectId = workspaceView.documentManager.getDocumentObjectId(documentId)
			if (workspaceView.cancelledOpenings[objectId]){
				delete workspaceView.cancelledOpenings[objectId]
				workspaceView.documentManager.closeDocument(documentId)
				return
			}

			let openingTabId = workspaceView.openingTabPrefix + objectId
			let openingIndex = tabView.getIndexById(openingTabId)
			if (openingIndex >= 0){
				tabView.setTabId(openingTabId, documentId)
				tabView.setTabLoading(documentId, workspaceView.documentManager.documentIsLoading(documentId))
				tabView.currentIndex = openingIndex
				workspaceView.updateTabName(documentId)

				let openingItem = tabView.getTabById(documentId)
				if (openingItem){
					openingItem.initializeLater(documentId, typeId)
				}

				workspaceView.startLoading(documentId)
				return
			}

			tabView.addTab(documentId, "", stackViewComp, workspaceView.pageIcon, "", false)
			tabView.setTabLoading(documentId, workspaceView.documentManager.documentIsLoading(documentId))
			tabView.currentIndex = tabView.tabModel.count - 1
			workspaceView.updateTabName(documentId)
			workspaceView.startLoading(documentId)
		}

		function onDocumentAlreadyOpened(documentId, typeId){
			let index = tabView.getIndexById(documentId)
			if (index >= 0){
				tabView.currentIndex = index
			}
		}

		function onOpenDocumentFailed(documentId, message){
			workspaceView.removeOpeningTabs()
			tabView.setTabLoading(documentId, false)
			workspaceView.stopLoading(documentId)
			if(message !==""){
				PopupManager.addErrorMessage(message, true)
			}
		}

		// Close document signals
		function onStartCloseDocument(documentId){
			workspaceView.startLoading(documentId)
		}

		function onDocumentClosed(documentId){
			tabView.removeTab(documentId)
			workspaceView.stopLoading(documentId)
		}

		function onDocumentLost(documentId, documentName){
			PopupManager.addInfoMessage(qsTr("The document \"%1\" is no longer open on the server and was closed").arg(documentName), true)
		}

		function onCloseDocumentFailed(documentId, message){
			workspaceView.stopLoading(documentId)
			if(message !==""){
				PopupManager.addErrorMessage(message, true)
			}
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
			if(message !==""){
				PopupManager.addErrorMessage(message, true)
			}
		}

		// Create document signals
		function onStartCreateDocument(documentTypeId){
		}

		function onCreateDocumentFailed(documentTypeId, message){
			if(message !==""){
				PopupManager.addErrorMessage(message, true)
			}
		}

		function onDocumentCreated(documentId, documentTypeId){
			if (workspaceView.focusExistingTab(documentId)){
				return
			}

			tabView.addTab(documentId, "", stackViewComp, workspaceView.pageIcon, "", false)
			tabView.setTabLoading(documentId, workspaceView.documentManager.documentIsLoading(documentId))

			tabView.currentIndex = tabView.tabModel.count - 1
			workspaceView.updateTabName(documentId)
			workspaceView.startLoading(documentId)
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
			if(message !==""){
				PopupManager.addErrorMessage(message, true)
			}
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
			if(message !==""){
				PopupManager.addErrorMessage(message, true)
			}
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
			if(message !==""){
				PopupManager.addErrorMessage(message, true)
			}
		}

		function onTryCloseDirtyDocument(documentId, callback){
			workspaceView.onTryCloseDirtyDocument(documentId, callback)
		}
	}

	function setCollectionViewComp(name, collectionViewComp){
		let tabId = UuidGenerator.generateUUID()
		workspaceView.collectionTabId = tabId
		tabView.addTab(tabId, name, collectionViewComp, workspaceView.pageIcon, "", false, true)
		tabView.currentIndex = 0
	}

	function closeAllDocuments(){
		let documentIds = workspaceView.documentManager.getOpenedDocumentIds()

		for (let i = 0; i < documentIds.length; i++){
			workspaceView.documentManager.closeDocument(documentIds[i])
		}
	}

	Rectangle {
		anchors.fill: parent
		color: workspaceView.contentColor
	}

	Component {
		id: popupMenuDialog

		PopupMenuDialog {
			itemWidth: Style.sizeHintXXS
			onFinished: {
				if (commandId === "Close"){
					workspaceView.closeTab(tabView.getTabIdByIndex(tabView.currentIndex))
				}
				else if (commandId === "CloseAll"){
					workspaceView.closeAllDocuments()
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
			property bool initialized: false

			function initializeLater(id, typeId){
				documentId = id
				documentTypeId = typeId
				initTimer.start()
			}

			Timer {
				id: initTimer
				interval: 0
				onTriggered: {
					if (!stackView.initialized){
						stackView.initialize(stackView.documentId, stackView.documentTypeId)
					}
				}
			}

			// Stands in for the editor until it is built.
			SkeletonTableView {
				id: openingSkeleton
				z: 2
				anchors.fill: parent
				headerVisible: false
				cardRows: true
				rowSpacing: Style.marginS
				contentMaxWidth: Style.sizeHintXXL
				contentTopMargin: Style.marginXL
				maxRows: 8
				Component.onCompleted: {
					openingSkeleton.start()
				}
			}

			function initialize(id, typeId){
				initialized = true
				openingSkeleton.stop()

				documentId = id
				documentTypeId = typeId

				viewTypeIds = workspaceView.documentManager.getSupportedDocumentViewTypeIds(documentTypeId)

				for (let i = 0; i < viewTypeIds.length; ++i){
					let viewComp = workspaceView.documentManager.getDocumentEditorFactory(documentTypeId, viewTypeIds[i])
					addPage(viewComp)
				}

				setCurrentIndex(0)

				// Align overlay state with current document-service state in case
				// start/stopLoading signals were emitted before this tab was initialized.
				if (workspaceView.documentManager
						&& workspaceView.documentManager.documentIsLoading(documentId)
						&& !stackView.skeletonShown()){
					loading.start()
				}
				else{
					loading.stop()
				}
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

				if (item.objectName === "DocumentViewBase"){
					if (item.documentManagerView !== undefined){
						item.documentManagerView = workspaceView
					}
					if (item.documentManager !== undefined){
						item.documentManager = workspaceView.documentManager
					}
					if (item.documentId !== undefined){
						item.documentId = stackView.documentId
					}
					if (item.documentTypeId !== undefined){
						item.documentTypeId = stackView.documentTypeId
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

			// A view showing its skeleton needs no loading indicator on top of it.
			function skeletonShown(){
				for (let viewTypeId in itemViewTypes){
					let view = itemViewTypes[viewTypeId]
					if (view.visible && view.skeletonVisible){
						return true
					}
				}

				return false
			}

			function onCommandActivated(commandId){
				let viewTypeIds = Object.keys(itemViewTypes)
				if (!viewTypeIds.includes(commandId)){
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
					if (documentId !== stackView.documentId || !stackView.initialized || stackView.skeletonShown()){
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
				opacity: workspaceView.showStandardLoading ? 1 : 0

				background.opacity: 0
			}
		}
	}

	TabView {
		id: tabView
		anchors.fill: parent
		closable: true
		tabVisible: workspaceView.tabVisible
		contentColor: workspaceView.contentColor

		onCurrentIndexChanged: {
			workspaceView.recordTabHistory()
		}

		onTabLoaded: {
			if (tabId === workspaceView.collectionTabId){
				workspaceView.collectionView = tabItem
			}
			else if (!workspaceView.isOpeningTab(tabId)){
				// The editor is built on the next tick, so the new tab and its skeleton paint first.
				let typeId = workspaceView.documentManager.getDocumentTypeId(tabId)
				tabItem.initializeLater(tabId, typeId)
			}
		}

		onTabClicked: {
			if (mouse.button === Qt.RightButton){
				let clickedTabId = tabView.getTabIdByIndex(index)
				if (clickedTabId !== workspaceView.collectionTabId){
					var point = tabItem.mapToItem(this, 0, 0)
					ModalDialogManager.openDialog(popupMenuDialog, {"x": point.x + Style.sizeHintXXS, "y": point.y, "model": tabContextMenuModel})
				}
			}
		}

		function onCloseTab(index){
			// Pinned Fixed collection tab must not be closable.
			if (index >= 0 && index < tabModel.count && tabModel.get(index).pinned){
				return
			}
			workspaceView.closeTab(getTabIdByIndex(index))
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
