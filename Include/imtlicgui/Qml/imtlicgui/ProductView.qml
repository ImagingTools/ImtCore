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

    Component.onCompleted: {
        console.log("ProductView onCompleted",  productsCollectionView.selectedIndex);

        console.log("ProductView onCompleted",  productsCollectionView.height);
        console.log("ProductView onCompleted",  productsCollectionView.width);

        console.log("ProductView collection size",   productsCollectionView.table);
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
            contextMenuModel.append({"id": "Edit", "name": "Edit", "imageSource": "../../../Icons/Light/Edit_On_Normal.svg", "mode": "Normal"});
            contextMenuModel.append({"id": "Remove", "name": "Remove", "imageSource": "../../../Icons/Light/Remove_On_Normal.svg", "mode": "Normal"});
            contextMenuModel.append({"id": "", "name": "", "imageSource": "", "mode": "Normal"});
            contextMenuModel.append({"id": "SetDescription", "name": "Set Description", "imageSource": "", "mode": "Normal"});
        }
    }

    function openContextMenu(item, mouseX, mouseY) {
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

                if (oldId !== parameters["newLicenseId"] || oldName !== parameters["newLicenseName"]){
                    dataModelLocal.SetData("Id", parameters["newLicenseId"] , productsCollectionView.selectedIndex);
                    dataModelLocal.SetData("Name", parameters["newLicenseName"], productsCollectionView.selectedIndex);
                    productsCollectionView.collectionViewModel.SetData("data", dataModelLocal);
                    productsCollectionView.refresh();

                    productsCollectionViewContainer.wasChanged = true;
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

                    productsCollectionViewContainer.wasChanged = true;
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

    function openMessageDialog(nameDialog, message) {
        var source = "AuxComponents/MessageDialog.qml";
        var parameters = {};
        parameters["resultItem"] = productsCollectionViewContainer;
        parameters["noButtonVisible"] = false;
        parameters["textOkButton"] = "Ok";
        parameters["message"] = message;
        parameters["nameDialog"] = nameDialog;
        thubnailDecoratorContainer.openDialog(source, parameters);
    }

    function makeCommandActive(commandId){
        productsCollectionViewContainer.rootItem.setModeMenuButton(commandId, "Normal");
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
                productsCollectionView.openMessageDialog("Error dialog", "Id can't be empty!", "ErrorDialog");
            }
            else{
                productViewSaveQuery.updateModel()
            }
        }
        else if (menuId  === "Remove") {
            var source = "AuxComponents/MessageDialog.qml";
            var parameters = {};
            parameters["message"] = "Remove selected file from the database ?";
            parameters["nameDialog"] = "RemoveDialog";
            parameters["resultItem"] = productsCollectionViewContainer;
            thubnailDecoratorContainer.openDialog(source, parameters);
        }
        else if (menuId  === "Close") {
            productsCollectionViewContainer.rootItem.closeTab();
        }
        else if (menuId  === "Duplicate") {
            var dataModelLocal = model.GetData("data");
            var duplicateName = "Copy of " + dataModelLocal.GetData("Name", productsCollectionView.selectedIndex);
            var duplicateId = dataModelLocal.GetData("Id", productsCollectionView.selectedIndex);
            var duplicateDescription = dataModelLocal.GetData("Description", productsCollectionView.selectedIndex);
            productsCollectionViewContainer.createLicense(duplicateId, duplicateName, duplicateDescription);
            productsCollectionViewContainer.wasChanged = true;
        }
        else {
            productsCollectionView.menuActivated(menuId)
        }
    }

    function refresh() {
        console.log("ProductView refresh");
        productsCollectionView.refresh();
    }

    function commandsChanged(commandsId){
        if (commandsId !== "ProductEdit") {
            return;
        }

        if (productsCollectionView.selectedIndex > -1) {
            productsCollectionViewContainer.rootItem.setModeMenuButton("Remove", "Normal");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Edit", "Normal");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Duplicate", "Normal");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Import", "Normal");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Export", "Normal");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Save", "Normal");//
            productsCollectionViewContainer.rootItem.setModeMenuButton("Close", "Normal");
        } else {
            productsCollectionViewContainer.rootItem.setModeMenuButton("Remove", "Disabled");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Edit", "Disabled");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Duplicate", "Disabled");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Export", "Disabled");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Save", "Disabled");//
        }

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
        productsCollectionView.selectItem(itemId, name);
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

        onSelectItem: {
            console.log("ProductView CollectionView onSelectItem", selectedId, name);

            var source = "../imtlicgui/EditLicenseDialog.qml";
            var parameters = {};
            parameters["licenseId"] = selectedId;
            parameters["licenseName"] = name;
            parameters["collectionViewLicenses"] = productsCollectionView;
            parameters["resultItem"] = productsCollectionViewContainer;

            thubnailDecoratorContainer.openDialog(source, parameters);
        }

        onSelectedIndexChanged: {
            console.log("PackageView CollectionView onSelectedIndexChanged", productsCollectionView.selectedIndex);
            if (productsCollectionView.selectedIndex > -1){
                productsCollectionViewContainer.commandsChanged("ProductEdit")

                treeView.visible = true;

                productMetaInfo.clearTreeView();
                productMetaInfo.updateTreeView();
                treeView.modelItems.Refresh();
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
                        productsCollectionViewContainer.openMessageDialog("Error Dialog", messageError);
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
                    productsCollectionViewContainer.multiDocViewItem.activeCollectionItem.callMetaInfoQuery();
                    productsCollectionViewContainer.wasChanged = false;
                }
            }
        }
    }

    Splitter {
        id: productSplitter;
        x: productsCollectionViewContainer.width - 300;

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

        anchors.right: parent.right;
        anchors.left: productSplitter.right;

        height: parent.height;

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

               text: "Product-Id";
            }
        }

        TextFieldCustom {
            id: tfcProductId;

            anchors.top: productIdTextRect.bottom;

            width: parent.width - 3;
            height: 30;

            text: productsCollectionViewContainer.itemId;

            onInputTextChanged: {
                productsCollectionViewContainer.wasChanged = true;
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

               text: "Product Name";
            }
        }

        TextFieldCustom {
            id: tfcProductName;

            anchors.top: productNameTextRect.bottom;

            width: parent.width - 3;
            height: 30;

            text: productsCollectionViewContainer.itemName;

            onInputTextChanged: {
                productsCollectionViewContainer.wasChanged = true;
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

                text: "Features";
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

            //modelItems: featuresTreeView.model;

            onItemTreeViewCheckBoxStateChanged: {
                console.log("ProductView TreeView onItemTreeViewCheckBoxStateChanged",
                            productsCollectionViewContainer.itemId,
                            productsCollectionView.table.getSelectedId(),
                            packageId,
                            featureId,
                            state);
               // productsCollectionViewContainer.wasChanged = true;
                productMetaInfo.clearTreeView();
                productMetaInfo.updateLicenseFeatures(productsCollectionViewContainer.itemId,
                                                      productsCollectionView.table.getSelectedId(),
                                                      packageId,
                                                      featureId,
                                                      state);
                productMetaInfo.updateTreeView();
            }
        }

        FeaturesTreeView {
            id: featuresTreeView;

            onModelTreeItemsChanged: {
                console.log("PackageView FeaturesTreeView onModelTreeViewChanged");
                treeView.modelItems = featuresTreeView.modelTreeItems;
            }

            onProductLicenseFeaturesChanged: {
                console.log("PackageView FeaturesTreeView onProductLicenseFeaturesChanged");
                productMetaInfo.clearTreeView();
                productMetaInfo.updateTreeView();
            }
        }

        function printModelItems(modelItems) {
            //var modelItems = treeView.modelItems;

            for (var i = 0; i < modelItems.GetItemsCount(); i++) {
                var modelChildren = modelItems.GetData("childItemModel", i);
                console.log("Package Id", modelItems.GetData("Id", i));

                if (modelChildren) {
                    for (var j = 0; j < modelChildren.GetItemsCount(); j++) {
                        console.log("\tFeature id ", modelChildren.GetData("Id", j));
                        console.log("\tstateChecked ", modelChildren.GetData("stateChecked", j));
                        console.log("\tisActive ", modelChildren.GetData("isActive", j));
                        console.log("\tvisible ", modelChildren.GetData("visible", j));
                    }
                }
            }
        }

        function clearTreeView(){
            console.log("ProductView clearTreeView");
            if(!treeView.modelItems){
                return;
            }

            var modelItems = treeView.modelItems;
            for(var i = 0; i < modelItems.GetItemsCount(); i++){
                var modelChildren = modelItems.GetData("childItemModel", i);
                if (!modelChildren){
                    continue;
                }

                for (var j = 0; j < modelChildren.GetItemsCount(); j++){
                    var isActive = modelChildren.GetData("isActive", j);
                    var state = modelChildren.GetData("stateChecked", j);
                    var id = modelChildren.GetData("Id", j);

                    if (isActive === 0){
                        console.log(id + " set active 1");
                        modelChildren.SetData("isActive", 1, j);
                    }

                    if (state === 2){
                        console.log(id + " set state 0");
                        modelChildren.SetData("stateChecked", 0, j);
                    }
                }

                modelItems.SetData("childItemModel", modelChildren, i);
            }

            treeView.modelItems =  modelItems;
            treeView.modelItems.Refresh();

            productMetaInfo.printModelItems(treeView.modelItems);
        }

        function updateTreeView(){
            console.log("ProductView updateTreeView");
            var i, j, k;

            var selectLicenseId = productsCollectionView.table.getSelectedId();
            var selectProductId = productsCollectionViewContainer.itemId;

            var modelPackages;

            if (!featuresTreeView.productLicenseFeatures){
                return;
            }

            console.log("A");
            for (i = 0; i < featuresTreeView.productLicenseFeatures.GetItemsCount(); i++){
                var curRootLicenseId = featuresTreeView.productLicenseFeatures.GetData("RootLicenseId", i);
                var curRootProductId = featuresTreeView.productLicenseFeatures.GetData("RootProductId", i);

                if (selectLicenseId === curRootLicenseId && selectProductId === curRootProductId){
                    modelPackages = featuresTreeView.productLicenseFeatures.GetData("Packages", i);
                    break;
                }
            }

            if (!modelPackages){
                return;
            }

            for(i = 0; i < treeView.modelItems.GetItemsCount(); i++){
                var treeViewPackageId = treeView.modelItems.GetData("Id", i);
                var packageIndex = -1;

                for (j = 0; j < modelPackages.GetItemsCount(); j++){
                    var licenseFeaturesPackageId = modelPackages.GetData("Id", j);

                    if (licenseFeaturesPackageId === treeViewPackageId){
                        packageIndex = j;
                        break;
                    }
                }

                if (packageIndex === -1){
                    continue;
                }

                var treeViewFeaturesModel = treeView.modelItems.GetData("childItemModel", i);

                if (!treeViewFeaturesModel){
                    continue;
                }

                var licenseFeaturesPackageFeaturesModel = modelPackages.GetData("Features", packageIndex);

                if (!licenseFeaturesPackageFeaturesModel){
                    continue;
                }

                for (j = 0; j < treeViewFeaturesModel.GetItemsCount(); j++){
                    var treeViewFeatureId = treeViewFeaturesModel.GetData("Id", j);
                    var ok = false;

                    for (k = 0; k < licenseFeaturesPackageFeaturesModel.GetItemsCount(); k++){
                        var licenseFeaturesFeatureId = licenseFeaturesPackageFeaturesModel.GetData("Id", k);

                        if (treeViewFeatureId === licenseFeaturesFeatureId){
                            ok = true;
                            break;
                        }
                    }

                    if (ok){
                        treeViewFeaturesModel.SetData("stateChecked", 2, j);

                        //Ищем фичи  от которых зависит выбранная, чтобы сделать их неактивными
                        productMetaInfo.updateDependsFeatures(treeViewPackageId, treeViewFeatureId);
                    }
                }

                treeView.modelItems.SetData("childItemModel", treeViewFeaturesModel, i);
            }

            productMetaInfo.printModelItems(treeView.modelItems);
        }

        function updateDependsFeatures(treeViewPackageId, treeViewFeatureId) {
            console.log("ProductView updateDependsFeatures", treeViewPackageId, treeViewFeatureId);
            var i, j, k, l;

            var dependsFeaturesIndex = -1;

            for (i = 0; i < featuresTreeView.modelDepends.GetItemsCount(); i++){
                var depFeatureId = featuresTreeView.modelDepends.GetData("RootFeatureId", i);
                var depPackageId = featuresTreeView.modelDepends.GetData("RootPackageId", i);

                if (depFeatureId === treeViewFeatureId && depPackageId === treeViewPackageId){
                    dependsFeaturesIndex = i;
                    break;
                }
            }

            if (dependsFeaturesIndex === -1){
                return;
            }

            var dependsPackagesModel = featuresTreeView.modelDepends.GetData("Packages", dependsFeaturesIndex);

            for (i = 0; i < treeView.modelItems.GetItemsCount(); i++) {
                var packageId = treeView.modelItems.GetData("Id", i);
                var modelChildren = treeView.modelItems.GetData("childItemModel", i);

                if (!modelChildren) {
                    continue;
                }

                for (j = 0; j < modelChildren.GetItemsCount(); j++) {
                    var id = modelChildren.GetData("Id", j);
                    for (k = 0; k < dependsPackagesModel.GetItemsCount(); k++){
                        var pId = dependsPackagesModel.GetData("Id", k);
                        var childItemModel = dependsPackagesModel.GetData("childItemModel", k);

                        for (l = 0; l < childItemModel.GetItemsCount(); l++){
                            var fId = childItemModel.GetData("Id", l);

                            if (pId === packageId && fId === id){
                                console.log("Find depend feature", packageId, id);
                                console.log("set isActive 0");
                                console.log("set state 2");
                                modelChildren.SetData("isActive", 0, j);
                                modelChildren.SetData("stateChecked", 2, j);
                            }
                        }
                    }
                }
                treeView.modelItems.SetData("childItemModel", modelChildren, i);
            }
        }

        function updateLicenseFeatures(productId, licenseId, packageId, featureId, state){
            console.log("ProductView updateLicenseFeatures", productId, licenseId, packageId, featureId, state);
            var licenseIndex = -1;
            var i;

            if (!featuresTreeView.productLicenseFeatures){
                return;
            }
            console.log("ProductView 1");
            for (i = 0; i < featuresTreeView.productLicenseFeatures.GetItemsCount(); i++){
                var curRootLicenseId = featuresTreeView.productLicenseFeatures.GetData("RootLicenseId", i);
                var curRootProductId = featuresTreeView.productLicenseFeatures.GetData("RootProductId", i);

                if (curRootLicenseId === licenseId && productId === curRootProductId){
                    licenseIndex = i;
                    break;
                }
            }

            if (licenseIndex === -1){
                licenseIndex = featuresTreeView.productLicenseFeatures.InsertNewItem();
                featuresTreeView.productLicenseFeatures.SetData("RootLicenseId", licenseId, licenseIndex);
                featuresTreeView.productLicenseFeatures.SetData("RootProductId", productId, licenseIndex);
                featuresTreeView.productLicenseFeatures.AddTreeModel("Packages", licenseIndex)
            }

            var packagesIndex = -1;
            var modelPackages = featuresTreeView.productLicenseFeatures.GetData("Packages", licenseIndex);

            for (i = 0; i < modelPackages.GetItemsCount(); i++){
                var curPackageId = modelPackages.GetData("Id", i);
                if (curPackageId === packageId){
                    packagesIndex = i;
                    break;
                }
            }

            if (packagesIndex === -1){
                packagesIndex = modelPackages.InsertNewItem();
                modelPackages.SetData("Id", packageId, packagesIndex);
                modelPackages.AddTreeModel("Features", packagesIndex);
            }

            var featureIndex = -1;
            var featuresModel = modelPackages.GetData("Features", packagesIndex);

            for (i = 0; i < featuresModel.GetItemsCount(); i++){
                var curFeatureId = featuresModel.GetData("Id", i);
                if (curFeatureId === featureId){
                    featureIndex = i;
                    break;
                }
            }

            if (state === 2 && featureIndex === -1){
                featureIndex = featuresModel.InsertNewItem();
                featuresModel.SetData("Id", featureId, featureIndex);
            }
            else if (state === 0 && featureIndex !== -1){
                featuresModel.RemoveItem(featureIndex);
            }

            modelPackages.SetData("Features", featuresModel, packagesIndex);

            if (featuresModel.GetItemsCount() === 0){
                modelPackages.RemoveItem(packagesIndex);
            }

            featuresTreeView.productLicenseFeatures.SetData("Packages", modelPackages, licenseIndex);
            if (modelPackages.GetItemsCount() === 0){
                featuresTreeView.productLicenseFeatures.RemoveItem(licenseIndex);
            }

            productMetaInfo.printModelItems(treeView.modelItems);
        }
    }
}
