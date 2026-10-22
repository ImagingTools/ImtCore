import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseImtCollectionSdl 1.0

Item {
	id: workspaceView

	property DocumentService documentManager
	property int popupWidth: Style.sizeHintXXS
	property ObjectVisualStatusProvider visualStatusProvider: null
	// Optional surface overrides (defaults keep the global MultiDoc look).
	property color contentColor: Style.baseColor
	property color tabPanelColor: Style.tabPanelBackgroundColor
	// Optional per-instance tab chrome; default is the global Style.tabPanelDecorator.
	property alias tabDelegateDecorator: tabView.tabDelegateDecorator
	// Fallback icon for document tabs when visual-status does not supply one.
	property string defaultDocumentIcon: ""
	property string dirtyPrefix: "* "

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
			workspaceView.setDocumentName(objectId, text)
			tabView.setTabDescription(objectId, description)
			if (icon !== ""){
				tabView.setTabIcon(objectId, icon)
			}
			else if (workspaceView.defaultDocumentIcon !== ""){
				tabView.setTabIcon(objectId, workspaceView.defaultDocumentIcon)
			}
		}
		
		function onVisualStatusReceiveFailed(objectId, errorMessage){
			workspaceView.setDocumentName(objectId, "")
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
		// The rest of the path is the document id, consumed here.
		forwardRest: false
		onActivated: {
			if (workspaceView.documentManager){
				if (restPath.length >= 1){
					let documentTypeId = matchedPath
					let documentId = restPath[0]
					if (params && params.readOnly !== undefined){
						workspaceView.documentManager.setObjectReadOnly(documentId, params.readOnly === true)
					}

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

	// Next tick: setting paths delivers a pending path, which must wait for the data controller and the collection tab.
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

		function onTypeIdChanged(){
			navigableItem.parentSegment = target.typeId
		}
		
		function onDocumentTypeIdRegistered(documentTypeId){
			pathsUpdateTimer.restart()
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
			if (workspaceView.cancelledOpenings[documentId]){
				delete workspaceView.cancelledOpenings[documentId]
				workspaceView.documentManager.closeDocument(documentId, true)
				return
			}

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
			if (tabIndex >= 0 && workspaceView.openingDocumentIds[documentId]){
				// The placeholder opened with the request takes the real view now that the data is here.
				delete workspaceView.openingDocumentIds[documentId]
				tabView.setTabName(documentId, workspaceView.tabTitle(documentId, documentName))
				tabView.setTabLoading(documentId, false)
				tabView.tabModel.setProperty(tabIndex, "sourceComponent", documentData.viewComp)
			}
			else if (tabIndex < 0){
				tabView.addTab(
							documentData.documentId,
							workspaceView.tabTitle(documentData.documentId, documentName),
							documentData.viewComp,
							workspaceView.defaultDocumentIcon,
							"",
							false)
				tabIndex = tabView.tabModel.count - 1
			}

			
			tabView.currentIndex = tabIndex
		}

		function onDocumentClosed(documentId){
			delete workspaceView.openingDocumentIds[documentId]
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

			let tabName = tabView.getTabName(documentId)
			while (tabName.startsWith(workspaceView.dirtyPrefix)){
				tabName = tabName.slice(workspaceView.dirtyPrefix.length)
			}

			workspaceView.setDocumentName(documentId, tabName)
		}
		
		function onDocumentOpened(documentId){
		}
		
		// A tab with a skeleton opens with the request instead of a busy overlay over the whole workspace.
		function onDocumentOpeningStarted(documentId){
			if (tabView.getIndexById(documentId) >= 0){
				return
			}

			workspaceView.openingDocumentIds[documentId] = true
			tabView.addTab(documentId, "", openingSkeletonComp, workspaceView.defaultDocumentIcon, "", false)
			tabView.setTabLoading(documentId, true)
			tabView.currentIndex = tabView.tabModel.count - 1
		}
		
		function onDocumentOpeningFailed(documentId, message){
			if (workspaceView.openingDocumentIds[documentId]){
				delete workspaceView.openingDocumentIds[documentId]
				tabView.removeTab(documentId)
			}
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
		tabView.setTabName(documentId, workspaceView.tabTitle(documentId, name))
		tabView.setTabDirty(documentId, workspaceView.documentManager.documentIsDirty(documentId))
	}

	// Every tab-name writer goes through here, so an async name update cannot drop the dirty marker.
	function tabTitle(documentId, name){
		if (name === ""){
			name = workspaceView.documentManager.defaultDocumentName
		}

		// Unsaved changes show as a dot on the tab (setTabDirty), not in its title.
		return name
	}

	function setTabIcon(tabId, icon){
		tabView.setTabIcon(tabId, icon)
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

	// Documents requested but not loaded yet, shown as placeholder tabs.
	property var openingDocumentIds: ({})
	// Placeholders closed before the data arrived; the document is closed when it does.
	property var cancelledOpenings: ({})

	Component {
		id: openingSkeletonComp

		Item {
			SkeletonTableView {
				id: openingSkeleton
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
			if (workspaceView.openingDocumentIds[tabId]){
				return
			}

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
			if (workspaceView.openingDocumentIds[tabId]){
				delete workspaceView.openingDocumentIds[tabId]
				workspaceView.cancelledOpenings[tabId] = true
				removeTab(tabId)
				return
			}

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
