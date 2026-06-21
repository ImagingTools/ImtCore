import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0

CollectionView {
	id: root;

	property bool hasRemoteChanges: false;
	property bool tableViewParamsStoredServer: true;
	property bool showRemoteChangesAlert: true

	property string gqlGetListCommandId: root.collectionId + "List";

	// Invisible fields that will be requested for collection
	property var additionalFieldIds: ["id", "name"]
	property var requestedFields: []

	property alias subscriptionCommandId: collectionChangeListener.gqlCommandId

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
		return {}
	}

	dataControllerComp: Component {
		CollectionRepresentation {
			collectionId: root.collectionId;
			gqlGetListCommandId: root.gqlGetListCommandId
			requestedFields: root.requestedFields

			additionalFieldIds: root.additionalFieldIds;

			onRemoved: {
				root.removed()
			}

			function getHeaders(){
				return root.getHeaders();
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

		function getHeaders() {
			return root.getHeaders()
		}

		onChanged: {
			root.collectionChanged(changeInfo)
			root.handleSubscription(rawData, changeInfo)
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
