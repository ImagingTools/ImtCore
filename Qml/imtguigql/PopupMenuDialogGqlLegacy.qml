import QtQuick 2.12
import Qt5Compat.GraphicalEffects 6.0
import QtGraphicalEffects 1.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0

Item {
	id: popupMenuContainer;


	width: itemWidth;
	height: filterField.height + itemBody.height;

	property var model;
	//property TreeItemModel model: TreeItemModel{};
	property Item root: null;
	property Item rootItem: null;

	property int itemWidth: Style.sizeHintXXS;
	property int itemHeight: 26;
	property int filterHeight: Style.controlHeightM;
	property int offset: 0;
	property int delegateRadius: 0;
	property int textSize: Style.fontSizeM;
	property int count: 15;
	property string commandId: "";
	property string filterName: "name";
	property string fontColor: Style.textColor;
	property alias filterText: filterField.text;
	property string excludeFilterPart: "";
	property bool endListStatus: false;
	property bool hiddenBackground: true;
	property bool canClose: true;
	property bool canUpdateModel: true;
	property bool preventFirstLoading: false;
	property bool doNotCorrectPosition : false;
	property string additionalFilter: "";

	property alias modelFilterAlias: modelFilter;

	property int elementsCount: -1;
	property int pauseDuration: 500;
	property real contentHeight: itemBody.height;
	property bool pinned: true;


	onElementsCountChanged: {
		//console.log("__________ELEMENTS_COUNT____________",elementsCount);
	}

	property bool ready: root && rootItem && gettedParams && filterIdsModel && properties;

	// ID for display in combo box delegates
	property string nameId: "name";


	property Component delegate: PopupMenuDelegate{
		width: popupMenuContainer.width;
		height: popupMenuContainer.itemHeight;

		text: model[popupMenuContainer.nameId];

		selected: popupMenuContainer.selectedIndex == model.index;

		onClicked: {
			let resultId = model.Id || model.id || "";

			popupMenuContainer.rootItem.finished(resultId, model.index)
		}

		onEntered: {
			popupMenuContainer.selectedIndex = model.index;
		}
	}


	property TreeItemModel properties: TreeItemModel{};
	property TreeItemModel gettedParams: TreeItemModel{};
	property TreeItemModel filterIdsModel: TreeItemModel{};

	/**
		Count of the visible item, if value = -1 then count unlimited
	*/
	property int countVisibleItem: -1;

	property int selectedIndex: -1;

	property bool hoverBlocked: true;

	property bool notSetProperties: true;
	property bool isUpwards: false;

	signal finished(string commandId, int index);
	signal endList();
	signal textEdited();
	signal started();
	signal clearSignal();
	signal propertiesChangedSignal();
	signal destructionSignal();


	function onBackgroundClicked(){
		popupMenuContainer.root.closeDialog();
	}

	function updateModel(){
		itemsModel.updateModel(0);
	}

	Component.onCompleted: {
		Events.subscribeEvent("AppSizeChanged", onAppSizeChanged);
	}

	Component.onDestruction: {
		Events.unSubscribeEvent("AppSizeChanged", onAppSizeChanged);
	}


	function onAppSizeChanged(parameters){
		onBackgroundClicked();
	}

	function onBackgroundClicked(events){
		popupMenuContainer.finished('', -1);
	}

	TreeItemModel{
		id: modelFilter;
	}


	Component.onCompleted: {
		//console.log("_____________POPUP_COMPL_____________", popupMenuContainer.preventFirstLoading);
		popupMenuContainer.forceActiveFocus();
		modelFilter.addTreeModel("FilterIds");
		modelFilter.setData("FilterIds", popupMenuContainer.filterIdsModel)
		modelFilter.addTreeModel("Sort");

		if(!popupMenuContainer.preventFirstLoading){
			itemsModel.updateModel(0);
		}
	}

	Component.onDestruction: {
		destructionSignal();
	}


	onPropertiesChanged: {
		//console.log("_____________PROPERTIES_CHANGED_____________")

		for (var item = 0; item < popupMenuContainer.properties.getItemsCount(); item++){
			modelFilter.setData(popupMenuContainer.properties.getData("id", item),  popupMenuContainer.properties.getData("Value", item));
		}
		if(!notSetProperties){
			propertiesChangedSignal();
		}
		notSetProperties = false;
		//itemsModel.updateModel(0);
		//popupMenuContainer.rootItem.currentIndex = -1;
	}
	//    onGettedParamsChanged: {
	//        popupMenuContainer.rootItem.currentIndex = -1;
	//    }
	//    onFilterIdsModelChanged: {
	//        popupMenuContainer.rootItem.currentIndex = -1;

	//    }


	onFinished: {
		//        if(popupMenuContainer.canClose){
		//            popupMenuContainer.root.closeDialog();
		//        }
	}

	function setTextFocus(focus){
		filterField.setFocus(focus);
	}

	CustomTextField {
		id: filterField;

		z: 100;
		width: parent.width;
		height: popupMenuContainer.filterHeight;
		margin: 10;
		radius: popupMenuContainer.delegateRadius;
		anchors.top: parent.top;
		anchors.left: parent.left;
		textSize: popupMenuContainer.textSize;
		fontColor: popupMenuContainer.fontColor;

		//        onTextEdited:  {
		//            console.log("______TEXT_EDITED______")
		//        }
		onTextChanged:  {

			if(popupMenuContainer.rootItem){
				popupMenuContainer.rootItem.filterText = text;
				popupMenuContainer.rootItem.currentIndex = -1;
				popupMenuContainer.rootItem.currentText = text;
			}

			if(popupMenuContainer.ready){
				popupMenuContainer.rootItem.currentIndex = -1;
				popupMenuContainer.offset = 0;
				//modelFilter.setData("TextFilter", popupMenuContainer.filterText);
				var str = text.replace(popupMenuContainer.excludeFilterPart, "");
				modelFilter.setData("TextFilter", str);
				popupMenuContainer.rootItem.editSignal();
				pause.stop();
				pause.start();
			}
		}
		onAccepted: {
			popupMenuContainer.rootItem.closeSignal(popupMenuContainer.model);
			popupMenuContainer.root.closeDialog();
		}

		onTextInputFocusChanged: {
			popupMenuContainer.rootItem.inFocus = filterField.textInputFocus;
		}


	}

	PauseAnimation {
		id: pause;

		duration: 400;
		onFinished:  {
			if(popupMenuContainer.canUpdateModel){
				popupMenuContainer.updateModel(0);
			}
		}
	}


	Item{
		id: clearContainer;

		anchors.right: filterField.right;
		anchors.verticalCenter: filterField.verticalCenter;
		anchors.rightMargin: Style.marginXS;

		z: filterField.z +1;

		width: 12;
		height: 12;
		visible: filterField.text !=="";

		Image {
			id: clearIcon;

			anchors.fill: parent;

			source: "../../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);

			sourceSize.width: width;
			sourceSize.height: height;

			visible: parent.visible;
		}

		MouseArea {
			id: clearMouseArea;

			anchors.fill: parent;
			hoverEnabled: true;

			cursorShape: Qt.PointingHandCursor;

			visible: parent.visible;

			onClicked: {
				//console.log("clearMouseArea")
				popupMenuContainer.clearSignal();
				filterField.text = "";
			}

		}

	}




	Rectangle {
		id: itemBody;

		anchors.top: !popupMenuContainer.isUpwards ? filterField.bottom : undefined;
		anchors.bottom: popupMenuContainer.isUpwards ? filterField.top : undefined;
		anchors.left: parent.left

		width: popupMenuContainer.width;
		height: !noDataRec.visible * popupMenuListView.height + noDataRec.height * noDataRec.visible;
		radius: popupMenuContainer.delegateRadius;

		color: Style.baseColor;

		border.width: 1;
		border.color: Style.alternateBaseColor;

		Rectangle{
			id: noDataRec;

			width: parent.width;
			height: 50;
			radius: parent.radius;
			color: parent.color;

			visible: false;

			Text{
				id: noDataText;

				anchors.centerIn: parent;

				font.pixelSize:  popupMenuContainer.textSize;
				color: popupMenuContainer.fontColor;

				text: "Нет данных";
			}
		}

		Rectangle{
			id: loadedRec;

			anchors.fill: parent;
			opacity: 0.5;
			color: "transparent";
			visible: false;

			Text {

				anchors.centerIn: parent;

				color: Style.textColor;
				font.pixelSize: Style.fontSizeM;
				font.family: Style.fontFamily;

				text: "Loaded..."
			}
		}


		ListView {
			id: popupMenuListView;

			width: popupMenuContainer.width;
			height: !count ? 0 :(popupMenuContainer.countVisibleItem == -1 || popupMenuContainer.countVisibleItem > popupMenuListView.count) ?
								 popupMenuListView.count * popupMenuContainer.itemHeight :
								 popupMenuContainer.countVisibleItem * popupMenuContainer.itemHeight;

			boundsBehavior: Flickable.StopAtBounds;
			clip: true;
			model: popupMenuContainer.model;

			onContentYChanged: {
				if(popupMenuContainer.ready){
					if(contentHeight - contentY - popupMenuListView.height == 0){
						popupMenuContainer.createAdditionalQuery();
					}
				}
			}
			//delegate: PopupMenuDelegate{textSize: popupMenuContainer.textSize}// Delegate Item
			delegate: popupMenuContainer.delegate;


		}//ListView

		MouseArea{
			anchors.fill: parent;
			hoverEnabled: true;
			visible: popupMenuContainer.rootItem ? popupMenuContainer.rootItem.hoverBlocked :
												   popupMenuContainer.hoverBlocked;
			onPositionChanged: {
				popupMenuContainer.hoverBlocked = false;
				if(popupMenuContainer.rootItem){
					popupMenuContainer.rootItem.hoverBlocked = false;
				}
			}
		}
	}//ItemListView



	DropShadow {
		id: dropShadow;

		anchors.fill: itemBody;

		horizontalOffset: 2;
		verticalOffset: popupMenuContainer.isUpwards ? -2 : 2;

		radius: 4;
		color: Style.shadowColor;

		source: itemBody;
	}

	function createAdditionalQuery(){
		var elemCountOk = popupMenuContainer.elementsCount >= 0 ? popupMenuContainer.elementsCount > popupMenuContainer.offset : true;
		var ok = !popupMenuContainer.endListStatus
				&& itemsModel.state == "Ready"
				&& elemCountOk;
		if(ok){
			loadedRec.visible = true;
			popupMenuContainer.offset = popupMenuContainer.offset + popupMenuContainer.count;
			itemsModel.updateModel(popupMenuContainer.offset);
		}
	}

	function contentYCorrection(down_){
		if(popupMenuContainer.rootItem){
			if(popupMenuContainer.rootItem.selectedIndex >=0){
				var contentY = popupMenuListView.contentY;
				var itemHeight = popupMenuContainer.itemHeight;
				var visibleCount = popupMenuContainer.countVisibleItem;
				var index = popupMenuContainer.rootItem.selectedIndex;
				if(down_){
					if((index+1) * itemHeight > contentY + visibleCount * itemHeight){
						popupMenuListView.contentY = (index+1) * itemHeight - visibleCount * itemHeight
					}
				}
				else {
					if(index * itemHeight < contentY){
						popupMenuListView.contentY = index * itemHeight
					}
				}
			}

		}
	}

	Shortcut {
		sequence: "Escape";
		enabled: true;
		onActivated: {
			popupMenuContainer.root.closeDialog();
		}
	}

	Shortcut {
		sequence: "Return";
		enabled: !filterField.textInputFocus;
		onActivated: {
			if(popupMenuContainer.rootItem){
				if(popupMenuContainer.rootItem.selectedIndex >=0){
					var id = popupMenuContainer.model.getData("id", popupMenuContainer.rootItem.selectedIndex);
					popupMenuContainer.finished(id, popupMenuContainer.rootItem.selectedIndex);
				}
			}
		}
	}

	Shortcut {
		sequence: "Up";
		enabled: true;
		onActivated: {
			if(filterField.textInputFocus){
				popupMenuContainer.setTextFocus(false)
			}
			popupMenuContainer.hoverBlocked = true;
			if(popupMenuContainer.rootItem){
				popupMenuContainer.rootItem.hoverBlocked = true;
				if(popupMenuContainer.rootItem.selectedIndex > 0){
					popupMenuContainer.rootItem.selectedIndex--;
					popupMenuContainer.contentYCorrection(false);
				}
			}

		}
	}
	Shortcut {
		sequence: "Down";
		enabled: true;
		onActivated: {
			if(filterField.textInputFocus){
				popupMenuContainer.setTextFocus(false)
			}
			popupMenuContainer.hoverBlocked = true;
			if(popupMenuContainer.rootItem){
				popupMenuContainer.rootItem.hoverBlocked = true;
				if(popupMenuContainer.rootItem.selectedIndex < popupMenuContainer.model.getItemsCount() - 1){
					popupMenuContainer.rootItem.selectedIndex++;
					popupMenuContainer.contentYCorrection(true);
				}
				else if(popupMenuContainer.rootItem.selectedIndex == popupMenuContainer.model.getItemsCount() - 1){
					if(popupMenuContainer.rootItem.selectedIndex >=0 && popupMenuContainer.elementsCount -1 > popupMenuContainer.rootItem.selectedIndex){
						popupMenuContainer.createAdditionalQuery();
					}
					//popupMenuListView.contentY += popupMenuContainer.itemHeight;
				}

			}
		}
	}


	//    PauseAnimation {
	//        id: contentYCorrectionPause;
	//        duration: 200
	//    }

	GqlModel {
		id: itemsModel;

		function updateModel(offsetVar){
			//console.log("______UPDATE_MODEL_____", offsetVar);
			var query = Gql.GqlRequest("query", popupMenuContainer.commandId);

			var inputParams = Gql.GqlObject("input");
			query.AddParam(inputParams);
			var viewParams = Gql.GqlObject("viewParams");
			viewParams.InsertField("offset", offsetVar);
			viewParams.InsertField("count", popupMenuContainer.count);
			viewParams.InsertField("FilterModel");
			if(popupMenuContainer.additionalFilter != ""){
				viewParams.InsertField("AdditionalFilter", popupMenuContainer.additionalFilter);
			}
			var jsonString = modelFilter.toJson();
			//            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")
			viewParams.InsertField("FilterModel", jsonString);
			inputParams.InsertFieldObject(viewParams);
			var queryFields = Gql.GqlObject("items");

			for (var item = 0; item < popupMenuContainer.gettedParams.getItemsCount(); item++){
				var nameParam = popupMenuContainer.gettedParams.getData("name", item);
				queryFields.InsertField(nameParam);
			}

			query.AddField(queryFields)
			var gqlData = query.GetQuery();

			//console.log("COMBO_QUERY ", gqlData);

			this.setGqlQuery(gqlData);
		}

		onStateChanged: {
			//console.log("State:", this.state, itemsModel);
			if (this.state === "Ready"){

				let dataModelLocal;

				if (itemsModel.containsKey("errors")){
					return;
				}


				if (itemsModel.containsKey("data")){

					dataModelLocal = itemsModel.getData("data");

					if (dataModelLocal.containsKey(popupMenuContainer.commandId)){
						dataModelLocal = dataModelLocal.getData(popupMenuContainer.commandId);
						var isClosing = false;
						if (dataModelLocal.containsKey("notification")){
							var notifModel = dataModelLocal.getData("notification");
							if (notifModel.containsKey("ElementsCount")){
								popupMenuContainer.elementsCount = notifModel.getData("ElementsCount");
							}
							if(notifModel.containsKey("Close")){
								if(notifModel.getData("Close")){
									//console.log("CLOSE::::")
									isClosing = true;
									//popupMenuContainer.rootItem.closeFunc();
								}
							}
						}

						dataModelLocal = dataModelLocal.getData("items");

						//console.log(popupMenuContainer.commandId, " = ", dataModelLocal);
						//console.log("comboModel:::", dataModelLocal.toJson());
						if (popupMenuContainer.offset == 0){
							popupMenuContainer.model = dataModelLocal;
							//popupMenuListView.model = popupMenuContainer.model;

							loadedRec.visible = false;
							popupMenuContainer.endListStatus = false;
							popupMenuContainer.rootItem.selectedIndex = -1;
							if(isClosing){
								popupMenuContainer.rootItem.closeFunc();
							}
						}
						else{//OFSET !== 0

							//console.log("count items = ", dataModelLocal.getItemsCount(), dataModelLocal.toJson())

							if(dataModelLocal.getItemsCount() > 0){
								for (var i = 0; i < dataModelLocal.getItemsCount(); i++){
									var index_ = popupMenuContainer.model.insertNewItem();
									//dataModelLocal.copyItemDataToModel(i, popupMenuContainer.model, popupMenuContainer.offset + i);
									dataModelLocal.copyItemDataToModel(i, popupMenuContainer.model, index_);
								}
								//popupMenuListView.model = popupMenuContainer.model;
								popupMenuContainer.endListStatus = false;

							}
							else{
								popupMenuContainer.endListStatus = true;
							}
							loadedRec.visible = false;

							if(popupMenuContainer.rootItem.currentIndex >=0 && popupMenuContainer.elementsCount -1 > popupMenuContainer.rootItem.currentIndex){
								//popupMenuListView.contentY += popupMenuContainer.itemHeight;

							}
						}
					}
					noDataRec.visible = !popupMenuContainer.model.getItemsCount();
					dataModelLocal.refresh();
					//console.log("popupMenuContainer.model.getItemsCount()::", popupMenuContainer.model.getItemsCount())
				}


			}
		}
	}


}
