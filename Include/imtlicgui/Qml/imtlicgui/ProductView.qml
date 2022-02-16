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

    property alias itemId: productCollectionView.itemId;
    property alias itemName: productCollectionView.itemName;

    property alias model: productCollectionView.collectionViewModel;

    property string typeOperation: multiDocViewItem.typeOperation;

    Component.onCompleted: {
        console.log("ProductView Component.onCompleted", productsCollectionViewContainer.itemId, productsCollectionViewContainer.itemName);
    }

    function dialogResult(parameters) {
         console.log("ProductView dialogResult", parameters["status"]);

        if (parameters["status"] === "ok") {

            if (parameters["dialog"] === "EditLicense") {
                var dataModelLocal = productCollectionView.collectionViewModel.GetData("data");
                console.log("ProductView onClicked ", dataModelLocal.GetItemsCount())
                dataModelLocal.SetData("Id", parameters["newLicenseId"] , productCollectionView.selectedIndex);//
                dataModelLocal.SetData("Name", parameters["newLicenseName"], productCollectionView.selectedIndex);

                productCollectionView.collectionViewModel.SetData("data", dataModelLocal);
                productCollectionView.refresh();
            }
            else if (parameters["dialog"] === "InputDialog") {
                var value = parameters["value"];
                console.log("LicenseCollectionViewContainer dialogResult", value);
                productsCollectionViewContainer.rootItem.updateTitleTab(productsCollectionViewContainer.itemId, value);
                saveModel.updateModel(value);
            }
        }
        else if (parameters["status"] === "yes") {

            if (productCollectionView.collectionViewModel.ContainsKey("data")) {
                var dataModelLocal = productCollectionView.collectionViewModel.GetData("data");
                dataModelLocal.RemoveItem(productCollectionView.table.selectedIndex);

                productCollectionView.collectionViewModel.SetData("data", dataModelLocal);
                productCollectionView.collectionViewModel.Refresh();
                productCollectionView.refresh();

                productCollectionView.table.selectedIndex = -1;
                productsCollectionViewContainer.commandsChanged("ProductEdit");
                productsCollectionViewContainer.makeCommandActive("Save");
            }
        }
    }

    function makeCommandActive(commandId){
        productsCollectionViewContainer.rootItem.setModeMenuButton(commandId, "Normal");
    }

    function createLicense(id, name) {
        var dataModelLocal = model.GetData("data");
        var index = dataModelLocal.InsertNewItem();

        dataModelLocal.SetData("Name", name, index);
        dataModelLocal.SetData("Id", id, index);

        model.SetData("data", dataModelLocal);
        model.Refresh();
        productCollectionView.refresh();
    }

    function menuActivated(menuId) {

        console.log("ProductView menuActivated", menuId);
        if (menuId  === "New"){
            productsCollectionViewContainer.createLicense("", "License Name");
        }
        else if (menuId  === "Save") {
            if (productCollectionView.itemId === "") {
                var source = "AuxComponents/InputDialog.qml";
                var parameters = {};
                parameters["message"] = "Please enter the name of the document: ";
                parameters["nameDialog"] = "Document Name";
                parameters["resultItem"] = productsCollectionViewContainer;
                thubnailDecoratorContainer.openDialog(source, parameters);
            }
            else {
                saveModel.updateModel()
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
            var duplicateName = "Copy of " + dataModelLocal.GetData("Name", productCollectionView.selectedIndex);
            var duplicateId = dataModelLocal.GetData("Id", productCollectionView.selectedIndex);
            productsCollectionViewContainer.createLicense(duplicateId, duplicateName);

        } else {
            productCollectionView.menuActivated(menuId)
        }
    }

    function refresh() {
        console.log("PackageView refresh()");
        productCollectionView.refresh();
    }

    function commandsChanged(commandsId){
        if (commandsId !== "ProductEdit") {
            return;
        }

        if (productCollectionView.selectedIndex > -1) {
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
        id: productModel;
    }

    CollectionView {
        id: productCollectionView;

        anchors.left: parent.left;
        anchors.right: productSplitter.left;

        height: parent.height;

        Component.onCompleted: {
            console.log("ProductView CollectionView onCompleted");
            productCollectionView.gqlModelInfo = "ProductInfo"
            productCollectionView.gqlModelItems = "LicenseList"
            productCollectionView.gqlModelRemove = "";
        }

        onItemIdChanged: {
            if (productCollectionView.itemId){
                console.log("ProductView onItemIdChanged")
                productCollectionView.gqlModelInfo = "ProductInfo"
                productCollectionView.gqlModelItems = "LicenseList"
                productCollectionView.gqlModelRemove = "";
            }
        }

        onSelectItem: {
            console.log("ProductView CollectionView onSelectItem", selectedId, name);

            var source = "../imtlicgui/EditLicenseDialog.qml";
            var parameters = {};
            parameters["licenseId"] = selectedId;
            parameters["licenseName"] = name;
            parameters["collectionViewLicenses"] = productCollectionView;
            parameters["resultItem"] = productsCollectionViewContainer;

            thubnailDecoratorContainer.openDialog(source, parameters);
        }

        onSelectedIndexChanged: {
            console.log("PackageView CollectionView onSelectedIndexChanged", productCollectionView.selectedIndex);
            if (productCollectionView.selectedIndex > -1){
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
        id: saveModel;

        function updateModel() {
            console.log( "updateModel saveModel", productsCollectionViewContainer.typeOperation);

            var query;
            var queryFields;
            var inputParams = Gql.GqlObject("input");

            if (productsCollectionViewContainer.typeOperation == "Open") {
                query = Gql.GqlRequest("query", "ProductUpdate");
                inputParams.InsertField("Id");
                inputParams.InsertFieldArgument("Id", productsCollectionViewContainer.itemId);
                queryFields = Gql.GqlObject("updatedNotification");
            } else {
                query = Gql.GqlRequest("query", "ProductAdd");
                queryFields = Gql.GqlObject("addedNotification");
            }

//            if(productsCollectionViewContainer.itemId != "" && productsCollectionViewContainer.typeOperation != "Copy"){
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

            productModel.SetData("Id", productsCollectionViewContainer.itemId)
            productModel.SetData("Name", productsCollectionViewContainer.itemName)
            productModel.SetExternTreeModel("licenses", productCollectionView.collectionViewModel.GetData("data"));
            productModel.SetExternTreeModel("dependencies", featuresTreeView.productLicenseFeatures);

            //featureCollectionViewContainer.model.SetIsArray(false);
            var jsonString = productModel.toJSON();
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
            console.log("State:", this.state, saveModel);
            if (this.state === "Ready"){

                productsCollectionViewContainer.multiDocViewItem.activeCollectionItem.refresh();

                var dataModelLocal = saveModel.GetData("data");
                if (dataModelLocal.ContainsKey("ProductAdd")) {
                    dataModelLocal = dataModelLocal.GetData("ProductAdd");

                    if(dataModelLocal.ContainsKey("addedNotification")){
                        dataModelLocal = dataModelLocal.GetData("addedNotification");
                        console.log("ProductView saveModel addedNotification exist");
                         productCollectionView.refresh();
                        if(dataModelLocal.ContainsKey("Id")){
                            console.log("ProductView saveModel addedNotification  Id exist");
                            productsCollectionViewContainer.itemId = dataModelLocal.GetData("Id");
                            //productsCollectionViewContainer.itemName = dataModelLocal.GetData("Name");

                            productsCollectionViewContainer.rootItem.updateTitleTab(productsCollectionViewContainer.itemId, productsCollectionViewContainer.itemName);
                        }
                        else if(saveModel.ContainsKey("errors")){
                            var errorsModel = accountItemModel.GetData("errors");
                            if(errorsModel !== null && errorsModel.ContainsKey(containerContactInfo.gqlModelItems)){
                                console.log("message", errorsModel.GetData(productsCollectionViewContainer.gqlModelItems).GetData("message"));
                            }
                        }
                    }
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

        color: "transparent";


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

//        TextFieldCustom {
//            id: tfcProductId;

//            anchors.top: productIdTextRect.bottom;

//            width: parent.width - 3;
//            height: 30;

//            text: productsCollectionViewContainer.itemId;

//            onTextChanged: {
//                productsCollectionViewContainer.itemId = tfcProductId.text;
//            }
//        }


        Rectangle {
            id: productNameTextRect;
//            anchors.top: tfcProductId.bottom;

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

//        TextFieldCustom {
//            id: tfcProductName;
//            anchors.top: productNameTextRect.bottom;
//            width: parent.width - 3;
//            height: 30;

//            text: productsCollectionViewContainer.itemName;

//            onTextChanged: {
//                productsCollectionViewContainer.itemName = tfcProductName.text;
//            }
//        }

        Rectangle {
            id: headerTreeView;
//            anchors.top: tfcProductName.bottom;
            anchors.topMargin: 5;
//            anchors.horizontalCenter: parent.horizontalCenter;
            width: parent.width - 5;

            height: 35;
            color: Style.theme === "Light" ? "white": Style.backgroundColor;

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
//            anchors.leftMargin: 5;
            anchors.right: parent.right;
            anchors.rightMargin: 5;
            clip: true;

            visible: false;

            anchors.bottom: parent.bottom;
//            width: 200;
//            height: 500;
            modelItems: featuresTreeView.model;

            onItemTreeViewCheckBoxStateChanged: {
                console.log("ProductView TreeView onItemTreeViewCheckBoxStateChanged",
                            productsCollectionViewContainer.itemId,
                            productCollectionView.table.getSelectedId(),
                            packageId,
                            featureId,
                            state);
                productMetaInfo.clearTreeView();
                productMetaInfo.updateLicenseFeatures(productsCollectionViewContainer.itemId,
                                                      productCollectionView.table.getSelectedId(),
                                                      packageId,
                                                      featureId,
                                                      state);
                productMetaInfo.updateTreeView();

            }
        }

        FeaturesTreeView {
            id: featuresTreeView;

            onDependModelChanged: {
                console.log( "PackageView FeaturesTreeView onDependModelChanged");
                //featureCollectionViewContainer.updateFeaturesTreeView();
            }

            onModelTreeViewChanged: {
                console.log("PackageView FeaturesTreeView onModelTreeViewChanged");
                //featureCollectionViewContainer.updateFeaturesTreeView();

                treeView.modelItems = featuresTreeView.modelTreeView;
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
            var modelItems = treeView.modelItems;

            for(var i = 0; i < modelItems.GetItemsCount(); i++){

                var childModel = modelItems.GetData("childItemModel", i);

                for (var j = 0; j < childModel.GetItemsCount(); j++){
                    var isActive = childModel.GetData("isActive", j);
                    var state = childModel.GetData("stateChecked", j);

                    if (isActive === 0){
                        childModel.SetData("isActive", 1, j);
                    }

                    if (state === 2){
                        childModel.SetData("stateChecked", 0, j);
                    }
                }

                modelItems.SetData("childItemModel", childModel, i);
            }

            treeView.modelItems =  modelItems;
        }

        function updateTreeView(){
            console.log("ProductView updateTreeView");
            var i, j, k;

            var selectLicenseId = productCollectionView.table.getSelectedId();
            var selectProductId = productsCollectionViewContainer.itemId;

            var packagesModel;

            if (!featuresTreeView.productLicenseFeatures){
                return;
            }

            for (i = 0; i < featuresTreeView.productLicenseFeatures.GetItemsCount(); i++){
                var curRootLicenseId = featuresTreeView.productLicenseFeatures.GetData("RootLicenseId", i);
                var curRootProductId = featuresTreeView.productLicenseFeatures.GetData("RootProductId", i);

                if (selectLicenseId === curRootLicenseId && selectProductId === curRootProductId){
                    packagesModel = featuresTreeView.productLicenseFeatures.GetData("Packages", i);
                    break;
                }
            }

            if (!packagesModel){
                return;
            }

            for(i = 0; i < treeView.modelItems.GetItemsCount(); i++){
                var treeViewPackageId = treeView.modelItems.GetData("Id", i);

                var packageIndex = -1;

                for (j = 0; j < packagesModel.GetItemsCount(); j++){
                    var licenseFeaturesPackageId = packagesModel.GetData("Id", j);

                    if (licenseFeaturesPackageId === treeViewPackageId){
                        packageIndex = j;
                        break;
                    }
                }

                if (packageIndex === -1){
                    continue;
                }

                var treeViewFeaturesModel = treeView.modelItems.GetData("childItemModel", i);
                var licenseFeaturesPackageFeaturesModel = packagesModel.GetData("Features", packageIndex);

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

            for (i = 0; i < featuresTreeView.dependModel.GetItemsCount(); i++){
                var depFeatureId = featuresTreeView.dependModel.GetData("RootFeatureId", i);
                var depPackageId = featuresTreeView.dependModel.GetData("RootPackageId", i);

                if (depFeatureId === treeViewFeatureId && depPackageId === treeViewPackageId){
                    dependsFeaturesIndex = i;
                    break;
                }
            }

            if (dependsFeaturesIndex === -1){
                return;
            }

            var dependsPackagesModel = featuresTreeView.dependModel.GetData("Packages", dependsFeaturesIndex);

            for (i = 0; i < treeView.modelItems.GetItemsCount(); i++) {
                var packageId = treeView.modelItems.GetData("Id", i);
                var childModel = treeView.modelItems.GetData("childItemModel", i);


                if (!childModel) {
                    continue;
                }

                for (j = 0; j < childModel.GetItemsCount(); j++) {

                    var id = childModel.GetData("Id", j);

                    for (k = 0; k < dependsPackagesModel.GetItemsCount(); k++){
                        var pId = dependsPackagesModel.GetData("Id", k);

                        var childItemModel = dependsPackagesModel.GetData("childItemModel", k);

                        for (l = 0; l < childItemModel.GetItemsCount(); l++){
                            var fId = childItemModel.GetData("Id", l);

                            if (pId === packageId && fId === id){
                                console.log("Find depend feature", packageId, id);
                                childModel.SetData("isActive", 0, j);
                                childModel.SetData("stateChecked", 2, j);
                            }

                        }
                    }
                }
                treeView.modelItems.SetData("childItemModel", childModel, i);
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

            var packagesModel = featuresTreeView.productLicenseFeatures.GetData("Packages", licenseIndex);

            for (i = 0; i < packagesModel.GetItemsCount(); i++){
                var curPackageId = packagesModel.GetData("Id", i);

                if (curPackageId === packageId){
                    packagesIndex = i;
                    break;
                }
            }

            if (packagesIndex === -1){
                packagesIndex = packagesModel.InsertNewItem();
                packagesModel.SetData("Id", packageId, packagesIndex);
                packagesModel.AddTreeModel("Features", packagesIndex);
            }

            var featureIndex = -1;

            var featuresModel = packagesModel.GetData("Features", packagesIndex);

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

            packagesModel.SetData("Features", featuresModel, packagesIndex);

            if (featuresModel.GetItemsCount() === 0){
                packagesModel.RemoveItem(packagesIndex);
            }

            featuresTreeView.productLicenseFeatures.SetData("Packages", packagesModel, licenseIndex);

            if (packagesModel.GetItemsCount() === 0){
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
