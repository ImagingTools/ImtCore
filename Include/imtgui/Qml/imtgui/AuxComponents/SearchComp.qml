import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

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
    property alias canClose: searchTextField.canClose;

    property string valueName: "Name";

    property string externalSearchParam: "";

    property TreeItemModel gettedParamsModel: TreeItemModel{};
    property TreeItemModel filterIdsModel: TreeItemModel{};
    property TreeItemModel propertiesModel: TreeItemModel{};

    property string parentIds: "";
    property string excludeFilterPart: "";
    property string selectedText: "";

    property alias delegate: searchTextField.delegate;
    property alias textField: searchTextField;
    property alias currentText: searchTextField.currentText;

    property alias openST: searchTextField.openST;

    property Component delegateDefault: PopupMenuDelegate{
        id: searchDelegateDefault;

        width: searchContainer.textField.width;
        height: visible ? searchContainer.textField.itemHeight : 0;
        textSize: searchContainer.textSize;
        text: "";
        clip: true;
        rootItem: searchContainer.textField;
        property TreeItemModel itemsModel: TreeItemModel{};
        Component.onCompleted:  {
            text = model.Name;
        }

    }

    signal accepted(string retVal);
    //signal updateSignal(string param);


    Component.onCompleted: {
    }

    onCommandIdChanged: {
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

    function setCurrentTextFunc(modelll, index_){
        if(searchContainer.isAddressSearch){
            searchTextField.currentText = modelll.GetData(searchContainer.valueName, index_);
            var parentIds__ = modelll.GetData("ParentIds",index_) !== undefined ? modelll.GetData("ParentIds", index_) : "";
            if(parentIds__ !== ""){
                parentIds__ = parentIds__ + ",";
            }
            var addressId__ = modelll.GetData("Id", index_);
            searchContainer.parentIds = parentIds__ + addressId__;
            searchContainer.selectedText = searchTextField.currentText;
            setPropertiesModel(searchContainer.propertyId, searchContainer.parentIds);

            searchContainer.externalSearchParam = searchContainer.parentIds;

            //console.log("searchContainer.externalSearchParam", searchContainer.externalSearchParam)
            searchContainer.accepted(searchContainer.externalSearchParam);

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

        else{
            searchTextField.currentText = modelll.GetData(searchContainer.valueName, index_);
        }

    }

    function textChangeFunc(filterText){
        //console.log("filterText:: " ,filterText);
        if(searchContainer.isAddressSearch){

            if(searchTextField.openST){
                var popup = modalDialogManager.topItem;

                var arrCount_prev = searchContainer.arraySize(searchContainer.selectedText);
                var arrCount = searchContainer.newArrayCount(searchContainer.selectedText, filterText);
                //console.log("arrCount::: prev , curr ", arrCount_prev, arrCount);

                if(arrCount < arrCount_prev){
                    searchContainer.selectedText = filterText;
                    searchTextField.excludeFilterPart = searchContainer.keepNElements(popup.filterText, arrCount);
                    searchContainer.parentIds = searchContainer.keepNElements(searchContainer.parentIds, arrCount);
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
            if(searchContainer.isAddressSearch){
                var textArraySize = searchContainer.arraySize(searchTextField.currentText);
                var parentIdsArraySize = searchContainer.arraySize(searchContainer.parentIds);
                searchTextField.excludeFilterPart = searchContainer.removeLastElement(searchTextField.currentText);
                if(textArraySize == parentIdsArraySize){
                    searchContainer.parentIds = searchContainer.removeLastElement(searchContainer.parentIds);
                }
                setPropertiesModel(searchContainer.propertyId, searchContainer.parentIds);
                if(searchContainer.canClose){
                    searchContainer.accepted(searchTextField.currentText);
                }
                //console.log("secondSearch:: ", "parentIds:: " ,searchContainer.parentIds)
            }
            else{
                if(searchContainer.canClose){
                    searchContainer.accepted(searchTextField.currentText);
                }
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
            searchContainer.gettedParamsModel.InsertNewItem();
            searchContainer.gettedParamsModel.CopyItemDataFromModel(j,filterIdsModel_,j);
        }
        if(propertiesModel_ !== null){
            for(let k = 0; k < propertiesModel_.GetItemsCount(); k++){
                searchContainer.gettedParamsModel.InsertNewItem();
                searchContainer.gettedParamsModel.CopyItemDataFromModel(k,propertiesModel_,k);
            }
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
        var length_ = Math.max(arr1.length, arr2.length);
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
        delegateRadius: 10;
        itemHeight: searchContainer.elementHeight;
        textSize: searchContainer.textSize;
        backVisible: !openST;
        complexModel: true;
        keepFilterText: true;
        preventFirstLoading:  searchContainer.externalSearchParam !=="";

        canClose: false;

        onInFocusChanged: {

        }
        onFilterTextChanged: {
            searchContainer.textChangeFunc(filterText);
        }
        onSetCurrentText:{
            searchContainer.setCurrentTextFunc(modelll, searchTextField.currentIndex);
        }

        delegate:
            PopupMenuDelegate{
            id: searchDelegate;

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

            //            Text{
            //                id: firstText;

            //                anchors.left: parent.left;
            //                anchors.top: parent.top;
            //                anchors.leftMargin: 10;
            //                anchors.topMargin: 6;

            //                font.family: Style.fontFamily;
            //                color: searchContainer.fontColor;
            //                font.pixelSize: searchContainer.textSize - 2;
            //                font.bold: false;

            //                text: "";
            //            }

            //            Text{
            //                id: secondText;

            //                anchors.left: parent.left;
            //                anchors.top: firstText.bottom;
            //                anchors.leftMargin: 10;
            //                anchors.topMargin: 2;

            //                font.family: Style.fontFamily;
            //                color: searchContainer.fontColor;
            //                font.pixelSize: searchContainer.textSize;
            //                font.bold: false;

            //                text: "";
            //            }

            //            Text{
            //                id: thirdText;

            //                anchors.left: secondText.right;
            //                anchors.top: firstText.bottom;
            //                anchors.leftMargin: 0;
            //                anchors.topMargin: 2;

            //                font.family: Style.fontFamily;
            //                color: searchContainer.fontColor;
            //                font.pixelSize: searchContainer.textSize;
            //                font.bold: false;

            //                property string number: "";

            //                text: number == "" ? "" : ", " + number;
            //            }

            //            Text{
            //                id: fourthText;

            //                anchors.left: thirdText.right;
            //                anchors.top: firstText.bottom;
            //                anchors.leftMargin: 0;
            //                anchors.topMargin: 2;

            //                font.family: Style.fontFamily;
            //                color: searchContainer.fontColor;
            //                font.pixelSize: searchContainer.textSize;
            //                font.bold: false;

            //                property string number: "";

            //                text: number == "" ? "" : ", " + number;
            //            }


            //            onClicked: {
            //            }

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
            searchContainer.closeComboFunc();
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
        text: "Поиск"

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

        iconSource: "../../../Icons/" + Style.theme + "/Close.svg";

        visible: searchTextField.currentText !== "" && !searchTextField.openST;
        enabled: visible;

        onClicked: {
            searchContainer.clearSearchFunc();
        }

    }

}

