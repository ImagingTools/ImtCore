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
        if (!featuresTreeView.modelTreeItems){
            featuresTreeView.loadFeaturesModel();
        }

        if (!featuresTreeView.modelDepends){
            featuresTreeView.loadDependModel();
        }

        if (!featuresTreeView.productLicenseFeatures){
            featuresTreeView.loadLicenseDependModel();
        }
    }

    onWasChangedChanged: {
        productsCollectionViewContainer.commandsChanged("ProductEdit");
    }

    Keys.onPressed: {
        console.log("ProductView keys pressed")
        if (treeView.focus){
            return;
        }

        if (event.key === Qt.Key_Tab){
            console.log('Key tab was pressed');
            treeView.forceActiveFocus();
        }
        else if (event.key === Qt.Key_Up){
            console.log('Key up was pressed');
            productsCollectionViewContainer.selectedIndexDecr();
        }
        else if (event.key === Qt.Key_Down){
            console.log('Key down was pressed');
             productsCollectionViewContainer.selectedIndexIncr();
        }
        else if (event.key === Qt.Key_Return){
            console.log('Key down was pressed');

            productsCollectionViewContainer.selectItem();
        }
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

        if (parameters["dialog"] === "EditLicense"){

            if (parameters["status"] === "ok") {
                var dataModelLocal = productsCollectionView.collectionViewModel.GetData("data");
                console.log("ProductView onClicked ", dataModelLocal.GetItemsCount());

                var oldId =  dataModelLocal.GetData("Id", productsCollectionView.selectedIndex);
                var oldName =  dataModelLocal.GetData("Name", productsCollectionView.selectedIndex);

                console.log("oldId", oldId);
                console.log("newLicenseId", parameters["newLicenseId"]);

                if (oldId !== parameters["newLicenseId"] || oldName !== parameters["newLicenseName"]){
                    dataModelLocal.SetData("Id", parameters["newLicenseId"] , productsCollectionView.selectedIndex);
                    dataModelLocal.SetData("Name", parameters["newLicenseName"], productsCollectionView.selectedIndex);
                    productsCollectionView.collectionViewModel.SetData("data", dataModelLocal);
                    productsCollectionView.refresh();

                    console.log("productsCollectionView.collectionViewModel", productsCollectionView.collectionViewModel.toJSON());

                    console.log("getSelectedId", productsCollectionView.table.getSelectedId());
                    productsCollectionViewContainer.updateFeatureLicenseModelAfterLicenseEdit(productsCollectionViewContainer.itemId, oldId, parameters["newLicenseId"]);

                    treeView.visible = true;
//                    productsCollectionViewContainer.wasChanged = true;
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
    }

    function updateFeatureLicenseModelAfterProductEdit(oldProductId, newProductId){
        console.log("updateFeatureLicenseModelAfterProductEdit", oldProductId, newProductId);
        if (!featuresTreeView.productLicenseFeatures || oldProductId == newProductId || oldProductId == "", newProductId == ""){
            return;
        }

        for (let i = 0; i < featuresTreeView.productLicenseFeatures.GetItemsCount(); i++){
            let rootProductId = featuresTreeView.productLicenseFeatures.GetData("RootProductId", i);

            if (rootProductId == oldProductId){
                featuresTreeView.productLicenseFeatures.SetData("RootProductId", newProductId, i);
            }
        }
    }

    function updateFeatureLicenseModelAfterLicenseEdit(productId, oldLicId, newLicId){
        console.log("updateFeatureLicenseModelAfterLicenseEdit", productId, oldLicId, newLicId);
        if (!featuresTreeView.productLicenseFeatures || oldLicId == newLicId || oldLicId == "" || newLicId == "" || productId == ""){
            return;
        }

        for (let i = 0; i < featuresTreeView.productLicenseFeatures.GetItemsCount(); i++){
            let rootLicId = featuresTreeView.productLicenseFeatures.GetData("RootLicenseId", i);
            let rootProductId = featuresTreeView.productLicenseFeatures.GetData("RootProductId", i);

            if (rootLicId == oldLicId && rootProductId == productId){
                featuresTreeView.productLicenseFeatures.SetData("RootLicenseId", newLicId, i);
            }
        }
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
        }
        else if (menuId  === "Save") {

//            if (productsCollectionViewContainer.operation === "New"){
//                var source = "AuxComponents/InputDialog.qml";
//                var parameters = {};
//                parameters["message"] = "Please enter the name of the document: ";
//                parameters["nameDialog"] = "Document Name";
//                parameters["resultItem"] = productsCollectionViewContainer;
//                thubnailDecoratorContainer.openDialog(source, parameters);
//            }

//            if (productsCollectionView.itemId === "") {
//                var source = "AuxComponents/InputDialog.qml";
//                var parameters = {};
//                parameters["message"] = "Please enter the name of the document: ";
//                parameters["nameDialog"] = "Document Name";
//                parameters["resultItem"] = productsCollectionViewContainer;
//                thubnailDecoratorContainer.openDialog(source, parameters);
//            }
//            else {
//                productViewSaveQuery.updateModel()
//            }

            if (tfcProductId.text === ""){
                productsCollectionView.openMessageDialog(qsTr("Error dialog"), qsTr("Id can't be empty!"), "ErrorDialog");
            }
            else{
                let name = productsCollectionViewContainer.checkLicenses();
                if (name == ""){
                    productViewSaveQuery.updateModel();
                }
                else{
                    let message = name + qsTr(" has an invalid id!");
                    productsCollectionView.openMessageDialog(qsTr("Error dialog"), message, "ErrorDialog");
                }
            }
        }
        else if (menuId  === "Remove") {
            var source = "AuxComponents/MessageDialog.qml";
            var parameters = {};
            parameters["message"] = "Remove selected file from the database ?";
            parameters["nameDialog"] = "Remove Dialog";
            parameters["resultItem"] = productsCollectionViewContainer;
            parameters["dialogId"] = "RemoveDialog";
            thubnailDecoratorContainer.openDialog(source, parameters);
        }
        else if (menuId  === "Close") {
            productsCollectionViewContainer.rootItem.closeTab();
        }
        else if (menuId  === "Duplicate") {
            var dataModelLocal = model.GetData("data");
            var duplicateName = "Copy of " + dataModelLocal.GetData("Name", productsCollectionView.selectedIndex);
            var duplicateId = dataModelLocal.GetData("Id", productsCollectionView.selectedIndex) + "_Copy";
            var duplicateDescription = dataModelLocal.GetData("Description", productsCollectionView.selectedIndex);
            productsCollectionViewContainer.createLicense(duplicateId, duplicateName, duplicateDescription);
//            productsCollectionViewContainer.wasChanged = true;
        }
        else {
            productsCollectionView.menuActivated(menuId)
        }
    }

    function refresh() {
        console.log("ProductView refresh");
        productsCollectionView.refresh();

        featuresTreeView.loadFeaturesModel();
        featuresTreeView.loadDependModel();
        featuresTreeView.loadLicenseDependModel();
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
//            productsCollectionViewContainer.rootItem.setModeMenuButton("Save", "Normal");//
            productsCollectionViewContainer.rootItem.setModeMenuButton("Close", "Normal");
        } else {
            productsCollectionViewContainer.rootItem.setModeMenuButton("Remove", "Disabled");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Edit", "Disabled");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Duplicate", "Disabled");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Export", "Disabled");
//            productsCollectionViewContainer.rootItem.setModeMenuButton("Save", "Disabled");//
        }
        productsCollectionViewContainer.rootItem.setModeMenuButton("Save", "Normal");
//        if (productsCollectionViewContainer.wasChanged){
//            productsCollectionViewContainer.rootItem.setModeMenuButton("Save", "Normal");
//        } else {
//            productsCollectionViewContainer.rootItem.setModeMenuButton("Save", "Disabled");
//        }
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

    CollectionView {
        id: productsCollectionView;

        anchors.left: parent.left;
        anchors.right: productSplitter.left;

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

                let curIdIsEmpty = productsCollectionView.table.getSelectedId() == "";
                if (!curIdIsEmpty){
                    treeView.visible = true;
                }
                else{
                    treeView.visible = false;
                }

                let rootProductId = productsCollectionViewContainer.itemId;
                let rootLicenseId = productsCollectionView.table.getSelectedId();
                let rootKey = rootProductId + '.' + rootLicenseId;

                featuresTreeView.clearTreeView();

                if (featuresTreeView.modelDepends){
                    let strValues = featuresTreeView.productLicenseFeatures.GetData(rootKey);
                    if (strValues){
                        let values = strValues.split(';');
                        for (let value of values){
                            featuresTreeView.selectFeature(value);
                        }
                    }

                    let upFeatures = [];
                    featuresTreeView.getFeaturesDependsByFeatureUp(rootKey, upFeatures);

                    if (upFeatures.length > 0){
                        featuresTreeView.updateDataFeatureList(upFeatures, 0, 0);
                    }
                }
            }
            else {
                treeView.visible = false;
            }
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
                inputParams.InsertField("Id");
                inputParams.InsertFieldArgument("Id", productsCollectionViewContainer.itemId);
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
            modelProducts.SetExternTreeModel("dependencies", featuresTreeView.productLicenseFeatures);

            var jsonString = modelProducts.toJSON();
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")

            inputParams.InsertField("Item");
            inputParams.InsertFieldArgument ("Item", jsonString);

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
                var dataModelLocal;
                if (productViewSaveQuery.ContainsKey("errors")){
                    dataModelLocal = productViewSaveQuery.GetData("errors");

                    dataModelLocal = dataModelLocal.GetData(productsCollectionViewContainer.gqlModelQueryType);
                    if (dataModelLocal){
                        var messageError = dataModelLocal.GetData("message");
                        productsCollectionView.openMessageDialog("Error Dialog", messageError, "ErrorDialog");
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

                    if (productsCollectionView.itemName !== tfcProductName.text){
                        productsCollectionViewContainer.rootItem.updateTitleTab(
                                    newId,
                                    tfcProductName.text);
                    }

                    if (productsCollectionViewContainer.operation == "New"){
                        productsCollectionViewContainer.operation = "Open";
                    }
//                    console.log("productsCollectionViewContainer.operation", productsCollectionViewContainer.operation);
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

        //anchors.right: parent.right;
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

               text: qsTr("Product-Id");
            }
        }

        TextFieldCustom {
            id: tfcProductId;

            anchors.top: productIdTextRect.bottom;

            width: parent.width - 3;
            height: 30;

            text: productsCollectionViewContainer.itemId;

            onInputTextChanged: {
//                productsCollectionViewContainer.wasChanged = true;
                if (tfcProductId.text != ""){

                    productsCollectionViewContainer.updateFeatureLicenseModelAfterProductEdit(
                                productsCollectionViewContainer.itemId,
                                tfcProductId.text);

                    //productsCollectionViewContainer.itemId = tfcProductId.text;
                }
            }

        }

        Rectangle {
            id: productNameTextRect;
            anchors.top: tfcProductId.bottom;

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

        TextFieldCustom {
            id: tfcProductName;

            anchors.top: productNameTextRect.bottom;

            width: parent.width - 3;
            height: 30;

            text: productsCollectionViewContainer.itemName;

            onInputTextChanged: {
//                productsCollectionViewContainer.wasChanged = true;
            }
        }

        Rectangle {
            id: headerTreeView;

            anchors.top: tfcProductName.bottom;
            anchors.topMargin: 5;

            width: parent.width - 5;
            height: 35;

            color: Style.baseColor;

            Text {
                id: titleHeader;

                anchors.verticalCenter: headerTreeView.verticalCenter;
                anchors.left: headerTreeView.left;
                anchors.leftMargin: 10;

                text: qsTr("Dependencies");
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

        TreeView {
            id: treeView;

            anchors.top: headerBottomBorder.bottom;
            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.rightMargin: 5;
            anchors.bottom: parent.bottom;

            clip: true;

            visible: false;

            modelItems: featuresTreeView.modelTreeItems;

            onItemTreeViewCheckBoxStateChanged: {
                console.log("ProductView TreeView onItemTreeViewCheckBoxStateChanged",
                            productsCollectionViewContainer.itemId,
                            productsCollectionView.table.getSelectedId(),
                            packageId,
                            featureId,
                            state);
               // productsCollectionViewContainer.wasChanged = true;
                let rootLProductd = productsCollectionViewContainer.itemId;
                let rootLicenseId = productsCollectionView.table.getSelectedId();
                let rootKey = rootLProductd + '.' + rootLicenseId;
                let value = packageId + '.' + featureId;

                if (state == 0){
                    featuresTreeView.deselectFeature(value);
                }

                if (featuresTreeView.productLicenseFeatures.ContainsKey(rootKey)){
                    let str = featuresTreeView.productLicenseFeatures.GetData(rootKey);
                    let arr = str.split(";");
                    if (state == 0){
                        if (arr){
                            let index = arr.indexOf(value);
                            if (index > -1){
                                arr.splice(index, 1);
                            }

                            if (arr.length == 0){
                                featuresTreeView.productLicenseFeatures.RemoveData(rootKey);
                            }
                            else{
                                let resStr = arr.join(';');
                                featuresTreeView.productLicenseFeatures.SetData(rootKey, resStr);
                            }
                        }
                    }
                    else{
                        if (arr.indexOf(value) == -1){
                            arr.push(value);
                            let resStr = arr.join(';');
                            featuresTreeView.productLicenseFeatures.SetData(rootKey, resStr);
                        }
                    }
                }
                else{
                    featuresTreeView.productLicenseFeatures.SetData(rootKey, value);
                }

                if (state == 2){
                    featuresTreeView.selectFeature(value);
                }
            }
        }

//        FeaturesTreeView {
//            id: featuresTreeView;

//            onModelTreeItemsChanged: {
//                console.log("PackageView FeaturesTreeView onModelTreeViewChanged");
//                //treeView.modelItems = featuresTreeView.modelTreeItems;
//            }

//            onProductLicenseFeaturesChanged: {
//                console.log("PackageView FeaturesTreeView onProductLicenseFeaturesChanged");
//            }
//        }
    }
}
