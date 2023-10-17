import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
import imtguigql 1.0

Rectangle{
    id: searchContainer;

    width: 400;
    height:  40;
    radius: Style.size_TextFieldRadius;
    border.color: Style.color_gray;
    border.width: 1;

    property int elementHeight: 60;

    property string fontColor: Style.textColor;
    property int textSize: Style.fontSize_common -2;

    property string commandId: "AddressesList";

    //
    property string gettedParam: "Name";
    property string filterId: "Name";
    property string propertyId: "";
    //

    property bool hasSearchProperty: false;
    property bool isAddressSearch: false;
    property bool canSearchAnyStr: false;
    property alias canClose: searchTextField.canClose;

    property string valueName: "Name";
    property string retValName: "";

    property string externalSearchParam: "";

    property TreeItemModel gettedParamsModel: TreeItemModel{};
    property TreeItemModel filterIdsModel: TreeItemModel{};
    property TreeItemModel propertiesModel: TreeItemModel{};

    property string parentIds: "";
    property string excludeFilterPart: "";
    property string selectedText: "";

    property bool isTextIncrease: false;
    property string previousText: "";

    property alias searchDelegate: searchTextField.delegate;
    property alias textField: searchTextField;
    property alias currentText: searchTextField.currentText;

    property alias itemHeight: searchTextField.itemHeight;
    property alias textFieldWidth: searchTextField.width;
    property alias textFieldTextSize: searchTextField.textSize;
    property alias arrowIconSource: searchTextField.imageSource;
    property alias arrowIconRotation: searchTextField.imageRotation;

    property alias openST: searchTextField.openST;

    signal accepted(string retVal);
    signal searchPartialAddress(string addressStr);

    Component.onCompleted: {
    }



    function clearSearchParams(){
        searchContainer.clearModels();

        searchTextField.excludeFilterPart = "";
        searchTextField.currentText = "";
        searchContainer.externalSearchParam = ""
        searchContainer.selectedText = "";

    }

    function setGettedParamsModel(param){
        searchContainer.gettedParamsModel.Clear();
        var index = searchContainer.gettedParamsModel.InsertNewItem();
        searchContainer.gettedParamsModel.SetData("Name", param, index);

    }

    function setFilterIdsModel(id){
        searchContainer.filterIdsModel.Clear();
        var index = searchContainer.filterIdsModel.InsertNewItem();
        searchContainer.filterIdsModel.SetData("Id", id, index);
    }

    function setPropertiesModel(id_,value_){
        searchContainer.propertiesModel.Clear();
        if(searchContainer.hasSearchProperty){
            var index = searchContainer.propertiesModel.InsertNewItem();
            searchContainer.propertiesModel.SetData("Id", id_);
            searchContainer.propertiesModel.SetData("Value", value_);
        }

    }

    function clearSearchFunc(){
        searchTextField.excludeFilterPart = "";
        searchTextField.currentText = "";
        searchContainer.selectedText = "";
        searchContainer.propertiesModel.Clear();
        searchContainer.parentIds = "";
        setPropertiesModel(searchContainer.propertyId, "");

        searchContainer.externalSearchParam = "";
        searchContainer.accepted("");

        if(searchTextField.openST){
            var popup = modalDialogManager.topItem;

            popup.modelFilterAlias.Clear();
            popup.modelFilterAlias.AddTreeModel("FilterIds");
            popup.modelFilterAlias.SetData("FilterIds", searchContainer.filterIdsModel)
            popup.modelFilterAlias.AddTreeModel("Sort");
            popup.modelFilterAlias.SetData("ParentIds",  searchContainer.parentIds);

            popup.modelFilterAlias.SetData("TextFilter", "");
            popup.excludeFilterPart = searchContainer.currentText;
            popup.filterText = searchContainer.currentText;
            popup.updateModel();
        }


    }

    function setCurrentTextAddressFunc(modelll, index_, addStr_){
        if(searchContainer.isAddressSearch){
            //searchTextField.currentIndex = -1;
            var addStrNew = addStr_ !== undefined ? addStr_ : "";

            var tempStr = currentText = modelll.GetData(searchContainer.valueName, index_) + addStrNew
            if(tempStr[tempStr.length - 1] == ","){
                tempStr = tempStr.slice(0, tempStr.length - 1);
            }

            searchTextField.currentText = tempStr;//modelll.GetData(searchContainer.valueName, index_) + addStrNew;

            var parentIds__ = modelll.GetData("ParentIds",index_) !== undefined ? modelll.GetData("ParentIds", index_) : "";
            if(parentIds__ !== ""){
                parentIds__ = parentIds__ + ",";
            }
            var addressId__ = modelll.GetData("Id", index_);
            searchContainer.parentIds = parentIds__ + addressId__;
            searchContainer.selectedText = searchTextField.currentText;
            setPropertiesModel(searchContainer.propertyId, searchContainer.parentIds);

            searchContainer.externalSearchParam = searchContainer.parentIds;

            var retV;
            if(searchContainer.retValName !== ""){
                retV = modelll.GetData(searchContainer.retValName, index_);
            }
            else {
                retV = searchContainer.externalSearchParam;
            }
            //console.log("searchContainer.externalSearchParam", searchContainer.externalSearchParam)
            searchContainer.accepted(retV);

            if(searchTextField.openST){
                var popup = modalDialogManager.topItem;

                popup.modelFilterAlias.Clear();
                popup.modelFilterAlias.AddTreeModel("FilterIds");
                popup.modelFilterAlias.SetData("FilterIds", searchContainer.filterIdsModel)
                popup.modelFilterAlias.AddTreeModel("Sort");
                popup.modelFilterAlias.SetData("ParentIds",  searchContainer.parentIds);

                popup.modelFilterAlias.SetData("TextFilter", "");
                popup.excludeFilterPart = searchContainer.currentText;
                popup.filterText = searchContainer.currentText;

                popup.updateModel();

            }

        }


    }

    function setCurrentTextFunc(modelll, index_){
        searchTextField.currentText = modelll.GetData(searchContainer.valueName, index_);
        var retV;
        if(searchContainer.retValName !== ""){
            retV = modelll.GetData(searchContainer.retValName, index_);
        }
        else{
            retV = searchTextField.currentText;
        }
        searchContainer.accepted(retV);
    }

    function textChangeFunc(filterText){
        //console.log("filterText:: " ,filterText);
        searchContainer.isTextIncrease = searchContainer.previousText.length < filterText.length;
        var comaDeleted = searchContainer.previousText[searchContainer.previousText.length -1] == "," && filterText[filterText.length -1] !== ",";
        searchContainer.previousText = filterText;

        console.log("isTextIncrease:: " , searchContainer.isTextIncrease)

        if(searchContainer.isAddressSearch){

            if(searchTextField.openST){
                var popup = modalDialogManager.topItem;

                if(!searchContainer.isTextIncrease){
                    var arrCount_prev = searchContainer.arraySize(searchContainer.selectedText);
                    var arrCount = searchContainer.newArrayCount(searchContainer.selectedText, filterText);
                    //console.log("arrCount::: prev , curr ", arrCount_prev, arrCount);

                    if(arrCount <= arrCount_prev){
                        var keepCount = arrCount;
                        if(comaDeleted){
                            keepCount--;
                        }

                        searchContainer.selectedText = filterText;
                        searchTextField.excludeFilterPart = searchContainer.keepNElements(popup.filterText, keepCount);
                        searchContainer.parentIds = searchContainer.keepNElements(searchContainer.parentIds, keepCount);
                        setPropertiesModel(searchContainer.propertyId, searchContainer.parentIds);
                        popup.excludeFilterPart = searchTextField.excludeFilterPart;

                        popup.modelFilterAlias.Clear();
                        popup.modelFilterAlias.AddTreeModel("FilterIds");
                        popup.modelFilterAlias.SetData("FilterIds", searchContainer.filterIdsModel)
                        popup.modelFilterAlias.AddTreeModel("Sort");
                        popup.modelFilterAlias.SetData("ParentIds",  searchContainer.parentIds);

                        var str = filterText.replace(popup.excludeFilterPart, "");
                        popup.modelFilterAlias.SetData("TextFilter", str);

                        updatePause.stop();
                        updatePause.start();

                    }


                }

                else {//isTextIncrease
                    var modelCount = popup.model.GetItemsCount();
                    if(modelCount){
                        if(filterText[filterText.length -1] == ","){
                            let str = filterText.slice(0,-1);
                            var strArrCount = searchContainer.arraySize(str);
                            var newAddress = searchContainer.keepNElements(popup.model.GetData(searchContainer.valueName),strArrCount)//;
                             str = str.replace(/ +/g, '');
                             newAddress = newAddress.replace(/ +/g, '');
                            //console.log("ЗАПЯТАЯ " , "str: ", str, "newAddress: ", newAddress);
                            //if(str !== newAddress){
                                searchContainer.setCurrentTextAddressFunc(popup.model,0, ",");
                            //}

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
            //searchContainer.updateModel("");
            searchContainer.accepted("");
        }
        else if(!searchTextField.openST && searchTextField.currentText !== ""){
//            if(searchContainer.canClose){
//                searchContainer.accepted(searchTextField.currentText);
//            }
        }
    }


    function closeComboAddressFunc(){
        if(!searchTextField.openST && searchTextField.currentText == "" ){
            searchTextField.excludeFilterPart = "";
            searchContainer.externalSearchParam = "";
            //searchContainer.updateModel("");
            searchContainer.accepted("");
        }
        else if(!searchTextField.openST && searchTextField.currentText !== ""){
            if(searchContainer.isAddressSearch){
                var textArraySize = searchContainer.arraySize(searchTextField.currentText);
                var parentIdsArraySize = searchContainer.arraySize(searchContainer.parentIds);
                searchTextField.excludeFilterPart = searchContainer.removeLastElement(searchTextField.currentText);
                if(textArraySize == parentIdsArraySize){
                    searchContainer.parentIds = searchContainer.removeLastElement(searchContainer.parentIds);
                }
                setPropertiesModel(searchContainer.propertyId, searchContainer.parentIds);

                //console.log("secondSearch:: ", "parentIds:: " ,searchContainer.parentIds)
            }

        }

    }

    function clearModels(){
        searchContainer.gettedParamsModel.Clear();
        searchContainer.filterIdsModel.Clear();
        searchContainer.propertiesModel.Clear();
    }

    function setSearchModels(gettedParamModel_, filterIdsModel_, propertiesModel_){
        searchContainer.clearModels();

        for(let i = 0; i < gettedParamModel_.GetItemsCount(); i++){
            searchContainer.gettedParamsModel.InsertNewItem();
            searchContainer.gettedParamsModel.CopyItemDataFromModel(i,gettedParamModel_,i);
        }
        for(let j = 0; j < filterIdsModel_.GetItemsCount(); j++){
            searchContainer.filterIdsModel.InsertNewItem();
            searchContainer.filterIdsModel.CopyItemDataFromModel(j,filterIdsModel_,j);
        }
        if(propertiesModel_ !== null){
            for(let k = 0; k < propertiesModel_.GetItemsCount(); k++){
                searchContainer.propertiesModel.InsertNewItem();
                searchContainer.propertiesModel.CopyItemDataFromModel(k,propertiesModel_,k);
            }
        }
    }

    function copyToGettedParamsModel(gettedParamModel_){
        searchContainer.gettedParamsModel.Clear();
        for(let i = 0; i < gettedParamModel_.GetItemsCount(); i++){
            searchContainer.gettedParamsModel.InsertNewItem();
            searchContainer.gettedParamsModel.CopyItemDataFromModel(i,gettedParamModel_,i);
        }
    }

    function copyToFilterIdsModel(filterIdsModel_){
        searchContainer.filterIdsModel.Clear();
        for(let j = 0; j < filterIdsModel_.GetItemsCount(); j++){
            searchContainer.filterIdsModel.InsertNewItem();
            searchContainer.filterIdsModel.CopyItemDataFromModel(j,filterIdsModel_,j);
        }
    }

    function copyToPropertiesModel(propertiesModel_){
        searchContainer.propertiesModel.Clear();
        for(let k = 0; k < propertiesModel_.GetItemsCount(); k++){
            searchContainer.propertiesModel.InsertNewItem();
            searchContainer.propertiesModel.CopyItemDataFromModel(k,propertiesModel_,k);
        }
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
        if(str == ""){
            return "";
        }
        var arr  = str.split(",");
        if(N >= arr.lenght){
            return str;
        }
        if(N === 0){
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
        if(str == ""){
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


    PauseAnimation {
        id: updatePause;

        duration: 400;
        onFinished: {
            if(searchTextField.openST && !searchTextField.canClose){
                var popup = modalDialogManager.topItem;
                popup.updateModel();
            }
        }
    }

    ComboBoxGql{
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
        preventFirstLoading:  searchContainer.externalSearchParam !=="";
        closeEmpty: true;

        canClose: true;
        doNotCorrectPosition : true;

        onInFocusChanged: {

        }
        onFilterTextChanged: {
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
            if(searchContainer.isAddressSearch){
                var modelCount = model_.GetItemsCount();
                if(modelCount){

                    var str = currentText;
                    var newAddress = model_.GetData(searchContainer.valueName),strArrCount;
                    var str_form = str.replace(/ +/g, '');
                    var newAddress_form = newAddress.replace(/ +/g, '');

                    if(str_form == newAddress_form || modelCount == 1){
                        searchContainer.setCurrentTextAddressFunc(model_,0);
                    }
                    else if(searchContainer.canSearchAnyStr){
                        searchContainer.searchPartialAddress(currentText);
                    }
                }
            }
        }

        delegate:
            PopupMenuDelegate{
            id: searchDel;

            width: searchTextField.width;
            height: visible ? searchTextField.itemHeight : 0;
            textSize: searchTextField.textSize;
            text: "";
            clip: true;
            rootItem: searchTextField;

            Component.onCompleted:  {
            }

            Text{
                id: mainText;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 10;
                anchors.right: parent.right;
                anchors.rightMargin: 10;

                wrapMode: Text.WordWrap;

                font.family: Style.fontFamily;
                color: searchContainer.fontColor;
                font.pixelSize: searchContainer.textSize;
                font.bold: false;

                text: model[searchContainer.valueName] !== undefined ?
                          model[searchContainer.valueName]: "нет данных";

            }



            onClicked: {
            }

        }//delegate

        onOpenSTChanged: {
            if(!searchTextField.openST){
                pauseClose.start();
            }
        }
    }

    PauseAnimation {
        id: pauseClose;
        duration: 10;

        onFinished: {
            if(searchContainer.isAddressSearch){
                searchContainer.closeComboAddressFunc();
            }
            else{
                searchContainer.closeComboFunc();
            }
        }
    }


    Text {
        id: placeHolder;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.left: parent.left;
        anchors.leftMargin: 10;

        font.family: Style.fontFamily;
        font.pixelSize: searchContainer.textSize;
        color: "gray";
        visible: (!searchTextField.openST && searchTextField.currentText == "") ? true :
                                                                                  (!searchTextField.openST && searchTextField.currentText !== "") ? false :
                                                                                                                                                    (searchTextField.openST && searchTextField.currentText !== "") ? false : false;
        text: qsTr("Поиск");

    }

    AuxButton{
        id: clearButton;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.left: parent.right;
        anchors.leftMargin: 6;

        width: height;
        height: Math.max(parent.height/2, 16);
        color: Style.backgroundColor;
        hasIcon: true;

        highlighted: Style.highlightedButtons !==undefined ? Style.highlightedButtons : containsMouse;

        iconSource: "../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);

        visible: searchTextField.currentText !== "" && !searchTextField.openST;
        enabled: visible;

        onClicked: {
            searchContainer.clearSearchFunc();
        }

    }

}

