import QtQuick 2.0
import imtauthgui 1.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Rectangle {
    id: containerInstallation;

    width: 500;
    height: 500;

    color: Style.backgroundColor;

    property Item rootItem;
    property Item multiDocViewItem;

    property string itemId;
    property string itemName;
    property string accountId;
    property string productId;
    property string gqlModelInfo;
    property string gqlModelCollectionInfo;
    property string gqlModelQueryType;
    property string gqlModelQueryTypeNotification;

    property TreeItemModel installationInfoModel;
    property TreeItemModel model;
    property TreeItemModel activeLicenses;

    property string operation;

    property bool wasChanged: false;

    onFocusChanged: {
        console.log("InstallationInfoEditor onFocusChanged", containerInstallation.focus);
        if (containerInstallation.focus){
            tfcInstance.forceActiveFocus();
        }
    }

    TreeItemModel {
        id: products;
    }

    TreeItemModel {
        id: licenses;
    }

    TreeItemModel {
        id: accounts;
    }

    TreeItemModel {
        id: modelInstallations;
    }

    Component.onCompleted: {
        containerInstallation.gqlModelInfo = "ProductInfo";
        containerInstallation.gqlModelCollectionInfo = "ProductList"
        headerInfoModel.updateModel();
    }

    onWasChangedChanged: {
        console.log("InstallationInfoEditor onWasChangedChanged", containerInstallation.wasChanged);
        containerInstallation.commandsChanged("InstallationEdit");
    }

    onModelChanged: {
        console.log("InstallationInfoEditor onModelChanged", containerInstallation.operation);
        if (containerInstallation.model.ContainsKey("data")){
            containerInstallation.contactInfoModel = containerContactInfo.model.GetData('data');
            containerInstallation.updateData();
        }
    }

    onActiveLicensesChanged: {
        console.log("InstallationInfoEditor onActiveLicensesChanged");
        if (!containerInstallation.installationInfoModel){
            return;
        }

        var licenses = containerInstallation.installationInfoModel.GetData("ActiveLicenses");
        if (licenses && licenses.GetItemsCount() > 0){
            var index;
            var count =  containerInstallation.activeLicenses.GetItemsCount();
            for (var i = 0; i < count; i++){
                var curId = containerInstallation.activeLicenses.GetData("Id", i);

                for (var j = 0; j < licenses.GetItemsCount(); j++){
                    var licId = licenses.GetData("Id", j);
                    var expiration = licenses.GetData("Expiration", j);

                    if (curId === licId){
                        containerInstallation.activeLicenses.SetData("LicenseState", 2, i);

                        if (expiration && expiration !== "Unlimited"){
                            var parts = expiration.split('.');
                            var date = new Date(parts[2], parts[1] - 1, parts[0]);
                            containerInstallation.activeLicenses.SetData("ExpirationState", 2, i);
                            containerInstallation.activeLicenses.SetData("Expiration", date, i);
                        }
                    }
                }
            }
        }
        licensesTable.elements = containerInstallation.activeLicenses;
    }

    onInstallationInfoModelChanged: {
        console.log("InstallationInfoEditor onInstallationInfoModelChanged");
        if (containerInstallation.operation !== "New"){
            containerInstallation.updateData();
        }
    }

    ListModel {
        id: listModelProducts;
    }

    ListModel {
        id: listModelAccounts;
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
            console.log("accId", accId);
            if (accId === accountId){
                accountIndex = i;
            }
        }
        console.log("accountId", accountId);
        console.log("Account index = ", accountIndex);
        if (accountIndex !== -1){
            customerCB.currentIndex = accountIndex;
        }

        var productIndex = -1;
        var productsData = products.GetData("data");
        for (i = 0; i < productsData.GetItemsCount(); i++){

            var prId = productsData.GetData("Id", i);

            if (prId === producId){
                productIndex = i;
            }
        }

        if (productIndex !== -1){
            productCB.currentIndex = productIndex;
        }
    }

    function dialogResult(parameters) {
         console.log("InstallationInfoEditor dialogResult", parameters["status"]);


        if (parameters["dialog"] === "InputDialog"){
            if (parameters["status"] === "ok"){
                var value = parameters["value"];
                console.log("featureCollectionViewContainer dialogResult", value);
                installationSaveQuery.updateModel(value);
            }
        }
        else if (parameters["dialog"] === "ErrorDialog"){

        }
    }

    function menuActivated(menuId) {
        console.log("InstallationInfoEditor menuActivated", menuId);
        if (menuId  === "New"){
//            collectionViewContainer.selectItem("", "")
        }
        else if (menuId  === "Save") {

            if (instanceIdText.text == ""){
                containerInstallation.openMessageDialog("Error Dialog", "Id can't be empty!", "ErrorDialog");
                return;
            }

            if (containerInstallation.operation === "New") {
                var source = "AuxComponents/InputDialog.qml";
                var parameters = {};
                parameters["message"] = "Please enter the name of the document: ";
                parameters["nameDialog"] = "Document Name";
                parameters["resultItem"] = containerInstallation;
                thubnailDecoratorContainer.openDialog(source, parameters);
            }
            else{
                installationSaveQuery.updateModel();
            }
        }
        else if (menuId === "Close") {
            containerInstallation.rootItem.closeTab();
        }
    }

    function openMessageDialog(nameDialog, message, type) {
        var source = "AuxComponents/MessageDialog.qml";
        var parameters = {};
        parameters["resultItem"] = containerInstallation;
        parameters["noButtonVisible"] = false;
        parameters["textOkButton"] = "Ok";
        parameters["message"] = message;
        parameters["nameDialog"] = nameDialog;
        parameters["dialogId"] = type;
        thubnailDecoratorContainer.openDialog(source, parameters);
    }

    function refresh(){}

    function commandsChanged(commandsId){
        if (commandsId !== "InstallationEdit") {
            return;
        }

        containerInstallation.rootItem.setModeMenuButton("New", "Normal");
        containerInstallation.rootItem.setModeMenuButton("Close", "Normal");

        containerInstallation.rootItem.setModeMenuButton("Save", "Normal");

//        if (containerInstallation.wasChanged){
//            containerInstallation.rootItem.setModeMenuButton("Save", "Normal");
//        }
//        else {
//            containerInstallation.rootItem.setModeMenuButton("Save", "Disabled");
//        }
    }

    Flickable {
        anchors.fill: parent;

        contentWidth: containerColumn.width;
        contentHeight: containerColumn.height + 50;
        boundsBehavior: Flickable.StopAtBounds;

        clip: true;

        Column {
             id: containerColumn;

             width: 500;

             spacing: 7;

             Text {
                 id: titleInstance;

                 anchors.left: containerColumn.left;
                 anchors.leftMargin: 10;

                 text: qsTr("Instance-ID");
                 color: Style.textColor;
                 font.family: Style.fontFamily;
                 font.pixelSize: Style.fontSize_common;
             }

             Rectangle {
                 id: tfcInstance;

                 anchors.horizontalCenter: containerColumn.horizontalCenter;

                 width: containerColumn.width - 20;
                 height: 45;

                 color: Style.imagingToolsGradient1;

                 border.width: 1;
                 border.color: Style.borderColor;

                 TextFieldCustom {
                     id: instanceIdText;

                     anchors.horizontalCenter: tfcInstance.horizontalCenter;
                     anchors.verticalCenter: tfcInstance.verticalCenter;

                     width: tfcInstance.width - 22;
                     height: 30;

                     onInputTextChanged: {
                         containerInstallation.wasChanged = true;
                     }

//                     onFocusChanged: {
//                         if (instanceIdText.focus){
//                             instanceIdText.setFocus(true);
//                         }
//                     }
                 }

                 onFocusChanged: {
                     if (tfcInstance.focus){
                         instanceIdText.setFocus(true);
                     }
                 }

                 KeyNavigation.tab: customerCB;
             }

             Text {
                 id: titleCustomer;

                 anchors.left: containerColumn.left;
                 anchors.leftMargin: 10;

                 text: qsTr("Customer");
                 color: Style.textColor;

                 font.family: Style.fontFamily;
                 font.pixelSize: Style.fontSize_common;
             }

             Rectangle {
                 id: customerBlock;
                 z: 10;

                 anchors.horizontalCenter: containerColumn.horizontalCenter;

                 height: 45;
                 width: containerColumn.width - 20;

                 color: Style.imagingToolsGradient1;

                 border.width: 1;
                 border.color: Style.borderColor;

                 ComboBox {
                     id: customerCB;

                     anchors.horizontalCenter: customerBlock.horizontalCenter;
                     anchors.verticalCenter: customerBlock.verticalCenter;

                     width: customerBlock.width - 22;
                     height: 23;

                     radius: 3;
                     model: listModelAccounts;

//                     backgroundColor: "#d0d0d0";
//                     borderColor: Style.alternateBaseColor;
//                     borderColor: customerCB.focus ? Style.iconColorOnSelected :
//                                                                     Style.alternateBaseColor;

                     textCentered: false;

                     property bool wasFocus: false;

                     Keys.onPressed: {
                         console.log("customerCB keys pressed")
                         if (event.key === Qt.Key_Space){
                             console.log('Key space was pressed');
                             customerCB.clicked();
                         }
//                         else if (event.key === Qt.Key_Tab){
//                             console.log('Key tab was pressed')
//                             productCB.forceActiveFocus();
//                         }
                     }

                     KeyNavigation.tab: productCB;
                     KeyNavigation.backtab: tfcInstance;

                     onCurrentIndexChanged: {
                         console.log("InstallationInfoEditor customerCB onCurrentIndexChanged");
                         containerInstallation.accountId = listModelAccounts.get(customerCB.currentIndex).id;
                         customerCB.currentText = listModelAccounts.get(customerCB.currentIndex).name;

                         if (containerInstallation.operation !== "New" && !customerCB.wasFocus){
                             customerCB.wasFocus = true;
                             return;
                         }

                         containerInstallation.wasChanged = true;
                     }

                     onClicked: {
                         customerCB.openContextMenu();
                     }

                     onDialogResultChanged: {
                         console.log("ComboBox onDialogResultChanged");
//                         settingsComboBoxContainer.rootItem.forceActiveFocus();
                         customerCB.forceActiveFocus();
                     }
                 }
             }

             Text {
                 id: titleProduct;
                 z : 5;

                 anchors.left: containerColumn.left;
                 anchors.leftMargin: 10;

                 text: qsTr("Product");
                 color: Style.textColor;
                 font.family: Style.fontFamily;
                 font.pixelSize: Style.fontSize_common;
             }

             Rectangle {
                 id: productBlock;
                 z : 5;

                 anchors.horizontalCenter: containerColumn.horizontalCenter;

                 height: 45;
                 width: containerColumn.width - 20;

                 color: Style.imagingToolsGradient1;

                 border.width: 1;
                 border.color: Style.borderColor;

                 ComboBox {
                     id: productCB;

                     anchors.horizontalCenter: productBlock.horizontalCenter;
                     anchors.verticalCenter: productBlock.verticalCenter;

                     width: productBlock.width - 22;
                     height: 23;

                     radius: 3;
                     model: listModelProducts;
                     textCentered: false;

                     property bool wasFocus: false;

                     Keys.onPressed: {
                         console.log("productCB keys pressed")
                         if (event.key === Qt.Key_Space){
                             console.log('Key space was pressed');
                             productCB.clicked();
                         }
                     }

                     KeyNavigation.tab: tfcInstance;
                     KeyNavigation.backtab: customerCB;

                     onCurrentIndexChanged: {
                         console.log("InstallationInfoEditor productCB onCurrentIndexChanged");
                         containerInstallation.productId = listModelProducts.get(productCB.currentIndex).id;
                         productCB.currentText = listModelProducts.get(productCB.currentIndex).name;

                         licensesModel.updateModel(containerInstallation.productId);

                         if (containerInstallation.operation !== "New" && !productCB.wasFocus){
                             productCB.wasFocus = true;
                             return;
                         }

                         containerInstallation.wasChanged = true;
                     }

                     onClicked: {
                         productCB.openContextMenu();
                     }

                     onDialogResultChanged: {
                         console.log("ComboBox onDialogResultChanged");
                         productCB.forceActiveFocus();
                     }
                 }
             }

             Text {
                 id: titleLicenses;

                 anchors.left: containerColumn.left;
                 anchors.leftMargin: 10;

                 text: qsTr("Licenses");
                 color: Style.textColor;
                 font.family: Style.fontFamily;
                 font.pixelSize: Style.fontSize_common;
             }

             Rectangle {
                 id: licensesBlock;

                 anchors.horizontalCenter: containerColumn.horizontalCenter;
                 //anchors.bottom: containerInstallation.bottom;

                 width: containerColumn.width - 20;
                 height: 300;

                 color: Style.imagingToolsGradient1;

                 border.width: 1;
                 border.color: Style.borderColor;

                 AuxTable {
                     id: licensesTable;

                     anchors.top: licensesBlock.top;
                     anchors.topMargin: 10;
                     anchors.bottom: parent.bottom;
                     anchors.bottomMargin: 10;
                     anchors.left: parent.left;
                     anchors.leftMargin: 10;
                     anchors.right: parent.right;
                     anchors.rightMargin: 10;

                     headers: headersModelLicenses;

                     hasFilter: false;

                     delegate: TableInstanceLicensesDelegate {
                         id: delegate;

                         width: licensesTable.width;
                         height: 35;

                         selected: licensesTable.selectedIndex === model.index;

                         onClicked: {
                             licensesTable.selectedIndex = model.index;
                         }

                         onCheckBoxLicenseClicked: {
                             console.log("InstallationInfoEditor AuxTable onCheckBoxLicenseClicked", modelIndex, state);
                             if (modelIndex >= 0){
                                 containerInstallation.activeLicenses.SetData("LicenseState", state, modelIndex);
                             }

                             containerInstallation.wasChanged = true;
                         }

                         onCheckBoxExpirationClicked: {
                             console.log("InstallationInfoEditor AuxTable onCheckBoxExpirationClicked", modelIndex, state);
                             if (modelIndex >= 0){
                                 containerInstallation.activeLicenses.SetData("ExpirationState", state, modelIndex);
                             }

                             if (state == 0){
                                 containerInstallation.activeLicenses.SetData("Expiration", "Unlimited", modelIndex);
                             }
                             else{
                                 containerInstallation.activeLicenses.SetData("Expiration", "01.01.2023", modelIndex);
                             }

                             containerInstallation.wasChanged = true;
                         }

                         onExpirationTextChanged: {
                             console.log("InstallationInfoEditor AuxTable onExpirationTextChanged", modelIndex, value);

                             if (modelIndex >= 0){
                                 containerInstallation.activeLicenses.SetData("Expiration", value, modelIndex);
                             }

                             containerInstallation.wasChanged = true;
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

    GqlModel {
        id: installItemModel;

        function updateModel() {
            console.log( "updateModel InstallationItem");

            var query = Gql.GqlRequest("query", "InstallationItem");

            if (containerInstallation.operation !== "New"){
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

                    if (containerInstallation.operation === "New"){
                        containerInstallation.installationInfoModel = dataModelLocal;
                    }

                    if(dataModelLocal.ContainsKey("item")){
                        dataModelLocal = dataModelLocal.GetData("item");
                        containerInstallation.installationInfoModel = dataModelLocal;
                    }
                    containerInstallation.model.SetExternTreeModel('data', containerInstallation.installationInfoModel)
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

                        if (containerInstallation.gqlModelInfo == "ProductInfo") {
                            products.SetExternTreeModel("headers", dataModelLocal);
                        }
                        else if (containerInstallation.gqlModelInfo == "AccountInfo") {
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
                console.log("products.toJSON()", products.toJSON());
                dataModelLocal = products.GetData("headers");
            }
            else if (containerInstallation.gqlModelCollectionInfo == "AccountList") {
                dataModelLocal = accounts.GetData("headers");
            }

            if (dataModelLocal.ContainsKey("Headers")){
                dataModelLocal = dataModelLocal.GetData("Headers");
            }

            for(var i = 0; i < dataModelLocal.GetItemsCount(); i++){
                queryFields.InsertField(dataModelLocal.GetData("Id",i));
            }


            query.AddField(queryFields);

            console.log("query", query);

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
                                listModelProducts.append({"id": dataModelLocal.GetData("Id", i), "name": dataModelLocal.GetData("Name", i)});
                            }

                            containerInstallation.gqlModelCollectionInfo = "AccountList";
                            containerInstallation.gqlModelInfo = "AccountInfo";
                            headerInfoModel.updateModel();
                        }
                        else if (containerInstallation.gqlModelCollectionInfo === "AccountList") {
                            accounts.SetExternTreeModel('data', dataModelLocal);

                            for (var i = 0; i < dataModelLocal.GetItemsCount(); i++) {
                                listModelAccounts.append({"id": dataModelLocal.GetData("Id", i), "name": dataModelLocal.GetData("AccountName", i)});
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

            if (dataModelLocal.ContainsKey("Headers")){
                dataModelLocal = dataModelLocal.GetData("Headers");
            }

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

                    if(dataModelLocal.ContainsKey("items")){
                        dataModelLocal = dataModelLocal.GetData("items");
                        licenses.SetExternTreeModel("data", dataModelLocal);

                        console.log("InstallationInfoEditor GqlModel licensesModel");

                        if (dataModelLocal){
                            for (var i = 0; i < dataModelLocal.GetItemsCount(); i++){
                                var curName = dataModelLocal.GetData("Name", i);
                                var curId = dataModelLocal.GetData("Id", i);

                                var newName = curName + " (" + curId + ")";

                                dataModelLocal.SetData("ExpirationState", 0, i);
                                dataModelLocal.SetData("LicenseState", 0, i);

                                dataModelLocal.SetData("Expiration", "01.01.2023", i);
                                dataModelLocal.SetData("Name", newName, i);
                            }
                        }
                        containerInstallation.activeLicenses = dataModelLocal;

//                        licensesTable.elements = dataModelLocal;
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
        id: installationSaveQuery;

        function updateModel(newName) {
            console.log( "InstallationInfoEditor installationSaveQuery updateModel");

            var query;
            var queryFields;
            var inputParams = Gql.GqlObject("input");

            if (containerInstallation.operation == "Open"){
                containerInstallation.gqlModelQueryType = "InstallationUpdate";
                containerInstallation.gqlModelQueryTypeNotification = "updatedNotification";
                query = Gql.GqlRequest("query", "InstallationUpdate");
                inputParams.InsertField("Id");
                inputParams.InsertFieldArgument("Id", containerInstallation.itemId);
                queryFields = Gql.GqlObject("updatedNotification");
            }
            else{
                containerInstallation.gqlModelQueryType = "InstallationAdd";
                containerInstallation.gqlModelQueryTypeNotification = "addedNotification";
                query = Gql.GqlRequest("query", "InstallationAdd");
                queryFields = Gql.GqlObject("addedNotification");

                if (newName){
                    containerInstallation.itemName = newName;
                }
//                else{
//                    containerInstallation.itemName = newName;
//                }
            }

            query.AddParam(inputParams);

            modelInstallations.SetData("Id", instanceIdText.text)
            modelInstallations.SetData("Name", containerInstallation.itemName)
            modelInstallations.SetData("AccountId", containerInstallation.accountId);
            modelInstallations.SetData("ProductId", containerInstallation.productId);

            modelInstallations.SetExternTreeModel("ActiveLicenses", containerInstallation.activeLicenses);

            var jsonString = modelInstallations.toJSON();
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
            console.log("State:", this.state, installationSaveQuery);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (installationSaveQuery.ContainsKey("errors")){

                    dataModelLocal = installationSaveQuery.GetData("errors");

                    dataModelLocal = dataModelLocal.GetData(containerInstallation.gqlModelQueryType);

                    if (dataModelLocal){
                        console.log("Message errors");
                        var messageError = dataModelLocal.GetData("message");
                        containerInstallation.openMessageDialog("Error Dialog", messageError, "ErrorDialog");
                    }

                    return;
                }

                if (installationSaveQuery.ContainsKey("data")){
                    dataModelLocal = installationSaveQuery.GetData("data");

                    if (dataModelLocal.ContainsKey(containerInstallation.gqlModelQueryType)){

                        dataModelLocal = dataModelLocal.GetData(containerInstallation.gqlModelQueryType);

                        if (dataModelLocal.ContainsKey(containerInstallation.gqlModelQueryTypeNotification)){
                            dataModelLocal = dataModelLocal.GetData(containerInstallation.gqlModelQueryTypeNotification);

                            if (dataModelLocal.ContainsKey("Id")){
                                containerInstallation.itemId = dataModelLocal.GetData("Id");
                            }
                        }
                    }
                    containerInstallation.rootItem.updateTitleTab(containerInstallation.itemId, containerInstallation.itemName);

                    if (containerInstallation.multiDocViewItem.activeCollectionItem){
                        //containerInstallation.multiDocViewItem.activeCollectionItem.callMetaInfoQuery();
                    }


//                    containerInstallation.multiDocViewItem.activeCollectionItem.refresh();

                    containerInstallation.wasChanged = false;
                }
            }
        }
    }
}
