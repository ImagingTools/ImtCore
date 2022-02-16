import QtQuick 2.0
import imtauthgui 1.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Rectangle {
    id: containerInstallation;

    anchors.fill: parent;

    width: 500;
    height: 500;

    property Item rootItem;
    property Item multiDocViewItem;
    property string itemId;
    property string itemName;

    property string accountId;
    property string productId;

    property string gqlModelInfo;
    property string gqlModelCollectionInfo;

    property string typeOperation: multiDocViewItem.typeOperation;

    property TreeItemModel installationInfoModel;
    property TreeItemModel model;

    color: Style.backgroundColor;

    Component.onCompleted: {
//        if (containerInstallation.typeOperation === "New") {
            containerInstallation.gqlModelInfo = "ProductInfo";
            containerInstallation.gqlModelCollectionInfo = "ProductList"
            headerInfoModel.updateModel();
//        }
    }

    onModelChanged: {
        console.log("InstallationInfoEditor onModelChanged", containerInstallation.typeOperation);
        if (containerInstallation.model.ContainsKey("data")){
            containerInstallation.contactInfoModel = containerContactInfo.model.GetData('data');
            containerInstallation.updateData();
        }
        else {
            if(containerInstallation.typeOperation === "New"){
                //containerContactInfo.accountType = "company";
//                cbTypeAccount.currentIndex = 0;
//                containerContactInfo.contactInfoModel = model.AddTreeModel("data");
//                containerContactInfo.contactInfoModel.SetData("Email","")
//                containerContactInfo.contactInfoModel.SetData("FirstName","")
//                containerContactInfo.contactInfoModel.SetData("LastName","")
            }
            else{

            }
        }
    }

    onInstallationInfoModelChanged: {
        console.log("InstallationInfoEditor onInstallationInfoModelChanged");
        if (containerInstallation.typeOperation !== "New"){
            containerInstallation.updateData();
        }
    }

    function updateData() {
        console.log("containerContactInfo updateData");

        instanceIdText.text = containerInstallation.installationInfoModel.GetData("Id");

        var accountId = containerInstallation.installationInfoModel.GetData("AccountId");
        var producId = containerInstallation.installationInfoModel.GetData("ProductId");

        var accountIndex = -1;
        var accountsData = accounts.GetData("data");
        for (var i = 0; i < accountsData.GetItemsCount(); i++){

            var accId = accountsData.GetData("Id", i);

            if (accId === accountId){
                accountIndex = i;
            }
        }

        if (accountIndex !== -1){
            customerCB.currentIndex = accountIndex;
        }

        var productIndex = -1;
        var productsData = products.GetData("data");
        for (var i = 0; i < productsData.GetItemsCount(); i++){

            var prId = productsData.GetData("Id", i);

            if (prId === producId){
                productIndex = i;
            }
        }

        if (productIndex !== -1){
            productCB.currentIndex = productIndex;
        }
    }

    TreeItemModel {
        id: products;
    }

    TreeItemModel {
        id: licenses;
    }

    TreeItemModel {
        id: activeLicenses;
    }

    TreeItemModel {
        id: accounts;
    }

    TreeItemModel {
        id: installationModel;
    }

    function dialogResult(parameters) {
         console.log("InstallationInfoEditor dialogResult", parameters["status"]);

        if (parameters["status"] === "ok") {

            if (parameters["dialog"] === "InputDialog") {
                var value = parameters["value"];
                console.log("featureCollectionViewContainer dialogResult", value);
                containerInstallation.rootItem.updateTitleTab(containerInstallation.itemId, value);
                saveModel.updateModel(value);
            }
        }
    }

    function menuActivated(menuId) {
        console.log("InstallationInfoEditor menuActivated", menuId);
        if (menuId  === "New"){
//            collectionViewContainer.selectItem("", "")
        }
        else if (menuId  === "Save") {
            //saveModel.updateModel();
            if (containerInstallation.typeOperation === "New") {
                var source = "AuxComponents/InputDialog.qml";
                var parameters = {};
                parameters["message"] = "Please enter the name of the document: ";
                parameters["nameDialog"] = "Document Name";
                parameters["resultItem"] = containerInstallation;
                thubnailDecoratorContainer.openDialog(source, parameters);
            }
        }
        else if (menuId === "Close") {
            containerInstallation.rootItem.closeTab();
        }
    }

    function refresh(){}

    function commandsChanged(commandsId){
        if (commandsId !== "InstallationEdit") {
            return;
        }

        containerInstallation.rootItem.setModeMenuButton("Save", "Normal");
        containerInstallation.rootItem.setModeMenuButton("New", "Normal");
        containerInstallation.rootItem.setModeMenuButton("Close", "Normal");
    }

    Flickable {
        anchors.fill: parent;

        contentWidth: container.width;
        contentHeight: container.height + 50;
        boundsBehavior: Flickable.StopAtBounds;
        clip: true;

        Column {
             id: container;

             width: 500;

             spacing: 7;

             Text {
                 id: titleInstance;

                 anchors.left: container.left;
                 anchors.leftMargin: 10;

                 text: qsTr("Instance-ID");
                 color: Style.textColor;
                 font.family: Style.fontFamily;
                 font.pixelSize: Style.fontSize_common;
             }

             Rectangle {
                 id: tfcInstance;

                 anchors.horizontalCenter: container.horizontalCenter;

                 width: container.width - 20;
                 height: 45;

                 color: Style.imagingToolsGradient1;
                 border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

                 TextFieldCustom {
                     id: instanceIdText;

                     anchors.horizontalCenter: tfcInstance.horizontalCenter;
                     anchors.verticalCenter: tfcInstance.verticalCenter;

                     width: tfcInstance.width - 22;
                     height: 23;
                 }
             }

             Text {
                 id: titleCustomer;

                 anchors.left: container.left;
                 anchors.leftMargin: 10;

                 text: qsTr("Customer");
                 color: Style.textColor;

                 font.family: Style.fontFamily;
                 font.pixelSize: Style.fontSize_common;
             }

             Rectangle {
                 id: customerBlock;
                 z: 10;

                 anchors.horizontalCenter: container.horizontalCenter;

                 height: 45;
                 width: container.width - 20;
                 color: Style.imagingToolsGradient1;
                 border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

                 ComboBox {
                     id: customerCB;
                     //z: 10;

                     anchors.horizontalCenter: customerBlock.horizontalCenter;
                     anchors.verticalCenter: customerBlock.verticalCenter;

                     width: customerBlock.width - 22;
                     height: 23;

                     radius: 3;
                     model: listModelAccounts;

                     backgroundColor: "#d0d0d0";
                     borderColor: Style.theme == "Dark" ? "#565757" : "#a4a4a6";
                     textCentered: false;

                     onCurrentIndexChanged: {
                         console.log("InstallationInfoEditor customerCB onCurrentIndexChanged");
                         containerInstallation.accountId = listModelAccounts.get(customerCB.currentIndex).id;
                         customerCB.currentText = listModelAccounts.get(customerCB.currentIndex).text;
                     }
                 }
             }

             Text {
                 id: titleProduct;
                 z : 5;

                 anchors.left: container.left;
                 anchors.leftMargin: 10;

                 text: qsTr("Product");
                 color: Style.textColor;
                 font.family: Style.fontFamily;
                 font.pixelSize: Style.fontSize_common;
             }

             Rectangle {
                 id: productBlock;
                 z : 5;

                 anchors.horizontalCenter: container.horizontalCenter;

                 height: 45;
                 width: container.width - 20;

                 color: Style.imagingToolsGradient1;
                 border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

                 ComboBox {
                     id: productCB;
                   //  z: 5;

                     anchors.horizontalCenter: productBlock.horizontalCenter;
                     anchors.verticalCenter: productBlock.verticalCenter;

                     width: productBlock.width - 22;
                     height: 23;

                     radius: 3;
                     model: listModelProducts;

                     backgroundColor: "#d0d0d0";
                     borderColor: Style.theme == "Dark" ? "#565757" : "#a4a4a6";
                     textCentered: false;

                     onCurrentIndexChanged: {
                         console.log("InstallationInfoEditor productCB onCurrentIndexChanged");
                         containerInstallation.productId = listModelProducts.get(productCB.currentIndex).id;
                         productCB.currentText = listModelProducts.get(productCB.currentIndex).text;

                         licensesModel.updateModel(containerInstallation.productId);
                     }

                 }
             }

             Text {
                 id: titleLicenses;

                 anchors.left: container.left;
                 anchors.leftMargin: 10;

                 text: qsTr("Licenses");
                 color: Style.textColor;
                 font.family: Style.fontFamily;
                 font.pixelSize: Style.fontSize_common;
             }

             Rectangle {
                 id: licensesBlock;

                 anchors.horizontalCenter: container.horizontalCenter;

                 width: container.width - 20;
                 height: 200;

                 color: Style.imagingToolsGradient1;
                 border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

                 AuxTable {
                     id: licensesTable;

                     anchors.top: licensesBlock.top;
                     anchors.topMargin: 10;
                     anchors.bottom: parent.bottom;
                     anchors.left: parent.left;
                     anchors.leftMargin: 10;
                     anchors.right: parent.right;
                     anchors.rightMargin: 10;

                     headers: headersModelLicenses;

                     delegate: TableInstanceLicensesDelegate {
                         id: delegate;

                         width: licensesTable.width;
                         height: 35;

                         name: model.Name;

                         selected: licensesTable.selectedIndex === model.index;

                         onClicked: {

                             licensesTable.selectedIndex = model.index;
                         }

                         onCheckBoxLicenseClicked: {
                             var licensesLocal = licenses.GetData("data");
                             if (state == 2) {
                                 var licenseId = licensesLocal.GetData("Id", modelIndex);
                                 var index = activeLicenses.InsertNewItem();
                                 activeLicenses.SetData("Id", licenseId, index);
                                 activeLicenses.SetData("Expiration", "unlimited", index);
                             } else {
                                var licenseId = licensesLocal.GetData("Id", modelIndex);

                                 for (var i = 0; i < activeLicenses.GetItemsCount(); i++) {
                                    if (activeLicenses.GetData("Id", i) === licenseId) {
                                       activeLicenses.RemoveItem(i);
                                        break;
                                    }
                                 }
                             }
                         }

                         onCheckBoxExpirationClicked: {
                             var licensesLocal = licenses.GetData("data");
                             if (state == 0) {
                                 var licenseId = licensesLocal.GetData("Id", modelIndex);

                                 for (var i = 0; i < activeLicenses.GetItemsCount(); i++) {
                                     if (activeLicenses.GetData("Id", i) === licenseId) {
                                         activeLicenses.SetData("Expiration", "unlimited", i);
                                         break;
                                     }
                                 }
                             }
                         }

                         onExpirationTextChanged: {
                             var licensesLocal = licenses.GetData("data");
                             var licenseId = licensesLocal.GetData("Id", modelIndex);

                             for (var i = 0; i < activeLicenses.GetItemsCount(); i++) {
                                 if (activeLicenses.GetData("Id", i) === licenseId) {
                                     activeLicenses.SetData("Expiration", value, i);
                                     break;
                                 }
                             }
                         }
                     }
                 }

                 TreeItemModel {
                     id: headersModelLicenses;

                     Component.onCompleted: {
                         headersModelLicenses.SetData("Name", "License", 0)
                         headersModelLicenses.InsertNewItem()
                         headersModelLicenses.SetData("Name", "Expiration", 1)
                         headersModelLicenses.Refresh()
                     }
                 }
             }
        }
    }

    ListModel {
        id: listModelProducts;
    }

    ListModel {
        id: listModelAccounts;
    }

    GqlModel {
        id: installItemModel;

        function updateModel() {
            console.log( "updateModel InstallationItem");

            var query = Gql.GqlRequest("query", "InstallationItem");

            if (containerInstallation.typeOperation !== "New"){
                var inputParams = Gql.GqlObject("input");
                inputParams.InsertField("Id");
                inputParams.InsertFieldArgument("Id", containerInstallation.itemId);
                query.AddParam(inputParams);
            }

            var queryFields = Gql.GqlObject("item");

            queryFields.InsertField("Id");
            queryFields.InsertField("AccountId");
            queryFields.InsertField("ProductId");
            queryFields.InsertField("Name");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("InstallationItem query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, installItemModel);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (installItemModel.ContainsKey("errors")){
                    return;
                }

                dataModelLocal = installItemModel.GetData("data");
                if(dataModelLocal.ContainsKey("InstallationItem")){
                    dataModelLocal = dataModelLocal.GetData("InstallationItem");
                    if(dataModelLocal.ContainsKey("item")){
                        //containerContactInfo.accountType = dataModelLocal.GetData("item").GetData("AccountType");

                        dataModelLocal = dataModelLocal.GetData("item");

//                        var installId = dataModelLocal.GetData("Id");
//                        var accountId = dataModelLocal.GetData("AccountId");
//                        var productId = dataModelLocal.GetData("ProductId");

                        containerInstallation.installationInfoModel = dataModelLocal;
                        containerInstallation.model.SetExternTreeModel('data', containerInstallation.installationInfoModel)
                    }
                }
            }
        }
    }

    GqlModel {
        id: headerInfoModel;

        function updateModel() {
            console.log( "InstallationInfoEditor load headers", containerInstallation.gqlModelInfo);

            var query = Gql.GqlRequest("query", containerInstallation.gqlModelInfo);
            var queryFields = Gql.GqlObject("headers");
            queryFields.InsertField("Id");
            queryFields.InsertField("Name");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("InstallationInfoEditor headers query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, containerInstallation.gqlModelInfo);
            if (this.state === "Ready"){
                var dataModelLocal = this.GetData("data");
                if(dataModelLocal && dataModelLocal.ContainsKey(containerInstallation.gqlModelInfo)){
                    dataModelLocal = dataModelLocal.GetData(containerInstallation.gqlModelInfo)
                    if(dataModelLocal.ContainsKey("headers")){
                        dataModelLocal = dataModelLocal.GetData("headers")

                        if (gqlModelInfo == "ProductInfo") {
                            products.SetExternTreeModel("headers", dataModelLocal);
                        }
                        else if (gqlModelInfo == "AccountInfo") {
                            accounts.SetExternTreeModel("headers", dataModelLocal);
                        }


                        itemsModel.updateModel();
                    }
                    else if(headerInfoModel.ContainsKey("errors")){
                        var errorsModelLocal = packageInfoModel.GetData("errors");
                        if(errorsModelLocal !== null && errorsModelLocal.ContainsKey(containerInstallation.gqlModelInfo)){
                            console.log("message", errorsModelLocal.GetData("ProductInfo").GetData("message"));
                        }
                    }
                }
            }
        }
    }

    GqlModel {
        id: itemsModel;

        function updateModel() {
            console.log( "InstallationInfoEditor load items", containerInstallation.gqlModelCollectionInfo);
            var query = Gql.GqlRequest("query", containerInstallation.gqlModelCollectionInfo);

            if(containerInstallation.itemId !== ""){
                var inputParams = Gql.GqlObject("input");
                inputParams.InsertField("Id");
                inputParams.InsertFieldArgument("Id", containerInstallation.itemId);
                query.AddParam(inputParams);
            }

            var queryFields = Gql.GqlObject("items");

            queryFields.InsertField("Id");
            var dataModelLocal;

            if (containerInstallation.gqlModelCollectionInfo == "ProductList") {
                dataModelLocal = products.GetData("headers");
            }
            else if (containerInstallation.gqlModelCollectionInfo == "AccountList") {
                dataModelLocal = accounts.GetData("headers");
            }

            for(var i = 0; i < dataModelLocal.GetItemsCount(); i++){
                queryFields.InsertField(dataModelLocal.GetData("Id",i));
            }
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("InstallationInfoEditor items query  ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, containerInstallation.gqlModelCollectionInfo);
            if (this.state === "Ready"){
                var dataModelLocal = this.GetData("data");
                if(dataModelLocal.ContainsKey(containerInstallation.gqlModelCollectionInfo)){
                    dataModelLocal = dataModelLocal.GetData(containerInstallation.gqlModelCollectionInfo);

                    if(dataModelLocal.ContainsKey("items")){
                        dataModelLocal = dataModelLocal.GetData("items");

                        if (containerInstallation.gqlModelCollectionInfo === "ProductList") {
                            products.SetExternTreeModel('data', dataModelLocal);

                            for (var i = 0; i < dataModelLocal.GetItemsCount(); i++) {
                                listModelProducts.append({"id": dataModelLocal.GetData("Id", i), "text": dataModelLocal.GetData("Name", i)});
                            }

                            containerInstallation.gqlModelCollectionInfo = "AccountList";
                            containerInstallation.gqlModelInfo = "AccountInfo";
                            headerInfoModel.updateModel();
                        }
                        else if (containerInstallation.gqlModelCollectionInfo === "AccountList") {
                            accounts.SetExternTreeModel('data', dataModelLocal);

                            for (var i = 0; i < dataModelLocal.GetItemsCount(); i++) {
                                listModelAccounts.append({"id": dataModelLocal.GetData("Id", i), "text": dataModelLocal.GetData("AccountName", i)});
                            }

                            installItemModel.updateModel();
                        }
                    }
                    else if(itemsModel.ContainsKey("errors")){
                        var errorsModel = itemsModel.GetData("errors");
                        if(errorsModel !== null && errorsModel.ContainsKey(containerInstallation.gqlModelCollectionInfo)){
                            console.log("message", errorsModel.GetData(containerInstallation.gqlModelCollectionInfo).GetData("message"));
                        }
                    }
                }
            }
        }
    }

    GqlModel {
        id: licensesModel;

        function updateModel(productId) {
            console.log( "InstallationInfoEditor load licenses");
            var query = Gql.GqlRequest("query", "LicenseList");

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id");
            inputParams.InsertFieldArgument("Id", productId);
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("items");

            queryFields.InsertField("Id");
            var dataModelLocal = products.GetData("headers");

            for(var i = 0; i < dataModelLocal.GetItemsCount(); i++) {
                queryFields.InsertField(dataModelLocal.GetData("Id",i));
            }

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("InstallationInfoEditor licenseModel query  ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, "LicenseList");
            if (this.state === "Ready") {
                var dataModelLocal = this.GetData("data");
                if(dataModelLocal.ContainsKey("LicenseList")){
                    dataModelLocal = dataModelLocal.GetData("LicenseList");
                    if(dataModelLocal !== null && dataModelLocal.ContainsKey("items")){
                        dataModelLocal = dataModelLocal.GetData("items");
                        licenses.SetExternTreeModel("data", dataModelLocal);

                        console.log("InstallationInfoEditor GqlModel licensesModel");
                        licensesTable.elements = dataModelLocal;
                    }
                    else if(licensesModel.ContainsKey("errors")){
                        var errorsModel = itemsModel.GetData("errors");
                        if(errorsModel.ContainsKey("LicenseList")){
                            console.log("message", errorsModel.GetData("LicenseList").GetData("message"));
                        }
                    }
                }
            }
        }
    }

    GqlModel {
        id: saveModel;

        function updateModel(newName) {
            console.log( "InstallationInfoEditor saveModel updateModel");

            var query;
            var queryFields;
            var inputParams = Gql.GqlObject("input");

            if (containerInstallation.typeOperation == "New") {
                query = Gql.GqlRequest("query", "InstallationAdd");
                queryFields = Gql.GqlObject("addedNotification");

                containerInstallation.itemId = instanceIdText.text;
                containerInstallation.itemName = newName;
            }
            else {
                query = Gql.GqlRequest("query", "InstallationUpdate");
                inputParams.InsertField("Id");
                inputParams.InsertFieldArgument("Id", containerInstallation.itemId);
                queryFields = Gql.GqlObject("updatedNotification");
            }

            query.AddParam(inputParams);

            installationModel.SetData("Id", containerInstallation.itemId)
            installationModel.SetData("Name", containerInstallation.itemName)

            installationModel.SetData("AccountId", containerInstallation.accountId);
            installationModel.SetData("ProductId", containerInstallation.productId);

           // installationModel.SetData("ActiveLicenses", containerInstallation.productId);

           // var licensesModelLocal = licenses.GetData("data");
            installationModel.SetExternTreeModel("ActiveLicenses", activeLicenses);

            var jsonString = installationModel.toJSON();
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")

            inputParams.InsertField("Item");
            inputParams.InsertFieldArgument ("Item", jsonString);

            queryFields.InsertField("Id");
            queryFields.InsertField("Successed");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("InstallationInfoEditot save model query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, saveModel);
            if (this.state === "Ready"){
                containerInstallation.multiDocViewItem.activeCollectionItem.refresh();
                var dataModelLocal = saveModel.GetData("data");
                if(dataModelLocal.ContainsKey("addedNotification")){
                    dataModelLocal = dataModelLocal.GetData("addedNotification");

                    if(dataModelLocal.ContainsKey("Id") && containerInstallation.typeOperation === "New"){
                        containerInstallation.itemId = dataModelLocal.GetData("Id");
                    }
                    else if(saveModel.ContainsKey("errors")){
                        var errorsModel = accountItemModel.GetData("errors");
                        if(errorsModel !== null && errorsModel.ContainsKey(containerContactInfo.gqlModelItems)){
                            console.log("message", errorsModel.GetData(containerInstallation.gqlModelItems).GetData("message"));
                        }
                    }
                }
            }
        }
    }
}
