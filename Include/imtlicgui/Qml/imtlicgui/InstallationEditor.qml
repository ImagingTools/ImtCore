import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

DocumentBase {
    id: installationEditorContainer;

    commandsDelegateSourceComp: installationEditorCommandsDelegate;

    property TreeItemModel accountsModel: TreeItemModel {}
    property TreeItemModel productsModel: TreeItemModel {}

    property bool blockUpdatingModel: false;

    Component.onCompleted: {
        licensesProvider.updateModel();
    }

    Component {
        id: installationEditorCommandsDelegate;
        InstallationEditorCommandsDelegate {}
    }

    onDocumentModelChanged: {
        let activeLicensesModel = installationEditorContainer.documentModel.GetData("ActiveLicenses");
        if (!activeLicensesModel){
            activeLicensesModel = installationEditorContainer.documentModel.AddTreeModel("ActiveLicenses");
        }

        installationEditorContainer.updateGui();

        undoRedoManager.registerModel(installationEditorContainer.documentModel)
    }

    onAccountsModelChanged: {
        console.log("onAccountsModelChanged", installationEditorContainer.accountsModel);
        customerCB.model = installationEditorContainer.accountsModel;
    }

    onProductsModelChanged: {
        console.log("onProductsModelChanged", installationEditorContainer.productsModel);
        productCB.model = installationEditorContainer.productsModel;
    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: installationEditorContainer.commandsId;

        onModelStateChanged: {
            installationEditorContainer.updateGui();
        }
    }

    LicensesProvider {
        id: licensesProvider;
    }

    MouseArea {
        anchors.fill: parent;

        onClicked: {
            installationEditorContainer.forceActiveFocus();
        }
    }

    function updateGui(){
        console.log("Begin updateGui");
        installationEditorContainer.blockUpdatingModel = true;

        instanceIdInput.text = installationEditorContainer.documentModel.GetData("Id");

        let accountId = installationEditorContainer.documentModel.GetData("AccountId");
        let productId = installationEditorContainer.documentModel.GetData("ProductId");

        //        customerCB.currentText = "";
        let customerModel = customerCB.model;
        for (let i = 0; i < customerModel.GetItemsCount(); i++){
            let id = customerModel.GetData("Id", i);
            if (id === accountId){
                customerCB.currentIndex = i;
                break;
            }
        }

        //        productCB.currentText = "";
        let productModel = productCB.model;
        for (let i = 0; i < productModel.GetItemsCount(); i++){
            let id = productModel.GetData("Id", i);
            if (id === productId){
                productCB.currentIndex = i;
                break;
            }
        }

        licensesTable.rowModel.clear();

        let activeLicensesModel = installationEditorContainer.documentModel.GetData("ActiveLicenses");
        if (!activeLicensesModel){
            activeLicensesModel = installationEditorContainer.documentModel.AddTreeModel("ActiveLicenses");
        }

        let licensesModel;
        for (let i = 0; i < licensesProvider.model.GetItemsCount(); i++){
            let id = licensesProvider.model.GetData("Id", i);
            if (id === productId){
                let productLicensesModel = licensesProvider.model.GetData("Licenses", i);
                licensesModel = productLicensesModel;
            }
        }

        if (licensesModel){
            console.log("licensesModel", licensesModel.toJSON());
            console.log("activeLicensesModel", activeLicensesModel.toJSON());
            for (let i = 0; i < licensesModel.GetItemsCount(); i++){
                let licenseId = licensesModel.GetData("Id", i);
                let licenseName = licensesModel.GetData("Name", i);

                let row = {"Id": licenseId, "Name": licenseName, "LicenseState": Qt.Unchecked, "ExpirationState": Qt.Unchecked, "Expiration": ""};

                for (let j = 0; j < activeLicensesModel.GetItemsCount(); j++){
                    let activeLicenseId = activeLicensesModel.GetData("Id", j);
                    let expiration = activeLicensesModel.GetData("Expiration", j);
                    if (licenseId == activeLicenseId){
                        row["LicenseState"] = Qt.Checked;

                        if (expiration == ""){
                            row["ExpirationState"] = Qt.Unchecked;
                        }
                        else{
                            row["ExpirationState"] = Qt.Checked;
                            row["Expiration"] = expiration;
                        }
                    }
                }

                console.log("row addRow", JSON.stringify(row));
                licensesTable.addRow(row);
            }
        }

        installationEditorContainer.blockUpdatingModel = false;
        console.log("End updateGui");
    }

    function updateModel(){
        console.log("Begin updateModel");
        undoRedoManager.beginChanges();

        installationEditorContainer.documentModel.SetData("Id", instanceIdInput.text)

        let selectedProductId = productCB.model.GetData("Id", productCB.currentIndex);
        installationEditorContainer.documentModel.SetData("ProductId", selectedProductId);

        let selectedAccountId = customerCB.model.GetData("Id", customerCB.currentIndex);
        installationEditorContainer.documentModel.SetData("AccountId", selectedAccountId);

        let activeLicenses = installationEditorContainer.documentModel.AddTreeModel("ActiveLicenses");

        for (let i = 0; i < licensesTable.rowModel.count; i++){
            let rowObj = licensesTable.rowModel.get(i);

            let licenseId = rowObj["Id"];
            let licenseName = rowObj["Name"];
            let expirationState  = rowObj["ExpirationState"];
            let expiration  = rowObj["Expiration"];
            let state = rowObj["LicenseState"];

            console.log("rowObj", JSON.stringify(rowObj));

            if (state == Qt.Checked){

                let index = activeLicenses.InsertNewItem();

                activeLicenses.SetData("Id", licenseId, index);
                activeLicenses.SetData("Name", licenseName, index);

                if (expirationState == Qt.Checked){
                    activeLicenses.SetData("Expiration", expiration, index);
                }
                else{
                    activeLicenses.SetData("Expiration", "", index);
                }
            }
        }

        undoRedoManager.endChanges();
        console.log("End updateModel");
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }


    Column {
        id: bodyColumn;

        width: 500;

        spacing: 7;

        Text {
            id: titleInstanceId;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            text: qsTr("Instance-ID");
        }

        RegExpValidator {
            id: regexValid;

            Component.onCompleted: {
                console.log("RegExpValidator onCompleted");
                let regex = settingsProvider.getInstanceMask();
                console.log("regex", regex);

                let re = new RegExp(regex)
                if (re){
                    regexValid.regExp = re;
                }
            }
        }

        CustomTextField {
            id: helperInput;

            visible: false;
            textInputValidator: regexValid;

            onTextChanged: {
                console.log("acceptableInput", helperInput.text, helperInput.acceptableInput);
                errorMessage.visible = !helperInput.acceptableInput;
            }
        }

        CustomTextField {
            id: instanceIdInput;

            width: parent.width;
            height: 30;

            placeHolderText: qsTr("Enter the instance-ID");

            borderColor: helperInput.acceptableInput ? Style.iconColorOnSelected : Style.errorTextColor;

            maximumLength: 17;

            onTextChanged: {
                helperInput.text = instanceIdInput.text;
            }

            onEditingFinished: {
                let currentId = installationEditorContainer.documentModel.GetData("Id");
                if (currentId != instanceIdInput.text){
                    installationEditorContainer.updateModel();
                }
            }
        }

        Text {
            id: errorMessage;

            color: Style.errorTextColor;

            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            visible: false;

            text: qsTr("Incorrect input instance-ID");
        }

        Text {
            id: titleCustomer;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            text: qsTr("Customer");
        }

        ComboBox {
            id: customerCB;

            width: parent.width;
            height: 23;

            radius: 3;

            onCurrentIndexChanged: {
                if (!installationEditorContainer.blockUpdatingModel){
                    installationEditorContainer.updateModel();
                }
            }
        }

        Text {
            id: titleProduct;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            text: qsTr("Product");
        }

        ComboBox {
            id: productCB;

            width: parent.width;
            height: 23;

            radius: 3;

            onCurrentIndexChanged: {
                console.log("InstallationEditor onCurrentIndexChanged",productCB.currentIndex);

                if (!installationEditorContainer.blockUpdatingModel){
                    let selectedProductId = productCB.model.GetData("Id", productCB.currentIndex);

                    //                        commandsDelegate.updateLicenses(selectedProductId);

                    installationEditorContainer.updateModel();

                    installationEditorContainer.updateGui();
                }
            }
        }

        Text {
            id: titleLicenses;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            text: qsTr("Licenses");
        }
    }//Column bodyColumn

    BasicTableView {
        id: licensesTable;

        anchors.top: bodyColumn.bottom;
        anchors.topMargin: 10;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;

        width: bodyColumn.width;


        rowDelegate: LicenseInstanceItemDelegate {}

        Component.onCompleted: {
            licensesTable.addColumn({"Id": "Name", "Name": "License Name"});
            licensesTable.addColumn({"Id": "Expiration", "Name": "Expiration"});
        }

        onRowModelDataChanged: {
            console.log("licensesTable onRowModelDataChanged");

            if (!installationEditorContainer.blockUpdatingModel){
                installationEditorContainer.updateModel();
            }
        }
    }
}//Container


