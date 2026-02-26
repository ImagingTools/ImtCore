import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0

Rectangle{
	id: searchContainer;

	width: Style.sizeHintM;
	height:  Style.controlHeightL;
	radius: Style.size_TextFieldRadius;
	border.color: Style.grayColor;
	border.width: 1;

	property int elementHeight: 60;

	property string fontColor: Style.textColor;
	property int textSize: Style.fontSizeM -2;

	property string commandId: "AddressesList";

	//
	property string gettedParam: "name";
	property string filterId: "name";
	property string propertyId: "";
	//

	property bool hasSearchProperty: false;
	property bool isAddressSearch: false;
	property bool canSearchAnyStr: false;
	property alias canClose: searchTextField.canClose;

	property string valueName: "name";
	property string retValName: "";

	property string externalSearchParam: "";

	property TreeItemModel gettedParamsModel: TreeItemModel{};
	property TreeItemModel filterIdsModel: TreeItemModel{};
	property TreeItemModel propertiesModel: TreeItemModel{};

	property TreeItemModel additionalFilterModel: TreeItemModel{};

	property TreeItemModel returnModel: TreeItemModel{};
	property bool canReturnModel: false;

	property string parentIds: "";
	property string excludeFilterPart: "";
	property string selectedText: "";
	property string additionalFilter: "";

	property bool isTextIncrease: false;
	property string previousText: "";

	property alias searchDelegate: searchTextField.delegate;
	property alias textField: searchTextField;
	property alias currentText: searchTextField.currentText;
	property alias placeHolderText: placeHolder.text;

	property alias itemHeight: searchTextField.itemHeight;
	property alias textFieldWidth: searchTextField.width;
	property alias textFieldTextSize: searchTextField.textSize;
	property alias arrowIconSource: searchTextField.imageSource;
	property alias arrowIconRotation: searchTextField.imageRotation;

	property alias openST: searchTextField.openST;

	signal accepted(string retVal);
	signal searchPartialAddress(string addressStr);
	signal returnModelSignal();
	signal clearSignal();
	signal closeSignal();

	Component.onCompleted: {
	}

	function clearSearchFunc(){
		console.log("clearSearchFunc")

		searchContainer.propertiesModel.clear();
		setPropertiesModel(searchContainer.propertyId, "");

		searchTextField.excludeFilterPart = "";
		searchContainer.externalSearchParam = "";
		searchTextField.currentText = "";
		searchTextField.filterText = "";
		searchContainer.selectedText = "";
		searchContainer.previousText = "";
		searchContainer.parentIds = "";

		searchContainer.accepted("");

		if(searchTextField.openST){
			var popup = ModalDialogManager.topItem;

			popup.modelFilterAlias.clear();
			popup.modelFilterAlias.addTreeModel("FilterIds");
			popup.modelFilterAlias.setData("FilterIds", searchContainer.filterIdsModel)
			popup.modelFilterAlias.addTreeModel("Sort");
			popup.modelFilterAlias.setData("ParentIds",  searchContainer.parentIds);
			searchContainer.setAdditionalFilterParamsForPopup(popup.modelFilterAlias);

			popup.modelFilterAlias.setData("TextFilter", "");
			popup.excludeFilterPart = searchContainer.currentText;
			popup.filterText = searchContainer.currentText;
		}
		searchContainer.clearSignal();
	}

	function clearSearchParams(){
		searchContainer.clearModels();

		searchTextField.excludeFilterPart = "";
		searchContainer.externalSearchParam = ""
		searchTextField.currentText = "";
		searchTextField.filterText = "";
		searchContainer.selectedText = "";
		searchContainer.previousText = "";
		searchContainer.parentIds = "";

	}

	function clearSearchParamsExeptModels(){

		searchTextField.excludeFilterPart = "";
		searchContainer.externalSearchParam = ""
		searchTextField.currentText = "";
		searchContainer.selectedText = "";
		searchTextField.filterText = "";
		searchContainer.previousText = "";
		searchContainer.parentIds = "";

	}

	function setGettedParamsModel(param){
		searchContainer.gettedParamsModel.clear();
		var index = searchContainer.gettedParamsModel.insertNewItem();
		searchContainer.gettedParamsModel.setData("name", param, index);
	}

	function setFilterIdsModel(id){
		searchContainer.filterIdsModel.clear();
		var index = searchContainer.filterIdsModel.insertNewItem();
		searchContainer.filterIdsModel.setData("id", id, index);
	}

	function setPropertiesModel(id_,value_){
		searchContainer.propertiesModel.clear();
		if(searchContainer.hasSearchProperty){
			var index = searchContainer.propertiesModel.insertNewItem();
			searchContainer.propertiesModel.setData("id", id_);
			searchContainer.propertiesModel.setData("value", value_);
			searchContainer.setAdditionalFilterParams();
		}

	}

	function setAdditionalFilterParams(){
		let keys = [];
		keys = searchContainer.additionalFilterModel.getKeys();
		for(let i = 0; i < keys.length; i++){
			let index_  = searchContainer.propertiesModel.insertNewItem();
			let key = keys[i];
			let value  = searchContainer.additionalFilterModel.getData(key);
			searchContainer.propertiesModel.setData("id", key, index_);
			searchContainer.propertiesModel.setData("value", value, index_);
		}
	}

	function setAdditionalFilterParamsForPopup(model_){
		let keys = [];
		keys = searchContainer.additionalFilterModel.getKeys();
		for(let i = 0; i < keys.length; i++){
			let key = keys[i];
			let value  = searchContainer.additionalFilterModel.getData(key);
			model_.setData(key, value);
		}
	}



	function setCurrentTextAddressFunc(modelll, index_, addStr_){
		let noTextChanges = false;

		console.log("setCurrentTextAddressFunc")
		if(searchContainer.isAddressSearch){
			//searchTextField.currentIndex = -1;
			var addStrNew = addStr_ !== undefined ? addStr_ : "";

			var tempStr = searchTextField.currentText = modelll.getData(searchContainer.valueName, index_) + addStrNew
			// if(tempStr[tempStr.length - 1] == ","){
			// 	tempStr = tempStr.slice(0, tempStr.length - 1);
			// }

			if(searchTextField.currentText === tempStr){
				noTextChanges = true;
			}
			searchTextField.currentText = tempStr;//modelll.getData(searchContainer.valueName, index_) + addStrNew;


			var parentIds__ = modelll.getData("parentIds",index_) !== undefined ? modelll.getData("parentIds", index_) : modelll.getData("ParentIds",index_) !== undefined ? modelll.getData("ParentIds",index_) :"";

			if(parentIds__ !== ""){
				parentIds__ = parentIds__ + ",";
			}
			var addressId__ = modelll.getData("id", index_) !== undefined ? modelll.getData("id", index_) : modelll.getData("Id", index_) !== undefined ? modelll.getData("Id", index_) : "";
			searchContainer.parentIds = parentIds__ + addressId__;
			searchContainer.selectedText = searchTextField.currentText;
			setPropertiesModel(searchContainer.propertyId, searchContainer.parentIds);
			//console.log("PARENT_IDS::", searchContainer.parentIds)

			searchContainer.externalSearchParam = searchContainer.parentIds;

			var retV;
			if(searchContainer.retValName !== ""){
				retV = modelll.getData(searchContainer.retValName, index_);
			}
			else {
				retV = searchContainer.externalSearchParam;
			}
			//console.log("searchContainer.externalSearchParam", searchContainer.externalSearchParam)

			if(searchContainer.canReturnModel){
				searchContainer.returnModel.clear();
				searchContainer.returnModel.copyItemDataFromModel(0, modelll, index_);
				searchContainer.returnModelSignal();
			}

			searchContainer.accepted(retV);

			if(searchTextField.openST){
				let popup = ModalDialogManager.topItem;

				popup.modelFilterAlias.clear();
				popup.modelFilterAlias.addTreeModel("FilterIds");
				popup.modelFilterAlias.setData("FilterIds", searchContainer.filterIdsModel)
				popup.modelFilterAlias.addTreeModel("Sort");
				popup.modelFilterAlias.setData("ParentIds",  searchContainer.parentIds);
				searchContainer.setAdditionalFilterParamsForPopup(popup.modelFilterAlias);

				popup.modelFilterAlias.setData("TextFilter", "");
				popup.excludeFilterPart = searchContainer.currentText;
				popup.filterText = searchContainer.currentText;

				if(noTextChanges){
					textChangeFunc(searchTextField.currentText);
					popup.updateModel()
				}
			}

		}

	}

	function setCurrentTextFunc(modelll, index_){
		searchTextField.currentText = modelll.getData(searchContainer.valueName, index_);
		var retV;
		if(searchContainer.retValName !== ""){
			retV = modelll.getData(searchContainer.retValName, index_);
		}
		else{
			retV = searchTextField.currentText;
		}
		searchContainer.accepted(retV);
	}

	function textChangeFunc(filterText){
		if(!searchTextField.openST){
			return;
		}
		if(!filterText){
			return;
		}
		//console.log("filterText:: " ,filterText);
		searchContainer.isTextIncrease = searchContainer.previousText.length < filterText.length;
		//var comaDeleted = searchContainer.previousText[searchContainer.previousText.length -1] == "," && filterText[filterText.length -1] !== ",";
		let comaCountPrev = searchContainer.arraySize(searchContainer.previousText);
		let comaCountNew = searchContainer.arraySize(filterText);
		let comaDeletedDiff = comaCountPrev - comaCountNew ;
		let comaDeleted = comaDeletedDiff > 0;
		searchContainer.previousText = filterText;
		//console.log("TEXT_CHANGE:: ", comaCountPrev, comaCountNew)
		//console.log("isTextIncrease:: " , searchContainer.isTextIncrease)

		if(searchContainer.isAddressSearch){
			if(searchTextField.openST){
				var popup = ModalDialogManager.topItem;

				if(!searchContainer.isTextIncrease){//NOT isTextIncrease
					var arrCount_prev = comaCountPrev//searchContainer.arraySize(searchContainer.selectedText);
					var arrCount = comaCountNew//searchContainer.newArrayCount(searchContainer.selectedText, filterText);
					//console.log("arrCount::: prev , curr ", arrCount_prev, arrCount);

					if(arrCount <= arrCount_prev){
						var keepCount = arrCount;
						if(comaDeleted){
							keepCount = keepCount - comaDeletedDiff;
						}
						if(keepCount < 0){
							keepCount = 0;
						}

						searchContainer.selectedText = filterText;

						if(comaDeleted){
							searchTextField.excludeFilterPart = searchContainer.keepNElements(popup.filterText, keepCount);
							searchContainer.parentIds = searchContainer.keepNElements(searchContainer.parentIds, keepCount);
							searchContainer.setPropertiesModel(searchContainer.propertyId, searchContainer.parentIds);
							popup.excludeFilterPart = searchTextField.excludeFilterPart;
							//console.log("excludeFilterPart:: ", searchTextField.excludeFilterPart)


							popup.modelFilterAlias.clear();
							popup.modelFilterAlias.addTreeModel("FilterIds");
							popup.modelFilterAlias.setData("FilterIds", searchContainer.filterIdsModel)
							popup.modelFilterAlias.addTreeModel("Sort");
							popup.modelFilterAlias.setData("ParentIds",  searchContainer.parentIds);
							searchContainer.setAdditionalFilterParamsForPopup(popup.modelFilterAlias);

							var str = filterText.replace(popup.excludeFilterPart, "");
							popup.modelFilterAlias.setData("TextFilter", str);
						}

						//updatePause.restart();
					}
				}

				else {//isTextIncrease
					var modelCount = popup.model.getItemsCount();
					if(modelCount){
						if(filterText[filterText.length -1] === ","){
							let str = filterText.slice(0,-1);
							var strArrCount = searchContainer.arraySize(str);
							//var newAddress = searchContainer.keepNElements(popup.model.getData(searchContainer.valueName),strArrCount)//;
							var newAddress = popup.model.getData(searchContainer.valueName);
							str = str.replace(/ +/g, '');
							newAddress = newAddress.replace(/ +/g, '');
							//console.log("ЗАПЯТАЯ " , "str: ", str, "newAddress: ", newAddress);
							if(str.toLowerCase() === newAddress.toLowerCase()){
								searchContainer.setCurrentTextAddressFunc(popup.model,0, ",");
							}
						}
					}
				}
			}
		}
	}


	function closeComboFunc(){
		if(!searchTextField.openST && searchTextField.currentText == "" ){
			searchTextField.excludeFilterPart = "";
			searchContainer.externalSearchParam = "";
			searchContainer.accepted("");
		}
		else if(!searchTextField.openST && searchTextField.currentText !== ""){
			// if(searchContainer.canClose){
			// 	searchContainer.accepted(searchTextField.currentText);
			// }
		}
	}


	function closeComboAddressFunc(){
		if(!searchTextField.openST && searchTextField.currentText == "" ){
			searchTextField.excludeFilterPart = "";
			searchContainer.externalSearchParam = "";
			searchContainer.accepted("");
		}
		else if(!searchTextField.openST && searchTextField.currentText !== ""){
			if(searchContainer.isAddressSearch){
				if(searchTextField.currentText[searchTextField.currentText.length - 1] === ","){
					searchTextField.currentText = searchTextField.currentText.slice(0, searchTextField.currentText.length - 1);
				}

				var textArraySize = searchContainer.arraySize(searchTextField.currentText);
				var parentIdsArraySize = searchContainer.arraySize(searchContainer.parentIds);
				searchTextField.excludeFilterPart = searchContainer.removeLastElement(searchTextField.currentText);
				if(textArraySize === parentIdsArraySize){
					searchContainer.parentIds = searchContainer.removeLastElement(searchContainer.parentIds);
				}
				setPropertiesModel(searchContainer.propertyId, searchContainer.parentIds);
				// searchContainer.externalSearchParam = searchContainer.parentIds;
				// searchTextField.excludeFilterPart = searchTextField.currentText;
				// setPropertiesModel(searchContainer.propertyId, searchContainer.parentIds);

				// console.log("secondSearch:: ", "parentIds:: " ,searchContainer.parentIds)
			}

		}

	}

	function removeLastAddressElement(){
		var textArraySize = searchContainer.arraySize(searchTextField.currentText);
		var parentIdsArraySize = searchContainer.arraySize(searchContainer.parentIds);
		searchTextField.excludeFilterPart = searchContainer.removeLastElement(searchTextField.currentText);
		if(textArraySize === parentIdsArraySize){
			searchContainer.parentIds = searchContainer.removeLastElement(searchContainer.parentIds);
		}
		setPropertiesModel(searchContainer.propertyId, searchContainer.parentIds);

	}

	function clearModels(){
		searchContainer.gettedParamsModel.clear();
		searchContainer.filterIdsModel.clear();
		searchContainer.propertiesModel.clear();
	}

	function setSearchModels(gettedParamModel_, filterIdsModel_, propertiesModel_){
		searchContainer.clearModels();

		for(let i = 0; i < gettedParamModel_.getItemsCount(); i++){
			searchContainer.gettedParamsModel.insertNewItem();
			searchContainer.gettedParamsModel.copyItemDataFromModel(i,gettedParamModel_,i);
		}
		for(let j = 0; j < filterIdsModel_.getItemsCount(); j++){
			searchContainer.filterIdsModel.insertNewItem();
			searchContainer.filterIdsModel.copyItemDataFromModel(j,filterIdsModel_,j);
		}
		if(propertiesModel_ !== null){
			for(let k = 0; k < propertiesModel_.getItemsCount(); k++){
				searchContainer.propertiesModel.insertNewItem();
				searchContainer.propertiesModel.copyItemDataFromModel(k,propertiesModel_,k);

			}
			searchContainer.setAdditionalFilterParams();
		}
	}

	function copyToGettedParamsModel(gettedParamModel_){
		searchContainer.gettedParamsModel.clear();
		for(let i = 0; i < gettedParamModel_.getItemsCount(); i++){
			searchContainer.gettedParamsModel.insertNewItem();
			searchContainer.gettedParamsModel.copyItemDataFromModel(i,gettedParamModel_,i);
		}
	}

	function copyToFilterIdsModel(filterIdsModel_){
		searchContainer.filterIdsModel.clear();
		for(let j = 0; j < filterIdsModel_.getItemsCount(); j++){
			searchContainer.filterIdsModel.insertNewItem();
			searchContainer.filterIdsModel.copyItemDataFromModel(j,filterIdsModel_,j);
		}
	}

	function copyToPropertiesModel(propertiesModel_){
		searchContainer.propertiesModel.clear();
		for(let k = 0; k < propertiesModel_.getItemsCount(); k++){
			searchContainer.propertiesModel.insertNewItem();
			searchContainer.propertiesModel.copyItemDataFromModel(k,propertiesModel_,k);
		}
		searchContainer.setAdditionalFilterParams();
	}


	////////////////////////////////////

	function removeComma(str){
		var newStr = str.trim();
		if(newStr[newStr.length - 1] === ","){
			newStr = newStr.slice(0, newStr.length - 1);
		}
		return newStr;
	}

	function removeLastElement(str){
		var arr  = str.split(",");
		arr.pop();
		var newStr = arr.join();
		//console.log("newStr::: ", newStr);
		return newStr;
	}

	function removeNElements(str, N){
		var arr  = str.split(",");
		var whileCount = 0;
		while(whileCount < N){
			arr.pop();
			whileCount++;
		}
		var newStr = "";
		if(arr.length > 0){
			newStr = arr.join();
		}
		//console.log("newStr::: ", newStr);
		return newStr;
	}

	function keepNElements(str, N){
		if(str === ""){
			return "";
		}
		var arr  = str.split(",");
		if(N >= arr.lenght){
			return str;
		}
		if(N <= 0){
			return "";
		}
		while(arr.length > N){
			arr.pop();
		}
		var newStr = "";
		if(arr.length > 0){
			newStr = arr.join();
		}
		console.log("newStr::: ", newStr);
		return newStr;
	}

	function arraySize(str){
		if(str === ""){
			return 0;
		}
		var arr  = str.split(",");
		//console.log("arraySize::: ", arr.length);
		return arr.length;
	}

	function getLastArrayElemet(str){
		var arr  = str.split(",");
		var newStr = "";
		if(arr.length){
			newStr = arr[arr.length -1];
		}
		//console.log("newStr::: ", newStr);
		return newStr;
	}

	function newArrayCount(str1, str2){
		var arr1  = str1.split(",");
		var arr2  = str2.split(",");
		var length_1 = arr1.length;
		var length_2 = arr2.length;
		var length_ = Math.min(arr1.length, arr2.length);
		var newArrayCount = length_;
		for (var i = 0; i < length_; i++){
			if(arr1[i] !== arr2[i]){
				newArrayCount = i;
				break;
			}
		}
		console.log("newArrayCount:: ", newArrayCount);
		return newArrayCount;
	}

	function comaCount(text){
		let length_ = text.length;
		let comaCount_ = 0;
		let target = ",";

		for (let i = 0; i < length_; i++){
			let pos = text[i].indexOf(target)
			if(pos > -1){
				comaCount_++;
			}
		}
		return comaCount_;
	}


	PauseAnimation {
		id: updatePause;

		duration: 400;
		onFinished: {
			if(searchTextField.openST && !searchTextField.canClose){
				var popup = ModalDialogManager.topItem;
				popup.updateModel();
			}
		}
	}

	ComboBoxGqlLegacy{
		id: searchTextField;

		anchors.horizontalCenter: parent.horizontalCenter;
		anchors.verticalCenter: parent.verticalCenter;

		width:  parent.width - 2*parent.border.width;
		height: parent.height - 2*parent.border.width;
		filterHeiht: height;

		commandId: searchContainer.commandId;
		count: 6;

		gettedParams: searchContainer.gettedParamsModel;
		properties: searchContainer.propertiesModel;
		filterIdsModel: searchContainer.filterIdsModel;

		radius: parent.radius;
		isColor: true;
		borderColor: "transparent";
		backgroundColor: "transparent";
		delegateRadius: radius;
		itemHeight: searchContainer.elementHeight;
		textSize: searchContainer.textSize;
		backVisible: !openST;
		complexModel: true;
		keepFilterText: true;
		preventFirstLoading:  filterText !== ""; //searchContainer.externalSearchParam !=="";
		closeEmpty: true;
		image.visible: !clearButton.visible;

		canClose: true;
		doNotCorrectPosition : true;
		additionalFilter: searchContainer.additionalFilter;

		onCloseEmptySignal:{
			if(searchContainer.isAddressSearch){
			}
		}

		onClearSignal:{
			//console.log("onClearSignal");
			searchContainer.clearSearchFunc();
		}

		onInFocusChanged: {

		}
		onFilterTextChanged: {
			//console.log("TEXT::_ ", filterText)
			searchContainer.textChangeFunc(filterText);
		}
		onSetCurrentText:{
			if(searchContainer.isAddressSearch){
				searchContainer.setCurrentTextAddressFunc(modelll, index);
			}
			else{
				searchContainer.setCurrentTextFunc(modelll, index);
			}
		}


		onCloseSignal:{
			console.log("CLOSE_SIGNAL")
			if(searchContainer.isAddressSearch){
				var modelCount = model_.getItemsCount();
				if(modelCount){

					var str = currentText;
					var newAddress = model_.getData(searchContainer.valueName)//,strArrCount;
					var str_form = str.replace(/ +/g, '');
					var newAddress_form = newAddress.replace(/ +/g, '');

					if(str_form === newAddress_form || modelCount === 1){
						searchContainer.setCurrentTextAddressFunc(model_,0);
					}
					else if(searchContainer.canSearchAnyStr){
						searchContainer.searchPartialAddress(currentText);
					}
				}
			}
		}

		function popupDestruction(){
			console.log("popupDestruction override")
			if(searchContainer.isAddressSearch){
				searchContainer.closeComboAddressFunc();
			}
		}

		delegate: Component{
			PopupMenuDelegate{
				id: searchDel;

				width: searchTextField.width;
				height: visible ? searchTextField.itemHeight : 0;
				// textSize: searchTextField.textSize;
				text: "";
				clip: true;
				rootItem: searchTextField;
				selected: searchTextField.selectedIndex === model.index;

				onClicked: {
					let resultId = model.id || model.id || "";
					searchTextField.finished(resultId, model.index)
				}
				onEntered: {
					searchTextField.selectedIndex = model.index;
				}

				Component.onCompleted:  {
				}

				Text{
					id: mainText;

					anchors.verticalCenter: parent.verticalCenter;
					anchors.left: parent.left;
					anchors.leftMargin: Style.marginM;
					anchors.right: parent.right;
					anchors.rightMargin: Style.marginM;

					wrapMode: Text.WordWrap;

					font.family: Style.fontFamily;
					color: searchContainer.fontColor;
					font.pixelSize: searchContainer.textSize;
					font.bold: false;
					z:20;

					text: model[searchContainer.valueName] !== undefined ?
							model[searchContainer.valueName]: "нет данных";
				}
			}
		} // delegate
	}

	Text {
		id: placeHolder;

		anchors.verticalCenter: parent.verticalCenter;
		anchors.left: parent.left;
		anchors.leftMargin: Style.marginM;
		anchors.right: parent.right;
		anchors.rightMargin: 2*Style.marginM;
		clip: true;

		font.family: Style.fontFamily;
		font.pixelSize: searchContainer.textSize;
		color: Style.placeHolderTextColor;
		visible: (!searchTextField.openST && searchTextField.currentText == "") ? true :
						(!searchTextField.openST && searchTextField.currentText !== "") ? false :
							(searchTextField.openST && searchTextField.currentText !== "") ? false : false;
		text: qsTr("Search");
	}

	Button{
		id: clearButton;

		anchors.verticalCenter: parent.verticalCenter;
		anchors.right: parent.right;
		anchors.rightMargin: Style.marginXS;

		width: height;
		height: 12;//Math.max(parent.height/2, 16);

		decorator: Component{IconButtonDecorator{}}

		iconSource: "../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);

		visible: searchTextField.currentText !== "" && !searchTextField.openST;
		enabled: visible;

		onClicked: {
			searchContainer.clearSearchFunc();
		}
	}

	function setAddressFromOutside(address, parentIds){
		searchContainer.parentIds = parentIds;
		searchTextField.currentText = address
		searchContainer.selectedText = searchTextField.currentText;
		setPropertiesModel(searchContainer.propertyId, searchContainer.parentIds);
		searchContainer.externalSearchParam = searchContainer.parentIds;
		searchTextField.excludeFilterPart = searchContainer.removeLastElement(searchTextField.currentText);
	}
}

