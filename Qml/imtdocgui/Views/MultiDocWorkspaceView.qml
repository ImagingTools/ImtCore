import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseImtCollectionSdl 1.0
import imtqmlutils 1.0

Item {
	id: workspaceView

	property DocumentService documentManager
	property int popupWidth: Style.sizeHintXXS
	property ObjectVisualStatusProvider visualStatusProvider: null
	// Optional surface overrides (defaults keep the global MultiDoc look).
	property color contentColor: Style.baseColor
	property color tabPanelColor: Style.baseColor
	// Optional per-instance tab chrome; default is the global Style.tabPanelDecorator.
	property alias tabDelegateDecorator: tabView.tabDelegateDecorator
	// Fallback icon for document tabs when visual-status does not supply one.
	property string defaultDocumentIcon: ""

	// Nested open/save loads — only this workspace is blocked, not the whole app
	// (Events StartLoading/StopLoading are listened by Configurator app-wide).
	property int __localLoadingDepth: 0

	signal viewLoaded(string viewId, var view)

	function startLocalLoading(){
		workspaceView.__localLoadingDepth = workspaceView.__localLoadingDepth + 1
		if (workspaceView.__localLoadingDepth === 1)
			localLoading.start()
	}

	function stopLocalLoading(){
		if (workspaceView.__localLoadingDepth > 0)
			workspaceView.__localLoadingDepth = workspaceView.__localLoadingDepth - 1
		if (workspaceView.__localLoadingDepth <= 0){
			workspaceView.__localLoadingDepth = 0
			localLoading.stop()
		}
	}

	Connections {
		target: workspaceView.visualStatusProvider ? workspaceView.visualStatusProvider : undefined
		
		function onVisualStatusReceived(objectId, icon, text, description){
			let name = text
			if (name === ""){
				name = workspaceView.documentManager.defaultDocumentName
			}

			tabView.setTabName(objectId, name)
			tabView.setTabDescription(objectId, description)
			if (icon !== ""){
				tabView.setTabIcon(objectId, icon)
			}
			else if (workspaceView.defaultDocumentIcon !== ""){
				tabView.setTabIcon(objectId, workspaceView.defaultDocumentIcon)
			}
		}
		
		function onVisualStatusReceiveFailed(objectId, errorMessage){
			tabView.setTabName(objectId, workspaceView.documentManager.defaultDocumentName)
			tabView.setTabDescription(objectId, "")
			if (workspaceView.defaultDocumentIcon !== ""){
				tabView.setTabIcon(objectId, workspaceView.defaultDocumentIcon)
			}
			else{
				tabView.setTabIcon(objectId, "")
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
				else{
					if (params.createNew === true){
						workspaceView.documentManager.insertNewDocument(matchedPath, "")
					}
					else{
						tabView.currentIndex = 0
					}
				}
			}
		}
	}

	Connections {
		id: connections
		target: workspaceView.documentManager
		
		function onTypeIdChanged(){
			navigableItem.parentSegment = target.typeId
		}
		
		function onDocumentTypeIdRegistered(documentTypeId){
			navigableItem.paths = target.getSupportedDocumentTypeIds()
		}

		function onDocumentSaved(documentId){
			workspaceView.stopLocalLoading()
			if (workspaceView.visualStatusProvider){
				let typeId = workspaceView.documentManager.getDocumentTypeId(documentId)
				workspaceView.visualStatusProvider.getVisualStatus(documentId, typeId)
			}

			// documentManager just refreshed its cached name (see DocumentService.onDocumentSaved)
			// from whatever the save actually confirmed - push it into the tab, otherwise a
			// newly created document keeps showing its "New <Type>" placeholder title forever.
			let savedName = workspaceView.documentManager.getDocumentName(documentId)
			if (savedName !== undefined && savedName !== ""){
				workspaceView.setDocumentName(documentId, savedName)
			}
		}
		
		function onDocumentSavingStarted(documentId){
			workspaceView.startLocalLoading()
		}
		
		function onDocumentSavingFailed(documentId, message){
			workspaceView.stopLocalLoading()
			workspaceView.openErrorDialog(message)
		}

		function onDocumentAdded(documentId){
			let typeId = workspaceView.documentManager.getDocumentTypeId(documentId)
			let documentName = workspaceView.documentManager.getDocumentName(documentId)

			// Always show a provisional title immediately. Waiting on GetObjectVisualStatus
			// (waitName=true) used to leave the tab spinner stuck forever when the request
			// hangs or fails without objectId — e.g. opening a Service while its agent is
			// disconnected. Visual status still refreshes the title when it arrives.
			if (documentName === ""){
				documentName = workspaceView.documentManager.defaultDocumentName
			}
			if (workspaceView.visualStatusProvider){
				workspaceView.visualStatusProvider.getVisualStatus(documentId, typeId)
			}

			let documentData = workspaceView.documentManager.getDocumentDataById(documentId)

			let tabIndex = tabView.getIndexById(documentId)
			if (tabIndex < 0){
				tabView.addTab(
							documentData.documentId,
							documentName,
							documentData.viewComp,
							workspaceView.defaultDocumentIcon,
							"",
							false)
				tabIndex = tabView.tabModel.count - 1
			}

			
			tabView.currentIndex = tabIndex
		}

		function onDocumentClosed(documentId){
			let tabIndex = tabView.getIndexById(documentId)
			tabView.removeTab(documentId)
		}

		// The marker is stripped before it is applied, so a title never collects
		// more than one. The dirty branch used to prepend without looking, and a
		// document reported dirty twice ended up titled "* * name".
		function onDocumentIsDirtyChanged(documentId, isDirty){
			let tabIndex = tabView.getIndexById(documentId)
			if (tabIndex < 0){
				return
			}

			let dirtyPrefix = "* "

			let tabName = tabView.getTabName(documentId)
			while (tabName.startsWith(dirtyPrefix)){
				tabName = tabName.slice(dirtyPrefix.length)
			}

			if (tabName === ""){
				tabName = workspaceView.documentManager.defaultDocumentName
			}

			tabView.setTabName(documentId, isDirty ? dirtyPrefix + tabName : tabName)
		}
		
		function onDocumentOpened(documentId){
			workspaceView.stopLocalLoading()
		}
		
		function onDocumentOpeningStarted(documentId){
			workspaceView.startLocalLoading()
		}
		
		function onDocumentOpeningFailed(documentId, message){
			workspaceView.stopLocalLoading()
			if (message !== undefined && message !== "")
				workspaceView.openErrorDialog(message)
		}
		
		function onTryCloseDirtyDocument(documentId, callback){
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
	
	function setCurrentTabIndex(index){
		tabView.currentIndex = index
	}

	function openErrorDialog(message){
		PopupManager.addErrorMessage(message, true)
	}
	
	function setDocumentName(documentId, name){
		tabView.setTabName(documentId, name)
	}

	function addFixedView(viewComp, name, id, forceFocus, pinned, icon){
		if (!forceFocus){
			forceFocus = false
		}
		if (pinned === undefined){
			pinned = true
		}
		if (icon === undefined || icon === null){
			icon = ""
		}

		if (!id || id === ""){
			id = UuidGenerator.generateUUID()
		}

		let index = tabView.getIndexById(id)
		if (index >= 0){
			tabView.currentIndex = index
			return
		}

		tabView.addTab(id, name, viewComp, icon, "", true, pinned)

		if (forceFocus){
			tabView.currentIndex = tabView.tabModel.count - 1
		}
	}

	Rectangle {
		anchors.fill: parent
		color: workspaceView.contentColor
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
		contentColor: workspaceView.contentColor
		tabPanelColor: workspaceView.tabPanelColor

		onTabLoaded: {
			workspaceView.documentManager.setupDocumentView(tabId, tabItem)
			workspaceView.viewLoaded(tabId, tabItem)
		}

		onTabClicked: {
			if (mouse.button === Qt.RightButton && index != 0){
				var point = tabItem.mapToItem(this, 0, 0)
				ModalDialogManager.openDialog(popupMenuDialog, {"x": point.x + workspaceView.popupWidth, "y": point.y, "model": tabContextMenuModel})
			}
		}

		function onCloseTab(index){
			// Pinned / Fixed tabs (e.g. collection list) must not be closable.
			if (index >= 0 && index < tabModel.count && tabModel.get(index).pinned){
				return
			}
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

	// Local busy overlay — scoped to this workspace only (not Configurator / whole app).
	Loading {
		id: localLoading
		anchors.fill: parent
		z: 100
		visible: false
		indicatorSize: Style.iconSizeL
		background.color: Style.baseColor
	}
}
