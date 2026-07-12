import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0

// Workspace hosting a shared CollectionView tab plus one tab per open
// document, driven entirely through DocumentService. Replaces
// MultiDocumentCollectionView: the GQL-specific session-restore parsing and
// the three copies of the dirty-close / name-input dialogs that used to
// live here have moved into DocumentService and DocumentInteractionController.
//
// singleDocumentMode turns this into what used to be a separate
// SingleDocumentBreadcrumbWorkspace component: the tab strip is replaced by
// a lightweight back-button/title header, and opening a document closes
// every other one first. The two only ever differed in chrome and in this
// one invariant - the whole document-lifecycle plumbing (open/close/rename/
// dirty tracking) is identical, so it isn't worth a second file.
Item {
	id: workspaceView

	property bool showStandardLoading: true
	property CollectionView collectionView: null
	property DocumentService documentManager
	property string collectionTabId: ""
	property bool tabVisible: true
	property bool singleDocumentMode: false

	// The currently selected document, or null when the collection tab
	// (index 0) is selected. Single source of truth for the
	// singleDocumentMode header - only ever non-null when there is
	// something to show a back button/title for.
	readonly property var __activeDocument: (singleDocumentMode && documentManager && tabView.currentIndex > 0)
			? documentManager.getDocument(tabView.getTabIdByIndex(tabView.currentIndex)) : null

	// Only used when singleDocumentMode: if a document gets closed while it
	// matches these fields, it is reopened immediately afterwards. Set
	// whenever an open request resolves to a document this workspace
	// already has open (see onDocumentAlreadyOpened), so a close racing
	// with that request doesn't leave the workspace stuck without the
	// document the user asked for.
	property string __reopenAfterCloseDocumentId: ""
	property string __reopenAfterCloseTypeId: ""
	property string __reopenAfterCloseObjectId: ""

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
				tabView.currentIndex = 0
			}
		}
	}

	NavigableItem {
		id: navigableItem
		onActivated: {
			if (workspaceView.documentManager && restPath.length >= 1){
				workspaceView.documentManager.openDocument(matchedPath, restPath[0])
			}
		}
	}

	function updateTabName(documentId){
		let doc = documentManager ? documentManager.getDocument(documentId) : null
		if (!doc){
			return
		}

		tabView.setTabName(documentId, doc.tabTitle)
	}

	function setCurrentTabIndex(index){
		tabView.currentIndex = index
	}

	function setCollectionViewComp(name, collectionViewComp){
		let tabId = UuidGenerator.generateUUID()
		workspaceView.collectionTabId = tabId
		tabView.addTab(tabId, name, collectionViewComp, "", "", false, true)
		tabView.currentIndex = 0
	}

	function __addTab(documentId){
		tabView.addTab(documentId, "", stackViewComp, "", "", false)
		tabView.currentIndex = tabView.tabModel.count - 1
		updateTabName(documentId)

		let doc = documentManager.getDocument(documentId)
		if (doc){
			doc.nameChanged.connect(function(){ workspaceView.updateTabName(documentId) })
			doc.isDirtyChanged.connect(function(){ workspaceView.updateTabName(documentId) })
		}
	}

	// singleDocumentMode only: closes every open document except
	// activeDocumentId, so at most one document is ever open at a time.
	function __closeOtherOpenedDocuments(activeDocumentId){
		if (!workspaceView.documentManager){
			return
		}

		let openedDocumentIds = workspaceView.documentManager.getOpenedDocumentIds()
		for (let i = 0; i < openedDocumentIds.length; ++i){
			if (openedDocumentIds[i] !== activeDocumentId){
				workspaceView.documentManager.closeDocument(openedDocumentIds[i])
			}
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

		function onDocumentAdded(documentId, typeId){
			workspaceView.__addTab(documentId)

			if (workspaceView.singleDocumentMode){
				workspaceView.__reopenAfterCloseDocumentId = ""
				workspaceView.__reopenAfterCloseTypeId = ""
				workspaceView.__reopenAfterCloseObjectId = ""
				workspaceView.__closeOtherOpenedDocuments(documentId)
			}
		}

		function onDocumentAlreadyOpened(documentId, typeId){
			let index = tabView.getIndexById(documentId)
			if (index >= 0){
				tabView.currentIndex = index
			}

			if (workspaceView.singleDocumentMode){
				workspaceView.__reopenAfterCloseDocumentId = documentId
				workspaceView.__reopenAfterCloseTypeId = typeId
				workspaceView.__reopenAfterCloseObjectId = workspaceView.documentManager.getDocumentObjectId(documentId)
				workspaceView.__closeOtherOpenedDocuments(documentId)
			}
		}

		function onOperationFailed(documentId, operation, message){
			if (workspaceView.singleDocumentMode && (operation === "open" || operation === "create")){
				workspaceView.__reopenAfterCloseDocumentId = ""
				workspaceView.__reopenAfterCloseTypeId = ""
				workspaceView.__reopenAfterCloseObjectId = ""
			}
		}

		function onDocumentClosed(documentId){
			tabView.removeTab(documentId)

			if (!workspaceView.singleDocumentMode){
				return
			}

			let shouldReopen = workspaceView.__reopenAfterCloseDocumentId === documentId
			let reopenTypeId = workspaceView.__reopenAfterCloseTypeId
			let reopenObjectId = workspaceView.__reopenAfterCloseObjectId

			workspaceView.__reopenAfterCloseDocumentId = ""
			workspaceView.__reopenAfterCloseTypeId = ""
			workspaceView.__reopenAfterCloseObjectId = ""

			if (shouldReopen && reopenTypeId !== "" && reopenObjectId !== ""){
				workspaceView.documentManager.openDocument(reopenTypeId, reopenObjectId)
			}
		}

		function onRestoringSessionChanged(){
			if (target.restoringSession){
				globalLoading.start()
			}
			else{
				globalLoading.stop()
			}
		}
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
					let tabId = tabView.getTabIdByIndex(tabView.currentIndex)
					if (tabId !== workspaceView.collectionTabId){
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
			property QtObject doc: null

			property var itemViewTypes: ({}) // ViewTypeId -> Item
			property var viewTypeIds: []

			function initialize(id, typeId){
				documentId = id
				documentTypeId = typeId
				doc = workspaceView.documentManager.getDocument(id)

				viewTypeIds = workspaceView.documentManager.getSupportedDocumentViewTypeIds(typeId)

				for (let i = 0; i < viewTypeIds.length; ++i){
					let viewComp = workspaceView.documentManager.getDocumentEditorFactory(typeId, viewTypeIds[i])
					addPage(viewComp)
				}

				setCurrentIndex(0)
				__syncLoading()
			}

			function __syncLoading(){
				if (doc && doc.loading){
					loading.start()
				}
				else{
					loading.stop()
				}
			}

			onPageAdded: {
				let viewTypeId = viewTypeIds[index]
				itemViewTypes[viewTypeId] = item

				item.commandActivated.connect(onCommandActivated)
				if (item.commandsController){
					item.commandsController.setIsToggleable(viewTypeId, true)
				}

				if (index === 0 && item.commandsController){
					item.commandsController.setToggled(viewTypeId, true)
				}

				if (item.documentManagerView !== undefined){
					item.documentManagerView = workspaceView
				}

				workspaceView.documentManager.editorHost.attachController(doc, viewTypeId, item)
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
				let ids = Object.keys(itemViewTypes)
				if (!ids.includes(commandId)){
					return
				}

				for (let i = 0; i < ids.length; ++i){
					let viewTypeId = ids[i]
					if (viewTypeId === commandId && currentIndex !== i){
						setCurrentIndex(i)
						break
					}

					let currentItem = itemViewTypes[viewTypeId]
					if (currentItem.commandsController){
						for (let j = 0; j < ids.length; ++j){
							currentItem.commandsController.setToggled(ids[j], ids[j] === commandId)
						}
					}
				}
			}

			Connections {
				target: stackView.doc
				function onLoadingChanged(){
					stackView.__syncLoading()
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

	// Breadcrumb-style header shown instead of the tab strip in
	// singleDocumentMode, whenever the selected tab is a document (index 0
	// is always the collection tab). Replaces the multi-level breadcrumb
	// ListView the old SingleDocumentBreadcrumbWorkspace had: since
	// singleDocumentMode never allows more than one document open at a
	// time, there is only ever one level to show.
	Item {
		id: header
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		height: visible ? Style.controlHeightL : 0
		visible: workspaceView.__activeDocument !== null

		ToolButton {
			id: backButton
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: parent.left
			anchors.leftMargin: Style.marginM
			width: Style.buttonWidthM
			height: width
			iconSource: "../../../" + Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal)
			onClicked: {
				if (workspaceView.__activeDocument){
					workspaceView.documentManager.closeDocument(workspaceView.__activeDocument.documentId)
				}
			}
		}

		Text {
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: backButton.right
			anchors.leftMargin: Style.marginM
			anchors.right: parent.right
			anchors.rightMargin: Style.marginM
			font.pixelSize: Style.fontSizeXXL
			font.family: Style.fontFamily
			color: Style.titleColor
			elide: Text.ElideRight

			text: workspaceView.__activeDocument ? workspaceView.__activeDocument.tabTitle : ""
		}
	}

	TabView {
		id: tabView
		anchors.top: header.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		closable: true
		tabVisible: workspaceView.singleDocumentMode ? false : workspaceView.tabVisible

		onTabLoaded: {
			if (tabId === workspaceView.collectionTabId){
				workspaceView.collectionView = tabItem
			}
			else{
				let typeId = workspaceView.documentManager.getDocumentTypeId(tabId)
				tabItem.initialize(tabId, typeId)
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
