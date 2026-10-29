import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtbaseImtCollectionSdl 1.0

CollectionView {
	id: root;

	property bool hasRemoteChanges: false;
	property bool tableViewParamsStoredServer: true;
	property bool showRemoteChangesAlert: true
	loadingIndicatorDelay: 300

	property string gqlGetListCommandId: root.collectionId + "List";

	// Invisible fields that will be requested for collection
	property var additionalFieldIds: ["id", "name"]
	property var requestedFields: []

	// Permission path per GQL command id, for the commands this view and its
	// representation issue.
	property var permissionPaths: ({})

	property alias subscriptionCommandId: collectionChangeListener.gqlCommandId

	// Set when the server offers the Share command: the documents can be shared with related organizations
	property bool sharingEnabled: false

	signal removed(string objectId)
	signal collectionChanged(var changeInfo)
	signal collectionInserted(var changeInfo)
	signal collectionUpdated(var changeInfo)
	signal collectionRemoved(var changeInfo)
	signal collectionLocalChanged(var changeInfo)
	signal collectionRemoteChanged(var changeInfo)

	commandsControllerComp: Component {
		GqlBasedCommandsController {
			typeId: root.collectionId;
			
			function getHeaders(){
				return root.getHeaders();
			}
		}
	}

	onHeadersChanged: {
		if (table.headers.getItemsCount() > 0 && tableViewParamsStoredServer){
			tableViewParamController.getModel();
		}
	}

	onCommandActivated: {
		if (commandId === "Share" && root.commandsController && root.commandsController.commandIsEnabled("Share")){
			let indexes = root.table.getSelectedIndexes()
			let documentName = indexes.length === 1 ? root.getElementDisplayName(indexes[0]) : ""
			ModalDialogManager.openDialog(shareDocumentDialogComp, {"collectionId": root.collectionId, "objectIds": root.getSelectedIds(), "documentName": documentName, "permissionPaths": root.permissionPaths})
		}
	}

	Connections {
		target: root.commandsController ? root.commandsController : null

		function onCommandsChanged(){
			root.updateSharing()
		}
	}

	function updateSharing(){
		if (root.sharingEnabled || !root.commandsController || !root.commandsController.commandExists("Share")){
			return
		}

		root.sharingEnabled = true
		root.registerFieldFilterDelegate("SharingScope", sharingScopeFilterComp)
	}

	function getSharingScope(){
		let filterDelegate = root.sharingEnabled ? root.filterMenu.getFilterDelegate("SharingScope") : null

		return filterDelegate ? filterDelegate.sharingScope : "All"
	}

	// The "name" field of the row, otherwise the value of the first column
	function getElementDisplayName(index){
		let elements = root.table.elements
		if (elements.containsKey("name", index)){
			return String(elements.getData("name", index))
		}

		let headerId = root.table.headers.getItemsCount() > 0 ? String(root.table.getHeaderId(0)) : ""
		return headerId !== "" && elements.containsKey(headerId, index) ? String(elements.getData(headerId, index)) : ""
	}

	function shareDocuments(objectIds, targetTenantIds, accessLevel, validUntil, message){
		let objectNames = []
		for (let i = 0; i < objectIds.length; i++){
			let name = ""
			for (let index = 0; index < root.table.elements.getItemsCount(); index++){
				if (String(root.table.elements.getData("id", index)) === String(objectIds[i])){
					name = root.getElementDisplayName(index)
					break
				}
			}

			objectNames.push(name)
		}

		shareDocumentsInput.m_collectionId = root.collectionId
		shareDocumentsInput.m_objectIds = objectIds
		shareDocumentsInput.m_objectNames = objectNames
		shareDocumentsInput.m_targetTenantIds = targetTenantIds
		shareDocumentsInput.m_accessLevel = accessLevel
		shareDocumentsInput.m_validUntil = validUntil
		shareDocumentsInput.m_message = message
		shareDocumentsSender.send(shareDocumentsInput)
	}

	ShareDocumentsInput {
		id: shareDocumentsInput
	}

	GqlSdlRequestSender {
		id: shareDocumentsSender

		gqlCommandId: ImtbaseImtCollectionSdlCommandIds.s_shareDocuments
		permissionPath: root.permissionPaths[gqlCommandId] || ""
		requestType: 1

		sdlObjectComp: Component {
			ShareDocumentsPayload {
				onFinished: {
					if (m_errorMessage && m_errorMessage !== ""){
						PopupManager.addErrorMessage(m_errorMessage, true)
						return
					}

					PopupManager.addSuccessMessage(qsTr("The documents have been shared"), true)
					root.doUpdateGui()
				}
			}
		}

		function getHeaders(){
			return root.getHeaders()
		}
	}

	Component {
		id: shareDocumentDialogComp

		ShareDocumentDialog {
			onShareRequested: {
				root.shareDocuments(objectIds, targetTenantIds, accessLevel, validUntil, message)
			}
		}
	}

	Component {
		id: sharingScopeFilterComp

		SharingScopeFilterDelegate {
		}
	}

	TableViewParamController {
		id: tableViewParamController;
		tableId: root.collectionId;
		onUpdated: {
			root.table.tableViewParams = tableViewParamController.tableViewParams;
			root.table.updateWidthFromViewParams();
		}
	}

	onTableViewParamsAccepted: {
		if (tableViewParamsStoredServer){
			tableViewParamController.tableViewParams = root.table.tableViewParams;
			tableViewParamController.saveModel();
		}
	}

	function getHeaders(){
		let headers = {}

		return headers
	}

	dataControllerComp: Component {
		CollectionRepresentation {
			collectionId: root.collectionId;
			gqlGetListCommandId: root.gqlGetListCommandId
			requestedFields: root.requestedFields
			permissionPaths: root.permissionPaths

			additionalFieldIds: root.sharingEnabled ? root.additionalFieldIds.concat(["sharingAccess"]) : root.additionalFieldIds;

			onRemoved: {
				root.removed()
			}

			function getHeaders(){
				let headers = root.getHeaders()
				if (root.sharingEnabled){
					headers["sharingScope"] = root.getSharingScope()
				}

				return headers;
			}
		}
	}

	Component.onCompleted: {
		table.setSortingInfo("timeStamp", "DESC")
		table.saveWidth.connect(root.tableViewParamsAccepted)
		Events.subscribeEvent("UpdateAllModels", root.receiveRemoteChanges);
	}

	Component.onDestruction: {
		table.saveWidth.disconnect(root.tableViewParamsAccepted)
		Events.unSubscribeEvent("UpdateAllModels", root.receiveRemoteChanges);
	}

	onHasRemoteChangesChanged: {
		if (root.visible && hasRemoteChanges && showRemoteChangesAlert){
			root.setAlertPanel(alertPanelComp)
		}
	}

	onSelectionChanged: {
		if (!root.visibleMetaInfo){
			return;
		}
	}

	onVisibleChanged: {
		if (hasRemoteChanges && visible && showRemoteChangesAlert){
			root.setAlertPanel(alertPanelComp)
		}
		else{
			root.setAlertPanel(undefined)
		}
	}

	function receiveRemoteChanges(){
		if (hasRemoteChanges){
			hasRemoteChanges = false;

			root.doUpdateGui();
			root.setAlertPanel(undefined);
		}
	}

	Connections {
		target: AuthorizationController
		function onTenantSelected(tenantId){
			root.doUpdateGui()
		}
	}

	Component {
		id: alertPanelComp;

		Rectangle {
			anchors.fill: parent;

			color: Style.selectedColor;

			Image {
				id: icon;

				anchors.verticalCenter: parent.verticalCenter;
				anchors.left: parent.left;
				anchors.leftMargin: Style.marginM;

				width: Style.iconSizeM;
				height: width;

				sourceSize.height: height;
				sourceSize.width: width;

				source: "../../../" + Style.getIconPath("Icons/Lamp", Icon.State.On, Icon.Mode.Normal);
			}

			BaseText {
				id: message;

				anchors.verticalCenter: parent.verticalCenter;
				anchors.left: icon.right;
				anchors.leftMargin: Style.marginM;
				anchors.right: updateButton.left;

				text: qsTr("This table has been modified from another computer");
			}

			Button {
				id: updateButton;

				anchors.verticalCenter: parent.verticalCenter;
				anchors.right: parent.right;
				anchors.rightMargin: Style.marginM;

				width: Style.buttonWidthXL;
				height: Style.controlHeightM;

				text: qsTr("Update");

				onClicked: {
					root.receiveRemoteChanges()
				}
			}
		}
	}

	function handleSubscription(dataModel, changeInfo) {
		if (changeInfo && changeInfo.valid && changeInfo.isLocalChange) {
			root.doUpdateGui()
		} else if (changeInfo && changeInfo.valid && changeInfo.isRemoteChange) {
			if (root.showRemoteChangesAlert)
				root.hasRemoteChanges = true
			else
				root.doUpdateGui()
		} else {
			root.doUpdateGui()
		}
	}

	RemoteCollectionChangeListener {
		id: collectionChangeListener
		collectionId: root.collectionId
		currentUserId: AuthorizationController.getUserId()
		deliveryEnabled: root.visible

		function getHeaders() {
			return root.getHeaders()
		}

		onChanged: {
			root.collectionChanged(changeInfo)
			root.handleSubscription(rawData, changeInfo)
		}

		onRefreshRequired: {
			root.doUpdateGui()
		}

		onInserted: {
			root.collectionInserted(changeInfo)
		}

		onUpdated: {
			root.collectionUpdated(changeInfo)
		}

		onRemoved: {
			root.collectionRemoved(changeInfo)
		}

		onLocalChanged: {
			root.collectionLocalChanged(changeInfo)
		}

		onRemoteChanged: {
			root.collectionRemoteChanged(changeInfo)
		}
	}
}
