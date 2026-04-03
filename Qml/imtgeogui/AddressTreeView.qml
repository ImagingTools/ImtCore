import QtQuick 2.12

import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0


Rectangle{
	id: treeBody

	clip: true
	radius: 20;
	color: Style.color_menu;

	property string addressListCommandId: "AddressList"
	property string searchNameId: "fullAddress"
	property string idParam: "id"
	property string nameParam: "name"
	property string fullAddressParam: "fullAddress"
	property string latitudeParam: "latitude"
	property string longitudeParam: "longitude"
	property string typeIdParam: "typeId"
	property string parentIdsParam: "parentIds"
	property string hasChildrenParam: "hasChildren__"
	property string idsToOpen: "";
	property var searchFields: [idParam, fullAddressParam,
		latitudeParam, longitudeParam,
		typeIdParam, parentIdsParam];
	property alias searchComp: searchComp
	property alias searchCompModel: searchComp.model
	property alias treeViewModel: treeView.model
	property alias treeView: treeView
	property alias treeLoading: treeLoading
	property alias moveToAnim: moveToAnim
	property Component addressTreeInputObjectComp: null
	property Component addressTreeSdlObjectComp: null
	property Component additionalDelegateComp: Component{Item{}}

	property GqlSdlRequestSender addressTreeRequest : GqlSdlRequestSender {
		property string parentIds: "";
		property string idsToOpen: "";
		property int insertIndex: -1;
		property TreeItemModel newTreeModel: TreeItemModel{}

		function reset(){
			treeBody.addressTreeRequest.insertIndex = -1;
			treeBody.addressTreeRequest.parentIds = "";
			treeBody.setExternalAddress("", "");
		}

		function updateModel(index_){
			if(index_ !== undefined){
				insertIndex = index_;
			}

			send()
			treeLoading.visible = true;
		}

		gqlCommandId: "AddressTreeList"
		inputObjectComp: treeBody.addressTreeInputObjectComp
		sdlObjectComp: treeBody.addressTreeSdlObjectComp
	}

	signal searchFinished(string itemId, int index)
	signal treeClicked(int index)
	signal treeViewSelectedIndexChanged(int index)
	signal searchFieldClear();
	signal treeViewInserted(int index);

	Component.onCompleted: {
		treeBody.addressTreeRequest.updateModel();
	}

	function setCustomFilters(inputParams){
		return
	}

	function setExternalAddress(address, parentIds){
		if(!parentIds && address === ""){
			searchComp.clearSignal();
		}
		else {
			searchComp.setExternalAddress(address, parentIds);
		}
	}

	function getFirstIdToOpen(){
		let index = treeBody.idsToOpen.indexOf(",");
		let retVal = treeBody.idsToOpen;
		if(index >= 0){
			retVal = treeBody.idsToOpen.slice(0, index);
		}
		return retVal;
	}

	function removeFirstIdToOpen(){
		let index = treeBody.idsToOpen.indexOf(",");
		if(index >= 0){
			treeBody.idsToOpen = treeBody.idsToOpen.slice(index + 1);
		}
		else {
			treeBody.idsToOpen = "";
		}
	}

	function openNestedTree(ids){
		treeBody.idsToOpen = ids;
		let id = getFirstIdToOpen();
		if(id === ""){
			return;
		}
		let index = treeView.findIndexById(id);
		treeView.indexToMove = index;
		treeView.openFunc(index);
	}

	function openReaction(index){
		if(treeBody.idsToOpen !== ""){
			treeBody.removeFirstIdToOpen();
			if(treeBody.idsToOpen !== ""){
				treeBody.openNestedTree(treeBody.idsToOpen)
			}
			else {
				treeView.selectedIndex = index;
			}
		}

		if(treeView.indexToMove >= 0 && treeBody.idsToOpen == ""){
			moveToAnim.restart();
		}
	}

	function handleSubscription(dataModel){
		if (!dataModel){
			return;
		}

		else{
			treeBody.addressTreeRequest.reset()
			let openedIdsString = treeView.getOpenedIds()
			treeBody.addressTreeRequest.idsToOpen = openedIdsString

			treeBody.addressTreeRequest.updateModel();
		}
	}

	SubscriptionClient {
		id: subscriptionClient;
		gqlCommandId: "OnAddressCollectionChanged"

		onMessageReceived: {
			treeBody.handleSubscription(data);
		}
	}

	Image{
		id: searchIcon;

		anchors.left: parent.left;
		anchors.leftMargin: Style.marginM;
		anchors.verticalCenter: searchComp.verticalCenter;

		height: 20;
		width: height;

		sourceSize.width: width;
		sourceSize.height: height;

		source: "../../../" +  Style.getIconPath("Icons/Search", Icon.State.Off, Icon.Mode.Disabled);
	}

	SearchAddressComp {
		id: searchComp;

		anchors.top: parent.top;
		anchors.left: searchIcon.right;
		anchors.margins: Style.marginM;
		width: parent.width - anchors.leftMargin - x;

		commandId: treeBody.addressListCommandId
		placeHolderText: qsTr("Address");
		nameId: treeBody.searchNameId
		dataProvider.fields: treeBody.searchFields
		dataProvider.textFilteringInfoIds: [nameId];

		onClearSignal: {
			if(searchComp.isOpen){
				return
			}

			treeView.selectedIndex = -1;
			treeBody.searchFieldClear();
		}

		onFinished: {
			if(index < 0){
				return
			}

			treeBody.openNestedTree(itemId)
			treeBody.searchFinished(itemId, index)
		}

		function setCustomFilters(inputParams){
			inputParams.InsertField("parentIds", searchComp.parentIds);
			treeBody.setCustomFilters(inputParams)
		}
	}

	TreeViewGql{
		id: treeView;

		anchors.top: searchComp.bottom;
		anchors.topMargin: Style.marginXS;
		anchors.bottom: parent.bottom;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.leftMargin: Style.marginXS;
		anchors.rightMargin: Style.marginXS;

		radius: parent.radius;
		hasSelection: true;
		scrollSize: 8;

		color: Style.color_menu;
		selectionColor: "lightsteelblue"
		hoverColor: Style.selectedColor;
		textColor: Style.firstColor;
		selectionRadius: 4;
		scrollIndicatorColor: Style.firstColor;
		scrollBackgroundColor: "#ffffff";
		additionalDelegateComp: treeBody.additionalDelegateComp

		property int indexToMove: -1;

		onSelectedIndexChanged: {
			treeBody.treeViewSelectedIndexChanged(selectedIndex)
		}

		onForcedOpen: {
			treeBody.openReaction(index);
		}

		onInserted: {
			treeBody.openReaction(index);
			treeBody.treeViewInserted(index)
		}

		function getOpenedIds(){
			let countItems = treeView.getModelItemsCount(treeView.model)
			if (countItems < 0){
				return "";
			}
			else{
				let openedIds = ""
				for (var index = 0; index < countItems; index++){
					let isOpened = treeView.checkIsOpen(index)
					if (isOpened){
						openedIds += treeView.getData("id", index) + ","
					}
				}
				if (openedIds !== ""){
					openedIds = openedIds.slice(0, -1)
				}

				return openedIds
			}
		}

		PauseAnimation {
			id: moveToAnim;

			duration: 200;
			onFinished: {
				treeView.moveToElement(treeView.indexToMove);
				treeView.indexToMove = -1;
			}
		}

		onOpenButtonClicked: {
			let parentIds	= model.getData(treeBody.parentIdsParam, index);
			let address		= model.getData(treeBody.fullAddressParam, index);

			treeBody.setExternalAddress(address, parentIds);
		}

		onRequestSignal: {
			let id			= model.getData(treeBody.idParam, index);
			let parentIds	= model.getData(treeBody.parentIdsParam, index);
			let hasChildren = model.getData(treeBody.hasChildrenParam, index);

			if(hasChildren){
				if(parentIds === ""){
					treeBody.addressTreeRequest.parentIds = id;
				}
				else {
					treeBody.addressTreeRequest.parentIds = parentIds + "," + id;
				}
				treeBody.addressTreeRequest.insertIndex = index;
				treeBody.addressTreeRequest.updateModel();
			}
		}
		onClicked: {
			treeBody.treeClicked(index)
		}
	}

	Rectangle{
		anchors.fill: parent;
		color: Style.backgroundColor;
		opacity: 0.3;
		visible: treeLoading.visible;
	}

	Loading{
		id: treeLoading;

		anchors.fill: parent;
		visible: false;
		color: "transparent";
	}
}
