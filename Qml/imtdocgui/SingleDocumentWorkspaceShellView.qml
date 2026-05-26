import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseImtCollectionSdl 1.0
import imtbaseCollectionDocumentServiceSdl 1.0

// Single-document workspace view that drives a single object through the
// regular CollectionDocumentManager pipeline via any DocumentServiceBase
// implementation, without ever showing a CollectionView or tab strip.
// The shell itself contains no GQL/transport-specific code.
//
// States surfaced via the `state` property:
//   - "empty"   : no objectId/typeId set, or document was closed
//   - "loading" : open/create in flight, or representation update in flight
//   - "error"   : last operation failed; user can retry via retry()
//   - "content" : document is open, data loaded, view is up
Item {
	id: workspaceView

	property DocumentServiceBase documentManager: null

	// Identification of the object to open through the collection.
	// Setting either objectId or objectTypeId after the workspace is bound
	// triggers an open/create attempt.
	property string objectId: ""
	property string objectTypeId: ""

	// When true and objectId is empty, a new document is created instead of
	// opened. Mirrors the createNew=true convention used by the existing
	// MultiDocWorkspaceView navigation layer.
	property bool createNew: false

	// Optional id to forward to the document manager as
	// `proposedSourceDocumentId` when a new document is created (i.e. when
	// `objectId` is empty / `createNew` is true). The server inserts the new
	// collection object with this id, which keeps the client-side
	// representation `m_id` aligned with the persisted object id without an
	// extra round-trip. Ignored when an existing document is opened.
	property string proposedSourceDocumentId: ""

	// Whether to render the standard loading overlay in the content area.
	property bool showStandardLoading: true

	// Whether the header bar (document name + close button) is visible.
	property bool headerVisible: true

	// Controls whether the workspace prompts the user for a document name
	// when the underlying document manager requests one (typically on the
	// first save of a brand new document that has no name yet).
	//
	// When false (e.g. for editors that don't expose a user-visible document
	// name like the Role/User/Group editors inside TenantEditor), the
	// workspace silently fulfils the name request with `getDefaultDocumentName()`
	// so the in-flight Save can proceed without showing a dialog.
	property bool documentNameInputEnabled: true

	// Current resolved document id (set after open/create succeeds).
	readonly property alias documentId: _internal.documentId

	// One of: "empty", "loading", "error", "content".
	readonly property alias state: _internal.state

	// Last error message (only meaningful in "error" state).
	readonly property alias lastErrorMessage: _internal.lastErrorMessage

	// Emitted when the document is fully ready (data loaded + view bound).
	signal documentReady(string documentId)

	// Emitted when the document is closed (either by the user or by a
	// server-side push). After this signal the view returns to "empty".
	signal closed(string documentId)

	QtObject {
		id: _internal

		property string documentId: ""
		property string state: "empty"
		property string lastErrorMessage: ""
		property bool requestInFlight: false
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

	onObjectTypeIdChanged: {
		_tryOpen()
	}

	onCreateNewChanged: {
		_tryOpen()
	}

	// Re-issues the last open/create attempt with the current objectId/typeId.
	function retry(){
		_internal.lastErrorMessage = ""
		_tryOpen()
	}

	// Programmatically closes the document (with standard dirty-confirmation
	// dialog wired up by the underlying service).
	function closeDocument(){
		if (!documentManager || _internal.documentId === ""){
			return
		}
		documentManager.closeDocument(_internal.documentId)
	}

	function _tryOpen(){
		if (!documentManager){
			return
		}

		if (objectTypeId === ""){
			_internal.state = "empty"
			return
		}

		if (objectId === "" && !createNew){
			_internal.state = "empty"
			return
		}

		// Reuse already-open document for the same objectId.
		if (objectId !== ""){
			let existingDocId = documentManager.getDocumentIdByObjectId(objectId)
			if (existingDocId !== ""){
				_internal.documentId = existingDocId
				_internal.state = "content"
				return
			}
		}

		_internal.requestInFlight = true
		_internal.state = "loading"
		_internal.lastErrorMessage = ""

		if (typeof documentManager.openOrCreateByObjectId === "function"){
			documentManager.openOrCreateByObjectId(objectTypeId, objectId, proposedSourceDocumentId)
		}
		else if (objectId === ""){
			documentManager.createDocument(objectTypeId, proposedSourceDocumentId)
		}
		else{
			documentManager.openDocument(objectTypeId, objectId)
		}
	}

	Connections {
		target: workspaceView.documentManager

		function onDocumentOpened(documentId, typeId){
			let docObjId = workspaceView.documentManager.getDocumentObjectId
					? workspaceView.documentManager.getDocumentObjectId(documentId)
					: ""
			if (workspaceView.objectId !== "" && docObjId !== workspaceView.objectId){
				return
			}
			if (typeId !== workspaceView.objectTypeId){
				return
			}
			_internal.documentId = documentId
			_internal.requestInFlight = false
			// remain in "loading" until documentReady (data loaded + view bound)
		}

		function onDocumentCreated(documentId, typeId){
			if (!workspaceView.createNew && workspaceView.objectId !== ""){
				return
			}
			if (typeId !== workspaceView.objectTypeId){
				return
			}
			_internal.documentId = documentId
			_internal.requestInFlight = false
		}

		function onDocumentAlreadyOpened(documentId, typeId){
			if (typeId !== workspaceView.objectTypeId){
				return
			}
			_internal.documentId = documentId
			_internal.requestInFlight = false
			_internal.state = "content"
		}

		function onOpenDocumentFailed(documentId, message){
			if (!_internal.requestInFlight){
				return
			}
			_internal.requestInFlight = false
			_internal.lastErrorMessage = message
			_internal.state = "error"
		}

		function onCreateDocumentFailed(documentTypeId, message){
			if (!_internal.requestInFlight || documentTypeId !== workspaceView.objectTypeId){
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

		function onCloseDocumentFailed(documentId, message){
			if (documentId !== _internal.documentId){
				return
			}
			ModalDialogManager.showErrorDialog(message)
		}

		function onTryCloseDirtyDocument(documentId, callback){
			if (documentId !== _internal.documentId){
				return
			}
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

		function onRequestDocumentName(documentId, documentTypeId){
			if (documentId !== _internal.documentId){
				return
			}
			if (workspaceView.documentManager && workspaceView.documentManager.hasDocumentNameProvider(documentTypeId)){
				return
			}
			if (!workspaceView.documentNameInputEnabled){
				// Name input is disabled for this workspace — fulfil the
				// request with a default name so the save flow proceeds
				// without prompting the user.
				workspaceView.documentManager.setDocumentName(
						documentId,
						workspaceView.documentManager.getDefaultDocumentName())
				return
			}
			ModalDialogManager.openDialog(inputDialogComp, {documentId: documentId})
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
		color: Style.backgroundColor2
	}

	// Header bar with document name + close button.
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
			id: headerNameText
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: closeButton.right
			anchors.leftMargin: Style.marginM
			anchors.right: parent.right
			anchors.rightMargin: Style.marginM
			font.pixelSize: Style.fontSizeXXL
			font.family: Style.fontFamily
			color: Style.titleColor
			elide: Text.ElideRight
			text: ""

			function refresh(){
				if (!workspaceView.documentManager || _internal.documentId === ""){
					text = ""
					return
				}
				let name = workspaceView.documentManager.getDocumentName(_internal.documentId)
				if (name === ""){
					name = workspaceView.documentManager.getDefaultDocumentName()
				}
				if (workspaceView.documentManager.documentIsDirty(_internal.documentId)){
					text = "* " + name
				}
				else{
					text = name
				}
			}

			Connections {
				target: workspaceView.documentManager
				function onDocumentNameChanged(documentId, oldName, newName){
					if (documentId === _internal.documentId){
						headerNameText.refresh()
					}
				}
				function onDocumentIsDirtyChanged(documentId, isDirty){
					if (documentId === _internal.documentId){
						headerNameText.refresh()
					}
				}
			}

			Connections {
				target: _internal
				function onDocumentIdChanged(){
					headerNameText.refresh()
				}
			}
		}
	}

	// Content area with state-driven children.
	Item {
		id: contentArea
		anchors.top: header.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		SingleDocumentWorkspaceContentView {
			id: contentView
			anchors.fill: parent
			visible: _internal.state === "content" || _internal.state === "loading"
			documentManager: workspaceView.documentManager
			documentId: _internal.documentId
			documentTypeId: workspaceView.objectTypeId
			showStandardLoading: workspaceView.showStandardLoading
		}

		// Empty placeholder.
		Text {
			anchors.centerIn: parent
			visible: _internal.state === "empty"
			font.pixelSize: Style.fontSizeL
			font.family: Style.fontFamily
			color: Style.titleColor
			text: qsTr("No document selected")
		}

		// Error placeholder with retry.
		Column {
			anchors.centerIn: parent
			visible: _internal.state === "error"
			spacing: Style.marginM

			Text {
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: Style.fontSizeL
				font.family: Style.fontFamily
				color: Style.titleColor
				text: _internal.lastErrorMessage !== ""
					  ? _internal.lastErrorMessage
					  : qsTr("Failed to open document")
			}

			Button {
				anchors.horizontalCenter: parent.horizontalCenter
				text: qsTr("Retry")
				onClicked: workspaceView.retry()
			}
		}

		// Loading overlay for the "loading" state when content is not yet
		// hosted (initial open). Once content view is up it manages its own
		// internal loading indicator via startLoading/stopLoading signals.
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
