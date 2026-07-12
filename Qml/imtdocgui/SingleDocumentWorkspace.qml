import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

// Workspace hosting a single document through DocumentService, without a
// tab strip or CollectionView. Replaces SingleDocumentWorkspaceShellView +
// SingleDocumentWorkspaceContentView (merged: the two were always used
// 1:1, so keeping them as separate files bought no reuse, only an extra
// signal hand-off between them).
//
// States surfaced via the `state` property:
//   - "empty"   : no objectId/typeId set, or the document was closed
//   - "loading" : open/create in flight, or the document isn't ready yet
//   - "error"   : last operation failed; retry() re-issues it
//   - "content" : document is open, data loaded, editor view is up
Item {
	id: workspaceView

	property DocumentService documentManager: null

	property string objectId: ""
	property string objectTypeId: ""

	// When true and objectId is empty, a new document is created instead of
	// opened.
	property bool createNew: false

	// Forwarded to DocumentService.openOrCreateByObjectId as
	// proposedSourceDocumentId when a new document is created.
	property string proposedSourceDocumentId: ""

	property bool showStandardLoading: true
	property bool headerVisible: true

	// When false, the workspace never shows the "enter a document name"
	// dialog: it resolves a name via documentNameResolver (or the service's
	// default name) and saves directly.
	property bool documentNameInputEnabled: true

	// Optional function(documentId) -> string consulted when
	// documentNameInputEnabled is false.
	property var documentNameResolver: null

	readonly property alias documentId: _internal.documentId
	readonly property alias state: _internal.state
	readonly property alias lastErrorMessage: _internal.lastErrorMessage

	// The document currently open, or null. Single source of truth for the
	// header title and the hosted editor.
	readonly property var __activeDocument: (documentManager && documentId !== "") ? documentManager.getDocument(documentId) : null

	signal documentReady(string documentId)
	signal closed(string documentId)

	QtObject {
		id: _internal

		property string documentId: ""
		property string state: "empty"
		property string lastErrorMessage: ""
		property bool requestInFlight: false
		property string requestedObjectId: ""
		property string requestedTypeId: ""
		property bool requestedCreateNew: false
		property string requestedProposedSourceDocumentId: ""
	}

	onDocumentManagerChanged: {
		_internal.documentId = ""
		_internal.state = "empty"
		_internal.lastErrorMessage = ""
		_tryOpen()
	}

	onObjectIdChanged: {
		_internal.documentId = ""
		_tryOpen()
	}

	onObjectTypeIdChanged: _tryOpen()
	onCreateNewChanged: _tryOpen()

	function retry(){
		_internal.lastErrorMessage = ""
		_tryOpen()
	}

	function closeDocument(){
		if (documentManager && _internal.documentId !== ""){
			documentManager.closeDocument(_internal.documentId)
		}
	}

	function _tryOpen(){
		if (!documentManager || objectTypeId === ""){
			_internal.state = "empty"
			return
		}

		if (objectId === "" && !createNew){
			_internal.state = "empty"
			return
		}

		if (_internal.requestInFlight
				&& _internal.requestedObjectId === objectId
				&& _internal.requestedTypeId === objectTypeId
				&& _internal.requestedCreateNew === createNew
				&& _internal.requestedProposedSourceDocumentId === proposedSourceDocumentId){
			return
		}

		if (objectId !== ""){
			let existingDocumentId = documentManager.getDocumentIdByObjectId(objectId)
			if (existingDocumentId !== ""){
				_internal.documentId = existingDocumentId
				_internal.state = "content"
				return
			}
		}

		_internal.requestInFlight = true
		_internal.requestedObjectId = objectId
		_internal.requestedTypeId = objectTypeId
		_internal.requestedCreateNew = createNew
		_internal.requestedProposedSourceDocumentId = proposedSourceDocumentId
		_internal.state = "loading"
		_internal.lastErrorMessage = ""

		documentManager.openOrCreateByObjectId(objectTypeId, objectId, proposedSourceDocumentId)
	}

	function _installNameResolver(doc){
		if (workspaceView.documentNameInputEnabled || !doc){
			return
		}

		doc.nameResolver = function(id){
			let name = typeof workspaceView.documentNameResolver === "function" ? workspaceView.documentNameResolver(id) : ""
			return name || workspaceView.documentManager.getDefaultDocumentName()
		}
	}

	Connections {
		target: workspaceView.documentManager

		function onDocumentAdded(documentId, typeId){
			if (typeId !== workspaceView.objectTypeId){
				return
			}

			let doc = workspaceView.documentManager.getDocument(documentId)
			if (workspaceView.objectId !== "" && doc.objectId !== workspaceView.objectId){
				return
			}

			_internal.documentId = documentId
			_internal.requestInFlight = false
			workspaceView._installNameResolver(doc)
			// remains "loading" until documentReady (data loaded + view bound)
		}

		function onDocumentAlreadyOpened(documentId, typeId){
			if (typeId !== workspaceView.objectTypeId){
				return
			}
			_internal.documentId = documentId
			_internal.requestInFlight = false
			_internal.state = "content"
		}

		function onOperationFailed(documentId, operation, message){
			if (!_internal.requestInFlight || (operation !== "open" && operation !== "create")){
				return
			}
			_internal.requestInFlight = false
			_internal.lastErrorMessage = message
			_internal.state = "error"
		}

		function onDocumentReady(documentId){
			if (documentId !== _internal.documentId){
				return
			}
			_internal.state = "content"
			workspaceView.documentReady(documentId)
		}

		function onDocumentClosed(documentId){
			if (documentId !== _internal.documentId){
				return
			}
			let closedId = _internal.documentId
			_internal.documentId = ""
			_internal.state = "empty"
			workspaceView.closed(closedId)
		}
	}

	Rectangle {
		anchors.fill: parent
		color: Style.baseColor
	}

	Item {
		id: header
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		height: visible ? Style.controlHeightL : 0
		visible: workspaceView.headerVisible && _internal.documentId !== ""

		ToolButton {
			id: closeButton
			objectName: "SingleDocumentCloseButton"
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: parent.left
			anchors.leftMargin: Style.marginM
			width: Style.buttonWidthM
			height: width
			iconSource: "../../../" + Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal)
			onClicked: workspaceView.closeDocument()
		}

		Text {
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: closeButton.right
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

	Item {
		id: contentArea
		anchors.top: header.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		Item {
			id: editorContainer
			anchors.fill: parent
			visible: _internal.state === "content" || _internal.state === "loading"

			property var doc: workspaceView.__activeDocument
			property var hostedItem: null

			onDocChanged: _rebuild()
			Component.onDestruction: _destroyHosted()

			function _rebuild(){
				_destroyHosted()

				if (!doc){
					return
				}

				let item = workspaceView.documentManager.editorHost.createEditor(doc, "", editorContainer)
				if (!item){
					return
				}

				hostedItem = item
				if (item.documentManagerView !== undefined){
					item.documentManagerView = workspaceView
				}

				__syncLoading()
			}

			function _destroyHosted(){
				if (hostedItem){
					hostedItem.destroy()
					hostedItem = null
				}
			}

			function __syncLoading(){
				if (doc && doc.loading){
					contentLoading.start()
				}
				else{
					contentLoading.stop()
				}
			}

			Connections {
				target: editorContainer.doc
				function onLoadingChanged(){
					editorContainer.__syncLoading()
				}
			}

			Loading {
				id: contentLoading
				z: parent.z + 1
				anchors.fill: parent
				visible: false
				opacity: workspaceView.showStandardLoading ? 1 : 0
				background.opacity: 0
			}
		}

		Text {
			anchors.centerIn: parent
			visible: _internal.state === "empty"
			font.pixelSize: Style.fontSizeL
			font.family: Style.fontFamily
			color: Style.titleColor
			text: qsTr("No document selected")
		}

		Column {
			anchors.centerIn: parent
			visible: _internal.state === "error"
			spacing: Style.marginM

			Text {
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: Style.fontSizeL
				font.family: Style.fontFamily
				color: Style.titleColor
				text: _internal.lastErrorMessage !== "" ? _internal.lastErrorMessage : qsTr("Failed to open document")
			}

			Button {
				anchors.horizontalCenter: parent.horizontalCenter
				text: qsTr("Retry")
				onClicked: workspaceView.retry()
			}
		}

		Loading {
			id: openingLoading
			z: parent.z + 1
			anchors.fill: parent
			visible: false
			opacity: workspaceView.showStandardLoading ? 1 : 0
			background.opacity: 0
		}

		Connections {
			target: _internal
			function onStateChanged(){
				if (_internal.state === "loading" && _internal.documentId === ""){
					openingLoading.start()
				}
				else{
					openingLoading.stop()
				}
			}
		}
	}
}
