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

//    property TreeItemModel installationInfoModel;
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
        id: installationInfoModel;
    }

    TreeItemModel {
        id: products;
    }

    TreeItemModel {
        id: accounts;
    }

    TreeItemModel {
        id: modelInstallations;
    }

    Component.onCompleted: {
        containerInstallation.setDefaultGui();
        containerInstallation.gqlModelInfo = "ProductInfo";
        containerInstallation.gqlModelCollectionInfo = "ProductList"
        headerInfoModel.updateModel();
    }

    onActiveLicensesChanged: {
        console.log("InstallationInfoEditor onActiveLicensesChanged");
        if (!installationInfoModel || !containerInstallation.activeLicenses){
            return;
        }
        containerInstallation.updateActiveLicenses();
    }

//    onInstallationInfoModelChanged: {
//        console.log("InstallationInfoEditor onInstallationInfoModelChanged");
//        if (containerInstallation.operation !== "New"){
//            containerInstallation.updateGui();
//        }
//    }

    Timer {
        id: saveTimer;

        interval: 500;

        onTriggered: {
            console.log("saveTimer onTriggered");
            undoRedo.addModel(installationInfoModel);
        }
    }

    UndoRedo {
        id: undoRedo;

        rootItem: containerInstallation.rootItem;
        multiDocViewItem: containerInstallation.multiDocViewItem;
    }

    ListModel {
        id: listModelProducts;
    }

    ListModel {
        id: listModelAccounts;
    }

    function updateActiveLicenses(){

        if (!containerInstallation.activeLicenses){
            return;
        }

        containerInstallation.setDefaultActiveLicensesGui();

        var licenses = installationInfoModel.GetData("ActiveLicenses");

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

                        if (expiration !== "Unlimited"){
                            containerInstallation.activeLicenses.SetData("ExpirationState", 2, i);
                            containerInstallation.activeLicenses.SetData("Expiration", expiration, i);
                        }
                    }
                }
            }
        }
    }

    function setDefaultActiveLicensesGui(){

        if (!containerInstallation.activeLicenses){
            return;
        }

        for (let i = 0; i < containerInstallation.activeLicenses.GetItemsCount(); i++){
            containerInstallation.activeLicenses.SetData("LicenseState", 0, i);
            containerInstallation.activeLicenses.SetData("ExpirationState", 0, i);
            containerInstallation.activeLicenses.SetData("Expiration", "Unlimited", i);
        }
    }

    function setDefaultGui(){
        installationInfoModel.SetData("Id", "");
        installationInfoModel.SetData("Name", "");
        installationInfoModel.SetData("ProductId", "");
        installationInfoModel.SetData("AccountId", "");
        let activeLicenses = installationInfoModel.AddTreeModel("ActiveLicenses");
    }

    function updateGui() {
        console.log("containerContactInfo updateGui");

        instanceIdText.text = installationInfoModel.GetData("Id");

        var accountId = installationInfoModel.GetData("AccountId");
        var producId = installationInfoModel.GetData("ProductId");

        var accountIndex = -1;
        var accountsData = accounts.GetData("data");
        for (var i = 0; i < accountsData.GetItemsCount(); i++){

            var accId = accountsData.GetData("Id", i);
            console.log("accId", accId);
            if (accId === accountId){
                accountIndex = i;
            }
        }

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

        containerInstallation.updateActiveLicenses();
    }

    function dialogResult(parameters) {
         console.log("InstallationInfoEditor dialogResult", parameters["status"]);


        if (parameters["dialog"] === "InputDialog"){
            if (parameters["status"] === "ok"){
                var value = parameters["value"];
                console.log("featureCollectionViewContainer dialogResult", value);
                installationInfoModel.SetData("Name", value);
                installationSaveQuery.updateModel(value);
            }
        }
        else if (parameters["dialog"] === "ErrorDialog"){

        }
        else if (parameters["dialog"] == "SaveDialog"){

            if (parameters["status"] == "yes"){
                containerInstallation.menuActivated("Save")
            }
            else if (parameters["status"] == "no"){
                containerInstallation.rootItem.closeTab();
            }
        }
    }

    function menuActivated(menuId) {
        console.log("InstallationInfoEditor menuActivated", menuId);
        if (menuId  === "Save") {

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

            if (containerInstallation.wasChanged){
                let source = "AuxComponents/MessageDialog.qml";
                let parameters = {};
                parameters["message"] = qsTr("Save all changes ?");
                parameters["nameDialog"] = "Save";
                parameters["dialogId"] = "SaveDialog";
                parameters["cancelButtonVisible"] = true;
                parameters["resultItem"] = containerInstallation;

                thubnailDecoratorContainer.openDialog(source, parameters);
            }
            else{
                containerInstallation.rootItem.closeTab();
            }
        }
        else if (menuId  === "Undo"){
            var result = undoRedo.undo();
            if (result != null){
                containerInstallation.parseData(result);
            }
        }
        else if (menuId  === "Redo"){
            if (!undoRedo.redoStackIsEmpty()){
                var result = undoRedo.redo();
                containerInstallation.parseData(result);
            }
        }
    }

    function parseData(json){
        json = json.replace(/\\/g, '');
        json = json.slice(1, json.length - 1);

        installationInfoModel.Parse(json);
        containerInstallation.updateGui();
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

        if (containerInstallation.wasChanged){
            containerInstallation.rootItem.setModeMenuButton("Save", "Normal");
        }
    }

    function addLicenseToActiveLicenses(itemId){
        let modelLicenses = installationInfoModel.GetData("ActiveLicenses");
        let curProductId = installationInfoModel.GetData("ProductId");
        let index = modelLicenses.InsertNewItem();
        modelLicenses.SetData("Id", itemId, index);
        modelLicenses.SetData("Expiration", "Unlimited", index);
//        modelLicenses.SetData("ProductId", containerInstallation.productId, index);
        modelLicenses.SetData("ProductId", curProductId, index);
        containerInstallation.updateGui();
    }

    function removeLicenseToActiveLicenses(itemId){
        let modelLicenses = installationInfoModel.GetData("ActiveLicenses");

        for (let i = 0; i < modelLicenses.GetItemsCount(); i++){
            let curItemId = modelLicenses.GetData("Id", i);
            if (curItemId == itemId){
                modelLicenses.RemoveItem(i);
                break;
            }
        }

        containerInstallation.updateGui();
    }

    function setDataToActiveLicenses(itemId, fieldId, value){
        console.log("containerInstallation setDataToActiveLicenses", itemId, fieldId, value);
        let modelLicenses = installationInfoModel.GetData("ActiveLicenses");

        let itemFind = false;
        for (let i = 0; i < modelLicenses.GetItemsCount(); i++){
            let curItemId = modelLicenses.GetData("Id", i);
            if (curItemId == itemId){
                itemFind = true;

                console.log("SetData fieldId value i", fieldId, value, i);
                modelLicenses.SetData(fieldId, value, i);
                break;
            }
        }

//        installationInfoModel.Refresh();

        containerInstallation.updateGui();
    }

    Flickable {
        anchors.fill: parent;

        anchors.leftMargin: 20;

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

                 RegExpValidator {
                     id: regexValid;

                     Component.onCompleted: {
                         console.log("RegExpValidator onCompleted");
                         let regex = preferenceDialog.getInstanceMask();
                         if (regex != ""){
                             let re = new RegExp(regex);
                             if (re){
                                 regexValid.regExp = re;
                             }
                         }
                     }
                 }

                 TextFieldCustom {
                     id: instanceIdText;

                     anchors.horizontalCenter: tfcInstance.horizontalCenter;
                     anchors.verticalCenter: tfcInstance.verticalCenter;

                     width: tfcInstance.width - 22;
                     height: 30;

                     placeHolderText: qsTr("Enter the instance ID");

                     Component.onCompleted: {
                         console.log("instanceIdText onCompleted", regexValid);
                         instanceIdText.setValidator(regexValid);
                     }

                     onInputTextChanged: {
                         installationInfoModel.SetData("Id", instanceIdText.text);

                         let splitData = containerInstallation.itemId.split("$IMTCORESEPARATOR$");
                         let value = splitData[0];

                         if (instanceIdText.text != value){
                             saveTimer.restart();
                         }
                     }
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

                     textCentered: false;

//                     property bool wasFocus: false;

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

                         let curAccountId = installationInfoModel.GetData("AccountId");
                         let selectedAccountId = listModelAccounts.get(customerCB.currentIndex).id;
                         installationInfoModel.SetData("AccountId", selectedAccountId);
                         if (curAccountId != selectedAccountId){
                             undoRedo.addModel(installationInfoModel);
                         }

                         containerInstallation.accountId = selectedAccountId;
                         customerCB.currentText = listModelAccounts.get(customerCB.currentIndex).name;
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

                         if (productCB.currentIndex < 0){
                             return;
                         }

                         let selectedProductId = listModelProducts.get(productCB.currentIndex).id;

                         let curProductId = installationInfoModel.GetData("ProductId");
                         installationInfoModel.SetData("ProductId", selectedProductId);

                         if (containerInstallation.operation != "New"){

                             if (curProductId != selectedProductId){
                                 undoRedo.addModel(installationInfoModel);
                             }
                         }
                         licensesModel.updateModel(selectedProductId);

                         productCB.currentText = listModelProducts.get(productCB.currentIndex).name;
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

                         property bool flag: true;

                         onClicked: {
                             licensesTable.selectedIndex = model.index;
                         }

                         onCheckBoxLicenseClicked: {
                             console.log("InstallationInfoEditor AuxTable onCheckBoxLicenseClicked", itemId, modelIndex, state);

                             if (state == 2){
                                 containerInstallation.addLicenseToActiveLicenses(itemId);
                             }
                             else if (state == 0){
                                 containerInstallation.removeLicenseToActiveLicenses(itemId);
                             }

                             undoRedo.addModel(installationInfoModel);
                         }

                         onCheckBoxExpirationClicked: {
                             console.log("InstallationInfoEditor AuxTable onCheckBoxExpirationClicked", itemId, modelIndex, state);

                             if (state == 0){
                                 containerInstallation.setDataToActiveLicenses(itemId, "Expiration", "Unlimited");
                             }
                             else{
                                 containerInstallation.setDataToActiveLicenses(itemId, "Expiration", "01.01.2023");
                             }

                             undoRedo.addModel(installationInfoModel);
                         }

                         onExpirationTextChanged: {
                             console.log("InstallationInfoEditor AuxTable onExpirationTextChanged", modelIndex, value);
                             let modelLicenses = installationInfoModel.GetData("ActiveLicenses");
                             let state = modelLicenses.GetData("Expiration", modelIndex);

                             if (state && state != value){
                                containerInstallation.rootItem.setModeMenuButton("Save", "Normal");
                             }

                             containerInstallation.setDataToActiveLicenses(itemId, "Expiration", value);
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

    LoadingPage {
        id: loadingPage;

        anchors.fill: parent;

        visible: false;
    }

    GqlModel {
        id: installItemModel;

        function updateModel() {
            console.log( "updateModel InstallationItem");

            var query = Gql.GqlRequest("query", "InstallationItem");

            if (containerInstallation.operation !== "New"){
                var inputParams = Gql.GqlObject("input");
                inputParams.InsertField("Id", containerInstallation.itemId);
                query.AddParam(inputParams);
            }

            var queryFields = Gql.GqlObject("item");

            queryFields.InsertField("Id");
            queryFields.InsertField("AccountId");
            queryFields.InsertField("ProductId");
            queryFields.InsertField("Name");

            query.AddField(queryFields);

            loadingPage.visible = true;
            var gqlData = query.GetQuery();
            console.log("InstallationItem query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, installItemModel);
            if (this.state === "Ready"){
                let keys = installItemModel.GetKeys();
                if (!keys || keys.length == 0){
                    loadingPage.visible = false;
                    thubnailDecoratorContainer.setInvalidConnection(true);
                    return;
                }
                var dataModelLocal;

                if (installItemModel.ContainsKey("errors")){
                    return;
                }

                dataModelLocal = installItemModel.GetData("data");
                if(dataModelLocal.ContainsKey("InstallationItem")){
                    dataModelLocal = dataModelLocal.GetData("InstallationItem");

                    if (!dataModelLocal.ContainsKey("item")){
                        dataModelLocal.AddTreeModel("item");
                    }

                    if(dataModelLocal.ContainsKey("item")){
                        dataModelLocal = dataModelLocal.GetData("item");

                        if (!dataModelLocal.ContainsKey("ActiveLicenses")){
                            dataModelLocal.AddTreeModel("ActiveLicenses");
                        }

                        installationInfoModel.SetData("ActiveLicenses", dataModelLocal.GetData("ActiveLicenses"));

                        if (dataModelLocal.ContainsKey("ProductId")){
                            containerInstallation.productId = dataModelLocal.GetData("ProductId");
                            installationInfoModel.SetData("ProductId", dataModelLocal.GetData("ProductId"));
                        }

                        if (dataModelLocal.ContainsKey("AccountId")){
                            installationInfoModel.SetData("AccountId", dataModelLocal.GetData("AccountId"));
                        }

                        if (dataModelLocal.ContainsKey("Id")){
                            installationInfoModel.SetData("Id", dataModelLocal.GetData("Id"));
                        }

                        containerInstallation.updateGui();
                    }

                    undoRedo.addModel(installationInfoModel);

//                    containerInstallation.model.SetExternTreeModel('data', installationInfoModel)
                }
                loadingPage.visible = false;
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
                inputParams.InsertField("Id", containerInstallation.itemId);
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
                                listModelAccounts.append({"id": dataModelLocal.GetData("Id", i), "name": dataModelLocal.GetData("Name", i)});
                            }

//                            installItemModel.updateModel();
                            if (containerInstallation.operation != "New"){
                                installItemModel.updateModel();
                            }
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
            console.log( "InstallationInfoEditor load licenses", productId);
            var query = Gql.GqlRequest("query", "LicenseList");

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", productId);
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
//                        licenses.SetExternTreeModel("data", dataModelLocal);

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

                        if (containerInstallation.activeLicenses){
                            containerInstallation.activeLicenses.Clear();
                        }

                        containerInstallation.activeLicenses = dataModelLocal;
                        licensesTable.elements = containerInstallation.activeLicenses;
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
                inputParams.InsertField("Id", containerInstallation.itemId);
                inputParams.InsertField("ProductId", containerInstallation.productId);
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

//            let curProductId = installationInfoModel.GetData("ProductId");

//            modelInstallations.SetData("Id", instanceIdText.text)
//            modelInstallations.SetData("Name", containerInstallation.itemName)
//            modelInstallations.SetData("AccountId", containerInstallation.accountId);
////            modelInstallations.SetData("ProductId", containerInstallation.productId);
//            modelInstallations.SetData("ProductId", curProductId);

//            let licenses = installationInfoModel.GetData("ActiveLicenses");
//            modelInstallations.SetExternTreeModel("ActiveLicenses", licenses);

            var jsonString = installationInfoModel.toJSON();
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")

            inputParams.InsertField ("Item", jsonString);

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
                let keys = installationSaveQuery.GetKeys();
                if (!keys || keys.length == 0){
                    thubnailDecoratorContainer.setInvalidConnection(true);
                    return;
                }

                var dataModelLocal;

                if (installationSaveQuery.ContainsKey("errors")){

                    dataModelLocal = installationSaveQuery.GetData("errors");

                    dataModelLocal = dataModelLocal.GetData(containerInstallation.gqlModelQueryType);

                    if (dataModelLocal){
                        console.log("Message errors");
                        var messageError = dataModelLocal.GetData("message");
                        containerInstallation.openMessageDialog("Error", messageError, "ErrorDialog");
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

                    if(containerInstallation.operation == "New"){
                        containerInstallation.operation = "Open";
                    }

                    containerInstallation.multiDocViewItem.disableChanges();

                    if (containerInstallation.multiDocViewItem.activeCollectionItem){
                        containerInstallation.multiDocViewItem.activeCollectionItem.callMetaInfoQuery();
                    }
//                    console.log("metainfo updated")
                }
            }
        }
    }
}
