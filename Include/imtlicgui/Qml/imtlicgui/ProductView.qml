import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0

Item {
    id: productsCollectionViewContainer;

    anchors.fill: parent;

    property Item rootItem;
    property Item multiDocViewItem;

    property alias itemId: productsCollectionView.itemId;
    property alias itemName: productsCollectionView.itemName;
    property alias model: productsCollectionView.collectionViewModel;

    property string operation;
    property string gqlModelQueryType;
    property string gqlModelQueryTypeNotification;

    property bool wasChanged: false;

    onWidthChanged: {
        if (productsCollectionViewContainer.width - productMetaInfo.width > 250){
            productSplitter.x = productsCollectionViewContainer.width - productMetaInfo.width;
        }
    }

    Component.onCompleted: {
        if (!treeViewModel.modelTreeView){
            Events.subscribeEvent("FeaturesTreeItemsUpdated", productsCollectionViewContainer.treeItemsUpdated);
            treeViewModel.reloadModel();
        }
        else{
            productsCollectionViewContainer.treeItemsUpdated();
        }

        if (!featureDependenciesModel.modelFeatureDependencies){
//            Events.subscribeEvent("LicenseFeaturesModelUpdated", productsCollectionViewContainer.licenseFeaturesUpdated);
            featureDependenciesModel.reloadModel();
        }

        if (!licenseFeaturesModel.modelLicenseFeatures){
            Events.subscribeEvent("LicenseFeaturesModelUpdated", productsCollectionViewContainer.licenseFeaturesUpdated);
            licenseFeaturesModel.reloadModel();
        }
        else{
            productsCollectionViewContainer.licenseFeaturesUpdated();
        }
    }

    onWasChangedChanged: {
        productsCollectionViewContainer.commandsChanged("ProductEdit");
    }

    ListModel {
        id: contextMenuModel;

        Component.onCompleted: {
            contextMenuModel.append({"id": "Edit", "name": qsTr("Edit"), "imageSource": "../../../Icons/Light/Edit_On_Normal.svg", "mode": "Normal"});
            contextMenuModel.append({"id": "Remove", "name": qsTr("Remove"), "imageSource": "../../../Icons/Light/Remove_On_Normal.svg", "mode": "Normal"});
            contextMenuModel.append({"id": "", "name": "", "imageSource": "", "mode": "Normal"});
            contextMenuModel.append({"id": "SetDescription", "name": qsTr("Set Description"), "imageSource": "", "mode": "Normal"});
        }
    }

    function openContextMenu(item, mouseX, mouseY) {
        if (productsCollectionView.table.height - mouseY <= 75){
            mouseY = mouseY - 75;
        }
        var point = productsCollectionViewContainer.mapToItem(thubnailDecoratorContainer, mouseX, mouseY);
        var source = "AuxComponents/PopupMenuDialog.qml";
        var parameters = {};
        parameters["model"] = contextMenuModel;
        parameters["resultItem"] = productsCollectionViewContainer;
        parameters["itemHeight"] = 25;
        parameters["itemWidth"] = 150;

        parameters["x"] = point.x;
        parameters["y"] = point.y;

        thubnailDecoratorContainer.openDialog(source, parameters);
    }

    function dialogResult(parameters) {
         console.log("ProductView dialogResult", parameters["dialog"], parameters["status"]);
        productsCollectionView.forceActiveFocus();
        if (parameters["dialog"] === "EditLicense"){

            if (parameters["status"] === "ok") {
                var dataModelLocal = productsCollectionView.collectionViewModel.GetData("data");
                console.log("ProductView onClicked ", dataModelLocal.GetItemsCount());

                var oldId =  dataModelLocal.GetData("Id", productsCollectionView.selectedIndex);
                var oldName =  dataModelLocal.GetData("Name", productsCollectionView.selectedIndex);

                if (oldId !== parameters["newLicenseId"] || oldName !== parameters["newLicenseName"]){
                    dataModelLocal.SetData("Id", parameters["newLicenseId"] , productsCollectionView.selectedIndex);
                    dataModelLocal.SetData("Name", parameters["newLicenseName"], productsCollectionView.selectedIndex);

                    licenseFeaturesModel.updateLicensesDependenciesAfterLicenseEditing(productsCollectionViewContainer.itemId, oldId,
                                                                       parameters["newLicenseId"],
                                                                       parameters["newLicenseName"]);

                    general.SetData("Licenses", productsCollectionView.table.elements)
                    undoRedo.addModel(general);

                    productsCollectionView.refresh();
                }
            }
        }
        else if (parameters["dialog"] === "InputDialog"){

            if (parameters["typeOperation"] === "Save") {
                var value = parameters["value"];
                productViewSaveQuery.updateModel(value);
            }
            else if (parameters["typeOperation"] === "SetDescription") {
                var value = parameters["value"];
                var dataModelLocal = productsCollectionView.collectionViewModel.GetData("data");

                var oldDescription = dataModelLocal.GetData("Description", productsCollectionView.table.selectedIndex);

                if (oldDescription !== value){
                    dataModelLocal.SetData("Description", value, productsCollectionView.table.selectedIndex);
                    productsCollectionView.collectionViewModel.SetData("data", dataModelLocal);
                    productsCollectionView.refresh();

//                    productsCollectionViewContainer.wasChanged = true;
                }
            }

            general.SetData("Licenses", productsCollectionView.table.elements)
            undoRedo.addModel(general);
        }
        else if (parameters["dialog"] === "RemoveDialog"){
            if (parameters["status"] === "yes") {
                if (productsCollectionView.collectionViewModel.ContainsKey("data")) {
                    var dataModelLocal = productsCollectionView.collectionViewModel.GetData("data");
                    dataModelLocal.RemoveItem(productsCollectionView.table.selectedIndex);

                    productsCollectionView.collectionViewModel.SetData("data", dataModelLocal);
                    productsCollectionView.collectionViewModel.Refresh();
                    productsCollectionView.refresh();

                    productsCollectionView.table.selectedIndex = -1;

                    if (dataModelLocal.GetItemsCount() == 0){
                        treeView.visible = false;
                    }

                    general.SetData("Licenses", productsCollectionView.table.elements)
                    undoRedo.addModel(general);
                }
            }
        }
        else if (parameters["dialog"] === "PopupMenu"){

            if (parameters["status"] === "Edit") {
                productsCollectionViewContainer.menuActivated("Edit");
            }
            else if (parameters["status"] === "Remove") {
                productsCollectionViewContainer.menuActivated("Remove");
            }
            else if (parameters["status"] === "Set Description") {
                var source = "AuxComponents/InputDialog.qml";
                var parameters = {};
                parameters["message"] = "Please enter the description of the license: ";
                parameters["nameDialog"] = "Set description";
                parameters["typeOperation"] = "SetDescription";
                parameters["startingValue"] = productsCollectionView.getDescriptionBySelectedItem();
                parameters["resultItem"] = productsCollectionViewContainer;
                thubnailDecoratorContainer.openDialog(source, parameters);
            }
        }
        else if (parameters["dialog"] === "SaveDialog"){

            if (parameters["status"] === "yes"){
                productsCollectionViewContainer.menuActivated("Save");
            }
            else if (parameters["status"] === "no"){
                productsCollectionViewContainer.rootItem.closeTab();
            }

        }

        productsCollectionView.forceActiveFocus();
    }

    function makeCommandActive(commandId){
        productsCollectionViewContainer.rootItem.setModeMenuButton(commandId, "Normal");
    }

    function checkLicenses(){
        let dataModelLocal = productsCollectionView.collectionViewModel.GetData("data");
        let nameLic = "";
        for (let i = 0; i < dataModelLocal.GetItemsCount(); i++){
            var licenseId = dataModelLocal.GetData("Id", i);
            if (licenseId == ""){
                nameLic = dataModelLocal.GetData("Name", i);
                break;
            }
        }

        return nameLic;
    }

    function createLicense(id, name, description) {
        var dataModelLocal = model.GetData("data");
        var index = dataModelLocal.InsertNewItem();

        dataModelLocal.SetData("Name", name, index);
        dataModelLocal.SetData("Id", id, index);
        dataModelLocal.SetData("Description", description, index);

        model.SetData("data", dataModelLocal);
        productsCollectionView.refresh();
    }

    function menuActivated(menuId) {
        console.log("ProductView menuActivated", menuId);
        if (menuId  === "New"){
            productsCollectionViewContainer.createLicense("", "License Name", "");

            general.SetData("Licenses", productsCollectionView.table.elements)
            undoRedo.addModel(general);

            productsCollectionView.refresh();
        }
        else if (menuId  === "Save") {
//            if (tfcProductId.text === ""){
//                productsCollectionView.openMessageDialog(qsTr("Error"), qsTr("Product-ID can't be empty!"), "ErrorDialog");
//            }
//            else{
                let name = productsCollectionViewContainer.checkLicenses();
                if (name == ""){
                    productViewSaveQuery.updateModel();
                }
                else{
                    let message = name + " " + qsTr("has an invalid id!");
                    productsCollectionView.openMessageDialog(qsTr("Error"), message, "ErrorDialog");
                }
//            }
        }
        else if (menuId  === "Remove") {
            var source = "AuxComponents/MessageDialog.qml";
            var parameters = {};
            parameters["message"] = "Remove the selected license from the collection ?";
            parameters["nameDialog"] = "Remove";
            parameters["resultItem"] = productsCollectionViewContainer;
            parameters["dialogId"] = "RemoveDialog";
            thubnailDecoratorContainer.openDialog(source, parameters);
        }
        else if (menuId  === "Close") {

            if (productsCollectionViewContainer.wasChanged){
                let source = "AuxComponents/MessageDialog.qml";
                let parameters = {};
                parameters["message"] = qsTr("Save all changes ?");
                parameters["nameDialog"] = "Save";
                parameters["dialogId"] = "SaveDialog";
                parameters["cancelButtonVisible"] = true;
                parameters["resultItem"] = productsCollectionViewContainer;

                thubnailDecoratorContainer.openDialog(source, parameters);
            }
            else{
                productsCollectionViewContainer.rootItem.closeTab();
            }
        }
        else if (menuId  === "Duplicate") {
            var dataModelLocal = model.GetData("data");
            var duplicateName = "Copy of " + dataModelLocal.GetData("Name", productsCollectionView.selectedIndex);
            var duplicateId = dataModelLocal.GetData("Id", productsCollectionView.selectedIndex) + "_Copy";
            var duplicateDescription = dataModelLocal.GetData("Description", productsCollectionView.selectedIndex);
            productsCollectionViewContainer.createLicense(duplicateId, duplicateName, duplicateDescription);
//            productsCollectionViewContainer.wasChanged = true;
        }
        else if (menuId  === "Undo"){
            var result = undoRedo.undo();
            if (result != null){

                productsCollectionViewContainer.parseData(result);
            }
        }
        else if (menuId  === "Redo"){
            if (!undoRedo.redoStackIsEmpty()){
                var result = undoRedo.redo();
                productsCollectionViewContainer.parseData(result);
            }
        }
        else {
            productsCollectionView.menuActivated(menuId)
        }
    }

    TreeItemModel {
        id: general;
    }

    function treeItemsUpdated(){
        console.log("ProductView TreeView treeItemsUpdated");
        general.SetData("TreeView", treeViewModel.modelTreeView)
    }

    function licenseFeaturesUpdated(){
        console.log("ProductView LicenseFeatures licenseFeaturesUpdated");
        general.SetData("LicenseFeatures", licenseFeaturesModel.modelLicenseFeatures)
    }

    function parseData(json){
        json = json.replace(/\\/g, '');
        json = json.slice(1, json.length - 1);

        console.log("json", json)

        general.Parse(json);
        console.log("general", general.toJSON())

        let elementsJson = general.GetData("Licenses").toJSON();
        let treeViewJson = general.GetData("TreeView").toJSON();
        let licenseFeaturesJson = general.GetData("LicenseFeatures").toJSON();

        productsCollectionView.table.elements.Parse(elementsJson);
        treeViewModel.modelTreeView.Parse(treeViewJson);
        licenseFeaturesModel.modelLicenseFeatures.Parse(licenseFeaturesJson);

        let productId = general.GetData("Id");
//        let productName = general.GetData("Name");

//        productIdText.text = productId;
        tfcProductId.text = productId;
//        productNameText.text = productName;

        treeViewModel.modelTreeView.Refresh();
        licenseFeaturesModel.modelLicenseFeatures.Refresh();
        productsCollectionView.refresh();
    }

    function refresh() {
        console.log("ProductView refresh");
//        productsCollectionView.refresh();

//        treeViewModel.reloadModel();
//        featureDependenciesModel.reloadModel();
//        licenseFeaturesModel.reloadModel();

        productsCollectionView.refresh();
    }

    function commandsChanged(commandsId){
        if (commandsId !== "ProductEdit"){
            return;
        }

        if (productsCollectionView.selectedIndex > -1) {
            productsCollectionViewContainer.rootItem.setModeMenuButton("Remove", "Normal");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Edit", "Normal");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Duplicate", "Normal");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Import", "Normal");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Export", "Normal");
        } else {
            productsCollectionViewContainer.rootItem.setModeMenuButton("Remove", "Disabled");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Edit", "Disabled");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Duplicate", "Disabled");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Export", "Disabled");
        }

        productsCollectionViewContainer.rootItem.setModeMenuButton("Close", "Normal");
        productsCollectionViewContainer.rootItem.setModeMenuButton("New", "Normal");

        undoRedo.updateStatesCommands();

        if (productsCollectionViewContainer.wasChanged){
            productsCollectionViewContainer.rootItem.setModeMenuButton("Save", "Normal");
        }
    }

    TreeItemModel {
        id: modelProducts;
    }

    function selectedIndexIncr(){
        console.log("ProductView selectedIndexIncr");
        if (productsCollectionView.table.selectedIndex == productsCollectionView.getCountItems() - 1){
            productsCollectionView.table.selectedIndex = 0;
        }
        else
            productsCollectionView.table.selectedIndex++;

        productsCollectionView.table.changeDataByIndex(productsCollectionView.table.selectedIndex);
    }

    function selectedIndexDecr(){
        console.log("ProductView selectedIndexDecr");
        if (productsCollectionView.table.selectedIndex == 0){
            productsCollectionView.table.selectedIndex = productsCollectionView.getCountItems() - 1;
        }
        else
            productsCollectionView.table.selectedIndex--;
        productsCollectionView.table.changeDataByIndex(productsCollectionView.table.selectedIndex);
    }

    function selectItem(){
        console.log("ProductView selectItem");

        var itemId = productsCollectionView.table.getSelectedId();
        var name = productsCollectionView.table.getSelectedName();
        productsCollectionView.itemSelect(itemId, name);
    }

    Timer {
        id: saveTimer;

        interval: 500;

        onTriggered: {
            console.log("saveTimer onTriggered");
            console.log("general", general.toJSON());
            undoRedo.addModel(general);
        }
    }

    UndoRedo {
        id: undoRedo;

        rootItem: productsCollectionViewContainer.rootItem;
        multiDocViewItem: productsCollectionViewContainer.multiDocViewItem;
    }

    CollectionView {
        id: productsCollectionView;

        anchors.left: parent.left;
        anchors.right: productSplitter.left;

        rootItem: productsCollectionViewContainer;

        height: parent.height;

        Component.onCompleted: {
            console.log("ProductView CollectionView onCompleted");
            productsCollectionView.gqlModelInfo = "ProductInfo"
            productsCollectionView.gqlModelItems = "LicenseList"
            productsCollectionView.gqlModelRemove = "";
        }

        onItemIdChanged: {
            if (productsCollectionView.itemId){
                console.log("ProductView onItemIdChanged")
                productsCollectionView.gqlModelInfo = "ProductInfo"
                productsCollectionView.gqlModelItems = "LicenseList"
                productsCollectionView.gqlModelRemove = "";
            }
        }

        onCollectionViewRightButtonMouseClicked: {
            console.log("ProductView CollectionView AuxTable onCollectionViewRightButtonMouseClicked");
            productsCollectionViewContainer.openContextMenu(item, mouseX, mouseY);
        }

        onItemSelect: {
            console.log("ProductView CollectionView onSelectItem", idSelect, name);

            var source = "../imtlicgui/EditLicenseDialog.qml";
            var parameters = {};
            parameters["licenseId"] = idSelect;
            parameters["licenseName"] = name;
            parameters["collectionViewLicenses"] = productsCollectionView;
            parameters["resultItem"] = productsCollectionViewContainer;

            thubnailDecoratorContainer.openDialog(source, parameters);
        }

        onSelectedIndexChanged: {
            console.log("PackageView CollectionView onSelectedIndexChanged", productsCollectionView.selectedIndex);
            if (productsCollectionView.selectedIndex > -1){
                productsCollectionViewContainer.commandsChanged("ProductEdit")

                if (productsCollectionViewContainer.operation != "New"){
                    let curIdIsEmpty = productsCollectionView.table.getSelectedId() == "";
                    treeView.visible = !curIdIsEmpty;
                }

                let rootLicenseId = productsCollectionView.table.getSelectedId();
                let rootKey = rootLicenseId;

                treeViewModel.clearTreeView();

                if (featureDependenciesModel.modelFeatureDependencies){
                    let strValues = licenseFeaturesModel.modelLicenseFeatures.GetData(rootKey);
                    if (strValues){
                        let values = strValues.split(';');
                        for (let value of values){
                            treeViewModel.selectFeature(value);
                        }
                    }

                    let upFeatures = [];
                    featureDependenciesModel.getFeaturesDependsByFeatureUp(rootKey, upFeatures);

                    if (upFeatures.length > 0){
                        treeViewModel.updateDataFeatureList(upFeatures, 0, 0);
                    }
                }
            }
            else {
                treeView.visible = false;
            }
        }

        onDataLoaded: {
            general.SetData("Licenses", productsCollectionView.table.elements)
            undoRedo.addModel(general);
            loadingPage.visible = false;
        }
    }

    GqlModel {
        id: productViewSaveQuery;

        function updateModel() {
            console.log( "updateModel productViewSaveQuery", productsCollectionViewContainer.operation);

            var query;
            var queryFields;
            var inputParams = Gql.GqlObject("input");

            if (productsCollectionViewContainer.operation == "Open") {
                productsCollectionViewContainer.gqlModelQueryType = "ProductUpdate";
                productsCollectionViewContainer.gqlModelQueryTypeNotification = "updatedNotification";

                query = Gql.GqlRequest("query", "ProductUpdate");
                inputParams.InsertField("Id", productsCollectionViewContainer.itemId);
                queryFields = Gql.GqlObject("updatedNotification");
            }
            else {
                productsCollectionViewContainer.gqlModelQueryType = "ProductAdd";
                productsCollectionViewContainer.gqlModelQueryTypeNotification = "addedNotification";

                query = Gql.GqlRequest("query", "ProductAdd");
                queryFields = Gql.GqlObject("addedNotification");
            }

            query.AddParam(inputParams);

            modelProducts.SetData("Id", tfcProductId.text)
            modelProducts.SetData("Name", tfcProductName.text)
//            modelProducts.SetData("Id", productsCollectionViewContainer.itemId)
//            modelProducts.SetData("Name", productsCollectionViewContainer.itemName)
            modelProducts.SetExternTreeModel("licenses", productsCollectionView.collectionViewModel.GetData("data"));
            modelProducts.SetExternTreeModel("dependencies", licenseFeaturesModel.modelLicenseFeatures);

            var jsonString = modelProducts.toJSON();
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")

            inputParams.InsertField ("Item", jsonString);

            queryFields.InsertField("Id");
            queryFields.InsertField("Successed");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("ProductEdit query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, productViewSaveQuery);
            if (this.state === "Ready"){
                let keys = productViewSaveQuery.GetKeys();
                if (!keys || keys.length == 0){
                    thubnailDecoratorContainer.setInvalidConnection(true);
                    return;
                }

                var dataModelLocal;
                if (productViewSaveQuery.ContainsKey("errors")){
                    dataModelLocal = productViewSaveQuery.GetData("errors");

                    dataModelLocal = dataModelLocal.GetData(productsCollectionViewContainer.gqlModelQueryType);
                    if (dataModelLocal){
                        var messageError = dataModelLocal.GetData("message");
                        productsCollectionView.openMessageDialog("Error", messageError, "ErrorDialog");
                    }

                    return;
                }

                if (productViewSaveQuery.ContainsKey("data")){
                    dataModelLocal = productViewSaveQuery.GetData("data");
                    dataModelLocal = dataModelLocal.GetData(productsCollectionViewContainer.gqlModelQueryType);
                    if (!dataModelLocal){
                        return;
                    }
                    dataModelLocal = dataModelLocal.GetData(productsCollectionViewContainer.gqlModelQueryTypeNotification);
                    if (!dataModelLocal){
                        return;
                    }
                    productsCollectionView.refresh();
                    var newId, newName;

                    if(dataModelLocal.ContainsKey("Id")){
                        newId = dataModelLocal.GetData("Id");
                    }

                    if (productsCollectionView.itemId !== newId){
                        productsCollectionView.itemId = newId;
                    }

                    if (productsCollectionViewContainer.operation == "New"){
                        productsCollectionViewContainer.operation = "Open";

                        let dataModelLocal = productsCollectionView.collectionViewModel.GetData("data");
                        if (dataModelLocal && dataModelLocal.GetItemsCount() > 0){
                            treeView.visible = true;
                        }
                    }
                    productsCollectionViewContainer.multiDocViewItem.disableChanges();

                    if (productsCollectionView.itemName !== tfcProductName.text){
                        productsCollectionViewContainer.rootItem.updateTitleTab(newId, tfcProductName.text);
                    }

                    productsCollectionViewContainer.multiDocViewItem.activeCollectionItem.callMetaInfoQuery();
                }
            }
        }
    }

    Splitter {
        id: productSplitter;
        x: productsCollectionViewContainer.width - 250;

        height: parent.height;
        width: 4;

        onXChanged: {
            if (!productsCollectionViewContainer.visible){
                return;
            }

            if (productSplitter.x > productsCollectionViewContainer.width - titleHeader.width){
                productSplitter.x = productsCollectionViewContainer.width - productSplitter.width;
            }

            if (productSplitter.x < 250){
                productSplitter.x = 250;
            }
        }
    }

    Rectangle {
        id: productMetaInfo;

        anchors.left: productSplitter.right;

        height: parent.height;
        width: productsCollectionViewContainer.width > 0 ? productsCollectionViewContainer.width - productsCollectionView.width : 250;
        color: Style.backgroundColor;

        Rectangle {
            id: productIdTextRect;

            anchors.left: parent.left;
            anchors.top: parent.top;

            width: parent.width;
            height: 20;

            color: Style.backgroundColor;

            Text {
               id: productIdText;

               anchors.left: parent.left;
               anchors.verticalCenter: parent.verticalCenter;

               color: Style.textColor;
               font.pixelSize: Style.fontSize_common;
               font.family: Style.fontFamily;

               text: qsTr("Product-ID");
            }
        }

        Rectangle {
            id: rectTfcProductId;

            anchors.top: productIdTextRect.bottom;
            width: parent.width - 3;
            height: 30;

            TextFieldCustom {
                id: tfcProductId;

                anchors.fill: parent;
                text: productsCollectionViewContainer.itemId;

                onInputTextChanged: {

                    general.SetData("Id", tfcProductId.text);

                    if (tfcProductId.text != productsCollectionViewContainer.itemId){
                        saveTimer.restart();
                    }
                }
            }

            onFocusChanged: {
                if (rectTfcProductId.focus){
                    tfcProductId.setFocus(true);
                }
            }

            KeyNavigation.tab: rectTfcProductName;
            KeyNavigation.backtab: rectTfcProductName;
        }

        Rectangle {
            id: productNameTextRect;
            anchors.top: rectTfcProductId.bottom;
            anchors.topMargin: 8;

            width: parent.width;
            height: 20;

            color: Style.backgroundColor;

            Text {
               id: productNameText;

               anchors.left: parent.left;
               anchors.verticalCenter: parent.verticalCenter;

               color: Style.textColor;
               font.pixelSize: Style.fontSize_common;
               font.family: Style.fontFamily;

               text: qsTr("Product Name");
            }
        }

        Rectangle {
            id: rectTfcProductName;

            anchors.top: productNameTextRect.bottom;

            width: parent.width - 3;
            height: 30;

            TextFieldCustom {
                id: tfcProductName;

                anchors.fill: parent;

                text: productsCollectionViewContainer.itemName;

                onInputTextChanged: {

                    general.SetData("Name", tfcProductName.text);

                    if (tfcProductName.text != productsCollectionViewContainer.itemName){
                        //general.SetData("Name", tfcProductName.text);
                        saveTimer.restart();
                    }
                }

            }

            onFocusChanged: {
                if (rectTfcProductName.focus){
                    tfcProductName.setFocus(true);
                }
            }

            KeyNavigation.tab: rectTfcProductId;
            KeyNavigation.backtab: rectTfcProductId;
        }

        Rectangle {
            id: headerTreeView;

            anchors.top: rectTfcProductName.bottom;
            anchors.topMargin: productNameText.height + productNameTextRect.anchors.topMargin;

            width: parent.width - 5;
            height: 35;

            color: Style.baseColor;

            Text {
                id: titleHeader;

                anchors.verticalCenter: headerTreeView.verticalCenter;
                anchors.left: headerTreeView.left;
                anchors.leftMargin: 10;

                text: qsTr("Features");
                color: Style.textColor;

                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamilyBold;
                font.bold: true;
            }
        }

        Rectangle {
            id: headerBottomBorder;

            anchors.bottom: headerTreeView.bottom;

            height: 1;
            width: parent.width;

            color: "lightgray";
        }

        Text {
            id: valueText;

            anchors.top: headerBottomBorder.bottom;
            anchors.topMargin: 10;
            anchors.horizontalCenter: parent.horizontalCenter;

            text: qsTr("Please save the product first!");
            visible: productsCollectionViewContainer.operation == "New";

            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            color: Style.textColor;
        }

        TreeView {
            id: treeView;

            anchors.top: headerBottomBorder.bottom;
            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.rightMargin: 5;
            anchors.bottom: parent.bottom;

            clip: true;

            visible: false;
            //visible: productsCollectionViewContainer.operation !== "New";

            modelItems: treeViewModel.modelTreeView;

            onItemTreeViewCheckBoxStateChanged: {
                console.log("ProductView TreeView onItemTreeViewCheckBoxStateChanged",
                            productsCollectionViewContainer.itemId,
                            productsCollectionView.table.getSelectedId(),
                            packageId,
                            featureId,
                            state);

                let rootLicenseId = productsCollectionView.table.getSelectedId();
                let rootKey = rootLicenseId;
                let value = featureId;

                if (state == 0){
                    treeViewModel.deselectFeature(value);
                }

                if (licenseFeaturesModel.modelLicenseFeatures.ContainsKey(rootKey)){
                    let str = licenseFeaturesModel.modelLicenseFeatures.GetData(rootKey);
                    let arr = str.split(";");
                    if (state == 0){
                        if (arr){
                            let index = arr.indexOf(value);
                            if (index > -1){
                                arr.splice(index, 1);
                            }

                            if (arr.length == 0){
                                licenseFeaturesModel.modelLicenseFeatures.RemoveData(rootKey);
                            }
                            else{
                                let resStr = arr.join(';');
                                licenseFeaturesModel.modelLicenseFeatures.SetData(rootKey, resStr);
                            }
                        }
                    }
                    else{
                        if (arr.indexOf(value) == -1){
                            arr.push(value);
                            let resStr = arr.join(';');
                            licenseFeaturesModel.modelLicenseFeatures.SetData(rootKey, resStr);
                        }
                    }
                }
                else{
                    licenseFeaturesModel.modelLicenseFeatures.SetData(rootKey, value);
                }

                if (state == 2){
                    treeViewModel.selectFeature(value);
                }

                productsCollectionViewContainer.licenseFeaturesUpdated();
                productsCollectionViewContainer.treeItemsUpdated();

                undoRedo.addModel(general);
//                console.log(" featuresTreeView.productLicenseFeatures",  featuresTreeView.productLicenseFeatures.toJSON());
            }
        }
    }

    LoadingPage {
        id: loadingPage;

        anchors.fill: parent;

        visible: true;
    }
}
