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

//    property var model;

    property alias itemId: productsCollectionView.itemId;
    property alias itemName: productsCollectionView.itemName;
    property alias model: productsCollectionView.collectionViewModel;

    property string operation;
    property string gqlModelQueryType;
    property string gqlModelQueryTypeNotification;

    Component.onCompleted: {
        console.log("ProductView onCompleted",  productsCollectionView.selectedIndex);

        console.log("ProductView onCompleted",  productsCollectionView.height);
        console.log("ProductView onCompleted",  productsCollectionView.width);

        console.log("ProductView collection size",   productsCollectionView.table);
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
        var source = "AuxComponents/PopupMenuDialog.qml";
        var parameters = {};
        parameters["model"] = contextMenuModel;
        parameters["resultItem"] = productsCollectionViewContainer;
        parameters["itemHeight"] = 25;
        parameters["itemWidth"] = 150;
        parameters["x"] = mouseX + 75;
        parameters["y"] = mouseY + 130;

        thubnailDecoratorContainer.openDialog(source, parameters);
    }

    function dialogResult(parameters) {
         console.log("ProductView dialogResult", parameters["dialog"], parameters["status"]);

        if (parameters["dialog"] === "EditLicense"){

            if (parameters["status"] === "ok") {
                var dataModelLocal = productsCollectionView.collectionViewModel.GetData("data");
                console.log("ProductView onClicked ", dataModelLocal.GetItemsCount())
                dataModelLocal.SetData("Id", parameters["newLicenseId"] , productsCollectionView.selectedIndex);//
                dataModelLocal.SetData("Name", parameters["newLicenseName"], productsCollectionView.selectedIndex);

                productsCollectionView.collectionViewModel.SetData("data", dataModelLocal);
                productsCollectionView.refresh();
            }
        }
        else if (parameters["dialog"] === "InputDialog"){

//            if (parameters["status"] === "ok") {
//                var value = parameters["value"];
//                console.log("LicenseCollectionViewContainer dialogResult", value);
//                productsCollectionViewContainer.rootItem.updateTitleTab(productsCollectionViewContainer.itemId, value);
//                productViewSaveQuery.updateModel(value);
//            }

            if (parameters["typeOperation"] === "Save") {
                var value = parameters["value"];
                productViewSaveQuery.updateModel(value);
            }
            else if (parameters["typeOperation"] === "SetDescription") {
                var value = parameters["value"];
                var dataModelLocal = productsCollectionView.collectionViewModel.GetData("data");

                dataModelLocal.SetData("Description", value, productsCollectionView.table.selectedIndex);
                productsCollectionView.collectionViewModel.SetData("data", dataModelLocal);

                productsCollectionView.refresh();
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
                    productsCollectionViewContainer.commandsChanged("ProductEdit");
                    productsCollectionViewContainer.makeCommandActive("Save");
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

//        if (parameters["status"] === "ok") {

//            if (parameters["dialog"] === "EditLicense") {
//                var dataModelLocal = productsCollectionView.collectionViewModel.GetData("data");
//                console.log("ProductView onClicked ", dataModelLocal.GetItemsCount())
//                dataModelLocal.SetData("Id", parameters["newLicenseId"] , productsCollectionView.selectedIndex);//
//                dataModelLocal.SetData("Name", parameters["newLicenseName"], productsCollectionView.selectedIndex);

//                productsCollectionView.collectionViewModel.SetData("data", dataModelLocal);
//                productsCollectionView.refresh();
//            }
//            else if (parameters["dialog"] === "InputDialog") {
//                var value = parameters["value"];
//                console.log("LicenseCollectionViewContainer dialogResult", value);
//                productsCollectionViewContainer.rootItem.updateTitleTab(productsCollectionViewContainer.itemId, value);
//                productViewSaveQuery.updateModel(value);
//            }
//        }
//        else if (parameters["status"] === "yes") {

//            if (productsCollectionView.collectionViewModel.ContainsKey("data")) {
//                var dataModelLocal = productsCollectionView.collectionViewModel.GetData("data");
//                dataModelLocal.RemoveItem(productsCollectionView.table.selectedIndex);

//                productsCollectionView.collectionViewModel.SetData("data", dataModelLocal);
//                productsCollectionView.collectionViewModel.Refresh();
//                productsCollectionView.refresh();

//                productsCollectionView.table.selectedIndex = -1;
//                productsCollectionViewContainer.commandsChanged("ProductEdit");
//                productsCollectionViewContainer.makeCommandActive("Save");
//            }
//        }
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

    function createLicense(id, name) {
        var dataModelLocal = model.GetData("data");
        var index = dataModelLocal.InsertNewItem();

        dataModelLocal.SetData("Name", name, index);
        dataModelLocal.SetData("Id", id, index);
        dataModelLocal.SetData("Description", "", index);

        model.SetData("data", dataModelLocal);
//        model.Refresh();
        productsCollectionView.refresh();
    }

    function menuActivated(menuId) {

        console.log("ProductView menuActivated", menuId);
        if (menuId  === "New"){
            productsCollectionViewContainer.createLicense("", "License Name");
        }
        else if (menuId  === "Save") {
            if (productsCollectionView.itemId === "") {
                var source = "AuxComponents/InputDialog.qml";
                var parameters = {};
                parameters["message"] = "Please enter the name of the document: ";
                parameters["nameDialog"] = "Document Name";
                parameters["resultItem"] = productsCollectionViewContainer;
                thubnailDecoratorContainer.openDialog(source, parameters);
            }
            else {
                productViewSaveQuery.updateModel()
            }
        }else if (menuId  === "Remove") {
            var source = "AuxComponents/MessageDialog.qml";
            var parameters = {};
            parameters["message"] = "Remove selected file from the database ?";
            parameters["nameDialog"] = "RemoveDialog";
            parameters["resultItem"] = productsCollectionViewContainer;
            thubnailDecoratorContainer.openDialog(source, parameters);
        }else if (menuId  === "Close") {
            productsCollectionViewContainer.rootItem.closeTab();
        } else if (menuId  === "Duplicate") {
            var dataModelLocal = model.GetData("data");
            var duplicateName = "Copy of " + dataModelLocal.GetData("Name", productsCollectionView.selectedIndex);
            var duplicateId = dataModelLocal.GetData("Id", productsCollectionView.selectedIndex);
            productsCollectionViewContainer.createLicense(duplicateId, duplicateName);

        } else {
            productsCollectionView.menuActivated(menuId)
        }
    }

    function refresh() {
        console.log("PackageView refresh()");
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
            productsCollectionViewContainer.rootItem.setModeMenuButton("Save", "Normal");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Close", "Normal");
        } else {
            productsCollectionViewContainer.rootItem.setModeMenuButton("Remove", "Disabled");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Edit", "Disabled");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Duplicate", "Disabled");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Export", "Disabled");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Save", "Disabled");
        }
    }

    TreeItemModel {
        id: modelProducts;
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
            } else {
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
            } else {
                productsCollectionViewContainer.gqlModelQueryType = "ProductAdd";
                productsCollectionViewContainer.gqlModelQueryTypeNotification = "addedNotification";

                query = Gql.GqlRequest("query", "ProductAdd");
                queryFields = Gql.GqlObject("addedNotification");
            }

//            if(productsCollectionViewContainer.itemId != "" && productsCollectionViewContainer.operation != "Copy"){
//                query = Gql.GqlRequest("query", "ProductUpdate");
//                inputParams.InsertField("Id");
//                inputParams.InsertFieldArgument("Id", productsCollectionViewContainer.itemId);
//                queryFields = Gql.GqlObject("updatedNotification");
//            }
//            else{
//                query = Gql.GqlRequest("query", "ProductAdd");
//                queryFields = Gql.GqlObject("addedNotification");
//            }
            query.AddParam(inputParams);

//            if (newId !== undefined && newId !== "") {
//                //console.log();
//                productsCollectionViewContainer.itemId = newId;
//                productsCollectionViewContainer.itemName = newId;
//            }

            modelProducts.SetData("Id", productsCollectionViewContainer.itemId)
            modelProducts.SetData("Name", productsCollectionViewContainer.itemName)
            modelProducts.SetExternTreeModel("licenses", productsCollectionView.collectionViewModel.GetData("data"));
            modelProducts.SetExternTreeModel("dependencies", featuresTreeView.productLicenseFeatures);

            //featureCollectionViewContainer.model.SetIsArray(false);
            var jsonString = modelProducts.toJSON();
            console.log("jsonString", jsonString)
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")
            console.log("jsonString", jsonString)

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

                    dataModelLocal = dataModelLocal.GetData(productsCollectionViewContainer.gqlModelQueryTypeNotification);

                    productsCollectionView.refresh();

                    var newId, newName;

                    if(dataModelLocal.ContainsKey("Id")){
                        newId = dataModelLocal.GetData("Id");
                    }

                    if (dataModelLocal.ContainsKey("Name")){
                        newName = dataModelLocal.GetData("Name");
                    }

                    if (productsCollectionView.itemId !== newId){
                        productsCollectionView.itemId = newId;
                        productsCollectionView.itemName = newId;

                        productsCollectionViewContainer.rootItem.updateTitleTab(productsCollectionViewContainer.itemId, productsCollectionViewContainer.itemName);
                    }



//                    if (productsCollectionView.itemName !== newName){
//                        productsCollectionView.itemName = newName;
//                        productsCollectionViewContainer.rootItem.updateTitleTab(productsCollectionViewContainer.itemId, productsCollectionViewContainer.itemName);
//                    }

                    productsCollectionViewContainer.multiDocViewItem.activeCollectionItem.callMetaInfoQuery();
//                    productsCollectionViewContainer.multiDocViewItem.activeCollectionItem.refresh();
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

               text: "Product-ID";
            }
        }

        TextFieldCustom {
            id: tfcProductId;

            anchors.top: productIdTextRect.bottom;

            width: parent.width - 3;
            height: 30;

            text: productsCollectionViewContainer.itemId;

            onTextChanged: {
                productsCollectionViewContainer.itemId = tfcProductId.text;
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

            onTextChanged: {
                productsCollectionViewContainer.itemName = tfcProductName.text;
            }
        }

        Rectangle {
            id: headerTreeView;

            anchors.top: tfcProductName.bottom;
            anchors.topMargin: 5;

            width: parent.width - 5;
            height: 35;

            color: Style.theme === "Light" ? "white": Style.backgroundColor;

//            color: "red";

            Text {
                id: titleHeader;

                anchors.verticalCenter: headerTreeView.verticalCenter;
                anchors.left: headerTreeView.left;
                anchors.leftMargin: 10;

                text: "Features";
                font.family: Style.fontFamilyBold;
                color: Style.textColor;
                font.pixelSize: Style.fontSize_small;
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

            modelItems: featuresTreeView.model;

            onItemTreeViewCheckBoxStateChanged: {
                console.log("ProductView TreeView onItemTreeViewCheckBoxStateChanged",
                            productsCollectionViewContainer.itemId,
                            productsCollectionView.table.getSelectedId(),
                            packageId,
                            featureId,
                            state);
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

            onModelDependsChanged: {
                console.log( "PackageView FeaturesTreeView onDependModelChanged");
            }

            onModelTreeItemsChanged: {
                console.log("PackageView FeaturesTreeView onModelTreeViewChanged");
                //featureCollectionViewContainer.updateFeaturesTreeView();

                treeView.modelItems = featuresTreeView.modelTreeItems;
            }

            onProductLicenseFeaturesChanged: {
                console.log("PackageView FeaturesTreeView onProductLicenseFeaturesChanged");
                productMetaInfo.clearTreeView();
                productMetaInfo.updateTreeView();
//                treeView.modelItems.Refresh();
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

                for (var j = 0; j < modelChildren.GetItemsCount(); j++){
                    var isActive = modelChildren.GetData("isActive", j);
                    var state = modelChildren.GetData("stateChecked", j);

                    if (isActive === 0){
                        modelChildren.SetData("isActive", 1, j);
                    }

                    if (state === 2){
                        modelChildren.SetData("stateChecked", 0, j);
                    }
                }

                modelItems.SetData("childItemModel", modelChildren, i);
            }

            treeView.modelItems =  modelItems;
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
                var licenseFeaturesPackageFeaturesModel = modelPackages.GetData("Features", packageIndex);

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

            //PRINT
            console.log();
            for (var i = 0; i < featuresTreeView.productLicenseFeatures.GetItemsCount(); i++){
                console.log("Root LicenseId", featuresTreeView.productLicenseFeatures.GetData("RootLicenseId", i));
                var packageModel = featuresTreeView.productLicenseFeatures.GetData("Packages", i);

                for (var j = 0; j < packageModel.GetItemsCount(); j++){
                    console.log("\tPackageId", packageModel.GetData("Id", j));

                    var featureModel = packageModel.GetData("Features", j);

                    for (var k = 0; k < featureModel.GetItemsCount(); k++){
                        console.log("\t\tFeatureId", featureModel.GetData("Id", k));
                    }
                }
            }


        }
    }
}
