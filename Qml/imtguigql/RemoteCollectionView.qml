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

	property string gqlGetListCommandId: root.collectionId + "List";

	// Invisible fields that will be requested for collection
	property var additionalFieldIds: ["id", "name"]

	signal removed(string objectId)

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

	onCollectionIdChanged: {
		if (collectionId !== ""){
			subscriptionClient.gqlCommandId = "On" + root.collectionId + "CollectionChanged";
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
		return {};
	}

	dataControllerComp: Component {
		CollectionRepresentation {
			property bool isReady: false;

			collectionId: root.collectionId;
			gqlGetListCommandId: root.gqlGetListCommandId

			additionalFieldIds: root.additionalFieldIds;

			onRemoved: {
				root.removed(objectId)
			}

			function getHeaders(){
				return root.getHeaders();
			}
		}
	}

	Component.onCompleted: {
		table.saveWidth.connect(root.tableViewParamsAccepted)
		Events.subscribeEvent("UpdateAllModels", root.receiveRemoteChanges);
	}

	Component.onDestruction: {
		table.saveWidth.disconnect(root.tableViewParamsAccepted)
		Events.unSubscribeEvent("UpdateAllModels", root.receiveRemoteChanges);
	}

	onHasRemoteChangesChanged: {
		if (root.visible && hasRemoteChanges){
			root.setAlertPanel(alertPanelComp)
		}
	}

	onSelectionChanged: {
		if (!root.visibleMetaInfo){
			return;
		}

		if (selection.length === 1){
			let index = selection[0];
			let elementsModel = root.table.elements;
			let elementId = elementsModel.getData("id", index);
			metaInfoProvider.getMetaInfo(elementId);
		}
	}

	onVisibleChanged: {
		if (hasRemoteChanges && visible){
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

	MetaInfoProvider {
		id: metaInfoProvider;

		getMetaInfoGqlCommand: root.collectionId + "MetaInfo";

		onMetaInfoModelChanged: {
			root.metaInfoView.metaInfoModel = metaInfoProvider.metaInfoModel
		}

		onStateChanged: {
			if (state === "Loading"){
				root.metaInfoView.startLoading();
			}
			else{
				root.metaInfoView.stopLoading();
			}
		}

		function getHeaders(){
			return root.getHeaders();
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
				anchors.leftMargin: Style.sizeMainMargin;

				width: 20;
				height: 20;

				sourceSize.height: height;
				sourceSize.width: width;

				source: "../../../" + Style.getIconPath("Icons/Lamp", Icon.State.On, Icon.Mode.Normal);
			}

			BaseText {
				id: message;

				anchors.verticalCenter: parent.verticalCenter;
				anchors.left: icon.right;
				anchors.leftMargin: Style.sizeMainMargin;
				anchors.right: updateButton.left;

				text: qsTr("This table has been modified from another computer");
			}

			Button {
				id: updateButton;

				anchors.verticalCenter: parent.verticalCenter;
				anchors.right: parent.right;
				anchors.rightMargin: Style.sizeMainMargin;

				width: 70;
				height: 30;

				text: qsTr("Update");

				onClicked: {
					root.receiveRemoteChanges()
				}
			}
		}
	}

	function handleSubscription(dataModel){
		if (!dataModel){
			return;
		}

		if (dataModel.containsKey("operationContext")){
			let operationContextInfo = dataModel.getData("operationContext");
			if (!operationContextInfo){
				return;
			}
			
			if (operationContextInfo.containsKey("ownerId")){
				let ownerId = operationContextInfo.getData("ownerId");
				let currentUserId = AuthorizationController.getLoggedUserId();
				if (ownerId == currentUserId){
					root.doUpdateGui();
				}
				else{
					root.hasRemoteChanges = true;
				}
			}
		}
		else{
			root.doUpdateGui();
		}
	}

	SubscriptionClient {
		id: subscriptionClient;

		function getHeaders(){
			return root.getHeaders();
		}

		onMessageReceived: {
			root.handleSubscription(data);
		}
	}
}
