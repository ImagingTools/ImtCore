import QtQuick 2.12

import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0


Rectangle{
	id: treeBody

	property string context: ""
	clip: true
	radius: 20;
	color: Style.baseColor;

	property string addressListCommandId: "AddressList"
	property string subscriptionCommandId: "OnAddressCollectionChanged"

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
	property string textColor: Style.firstColor
	property var searchFields: [
		idParam, fullAddressParam,
		latitudeParam, longitudeParam,
		typeIdParam, parentIdsParam
	];
	property alias searchComp: searchComp
	property alias searchCompModel: searchComp.model
	property alias treeViewModel: treeView.model
	property alias treeView: treeView
	property alias treeLoading: treeLoading
	property alias moveToAnim: moveToAnim
	property Component addressTreeInputObjectComp: null
	property Component addressTreeSdlObjectComp: null
	property Component additionalDelegateComp: Component{Item{}}

	property GqlPureRequestSender addressTreeRequest : GqlPureRequestSender {
		context: treeBody.context
		property string parentIds: "";
		property string idsToOpen: "";
		property int insertIndex: -1;
		traceTiming: true

		gqlCommandId: treeBody.addressListCommandId

		property var customFinishedCallback: null

		function resultItems(){
			let payload = result;
			if (!payload){
				return [];
			}
			if (payload.items && payload.items.length !== undefined){
				return payload.items;
			}
			if (payload.length !== undefined){
				return payload;
			}
			return [];
		}

		function defaultFinishedCallback(status, insertIndex, idsToOpen) {
			treeLoading.visible = false;
			if (status !== 1) {
				return;
			}

			let items = resultItems();
			let itemCount = items.length;
			let buildStarted = treeBody.addressTreeRequest.traceNow();

			if (itemCount === 0 && insertIndex === -1) {
				treeView.model.clear();
				treeView.selectedIndex = -1;
			}

			if (itemCount > 0) {
				treeView.insertTree(insertIndex, items);
			}

			console.log("AddressTree: сборка дерева", Math.round(treeBody.addressTreeRequest.traceNow() - buildStarted), "мс, узлов", itemCount, "index", insertIndex)

			if (treeView.model.getItemsCount() > 0 && idsToOpen !== "") {
				let ids_ = idsToOpen;
				treeBody.addressTreeRequest.idsToOpen = "";
				treeBody.openNestedTree(ids_);
			}

			if (treeView.model.getItemsCount() <= 0 || idsToOpen === "") {
				treeBody.addressTreeRequest.idsToOpen = "";
			}
		}

		onFinished: {
			let reqStatus = status;
			let reqInsertIndex = insertIndex;
			let reqIdsToOpen = idsToOpen;
			let responseAt = treeBody.addressTreeRequest.traceResponseAt;

			if (customFinishedCallback) {
				customFinishedCallback(reqStatus, result, reqInsertIndex, resultItems(), reqIdsToOpen);
				return;
			}

			defaultFinishedCallback(reqStatus, reqInsertIndex, reqIdsToOpen);

			if (responseAt > 0){
				console.log("AddressTree: всего после ответа", Math.round(treeBody.addressTreeRequest.traceNow() - responseAt), "мс")
			}
		}

		function createQueryParams(query){
			if (treeBody.addressTreeInputObjectComp){
				let inputObject = treeBody.addressTreeInputObjectComp.createObject(addressTreeRequest);
				let gqlObject = Gql.GqlObject("input");
				gqlObject.fromObject(inputObject);
				query.AddParam(gqlObject);
				inputObject.destroy();
				return;
			}
			query.AddParam(inputParams);
		}

		function getRequestedFields(){
			let items = Gql.GqlObject("items");
			treeBody.addQueryField(items, treeBody.idParam);
			treeBody.addQueryField(items, treeBody.nameParam);
			treeBody.addQueryField(items, treeBody.fullAddressParam);
			treeBody.addQueryField(items, treeBody.latitudeParam);
			treeBody.addQueryField(items, treeBody.longitudeParam);
			treeBody.addQueryField(items, treeBody.typeIdParam);
			treeBody.addQueryField(items, "typeId__");
			treeBody.addQueryField(items, treeBody.parentIdsParam);
			treeBody.addQueryField(items, treeBody.hasChildrenParam);
			treeBody.addQueryField(items, "activeDevCount");
			treeBody.addQueryField(items, "inactiveDevCount");

			let fields = Gql.GqlObject("");
			fields.InsertFieldObject(items);
			return fields;
		}

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

	function addQueryField(gqlObject, name){
		if (!name){
			return;
		}
		gqlObject.InsertField(name);
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

	function openNestedTree(ids, startIndex){
		treeBody.idsToOpen = ids;
		let id = getFirstIdToOpen();
		if(id === ""){
			return;
		}
		let index = treeView.findIndexById(id, "id", startIndex);
		treeView.indexToMove = index;
		treeView.openFunc(index);
	}

	function openReaction(index){
		if(treeBody.idsToOpen !== ""){
			treeBody.removeFirstIdToOpen();
			if(treeBody.idsToOpen !== ""){
				treeBody.openNestedTree(treeBody.idsToOpen, index + 1)
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
		gqlCommandId: treeBody.subscriptionCommandId

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

	TreeViewPure{
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

		color: Style.baseColor;
		selectionColor: Style.selectedColor
		hoverColor: Style.selectedColor;
		textColor: treeBody.textColor
		selectionRadius: 4;
		scrollIndicatorColor: Style.firstColor;
		scrollBackgroundColor: Style.scrollBackgroundColor;
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
