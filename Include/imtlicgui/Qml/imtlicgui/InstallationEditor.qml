import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: installationEditorContainer;

    property TreeItemModel documentModel: TreeItemModel{}
    property TreeItemModel licensesModel: TreeItemModel{}
    property TreeItemModel productsModel: TreeItemModel{}
    property TreeItemModel orderProductsModel: TreeItemModel{}
    property TreeItemModel softwareModel: TreeItemModel{}
    property TreeItemModel hardwareModel: TreeItemModel{}

    property bool blockUpdatingModel: false;
    property bool centered: true;


    width: 550;
    height: 800;

    UuidGenerator {
        id: uuidGenerator;
    }

    Component.onCompleted: {

    }

    function started(){
        productCB.model = installationEditorContainer.productsModel;

        updateCategoryProducts("Software");
        updateCategoryProducts("Hardware");

        console.log("installationEditorContainer.hardwareModel", installationEditorContainer.hardwareModel.toJSON())
        console.log("installationEditorContainer.softwareModel", installationEditorContainer.softwareModel.toJSON())

        generateId();
        updateGui();
        console.log("started", documentModel.toJSON());
    }

    function generateId(){
        if (!documentModel.ContainsKey("Id")){
            let uuid = uuidGenerator.generateUUID();
            documentModel.SetData("Id", uuid);
        }
    }

    function getProductName(productId){
        let productModel = installationEditorContainer.productsModel;
        let retVal = "";
        for (let i = 0; i < productsModel.GetItemsCount(); i++){
            let id = productsModel.GetData("Id", i);
            if (id === productId){
                retVal = productsModel.GetData("Name", i);
                break;
            }
        }
        return retVal;
    }

    function updateCategoryProducts(category){
        let productModel = installationEditorContainer.orderProductsModel;
        let resultModel;
        if (category === "Hardware"){
            resultModel = installationEditorContainer.hardwareModel;
        }
        else{
            resultModel = installationEditorContainer.softwareModel;
        }
        resultModel.Clear();
        console.log("updateCategoryProducts", resultModel.toJSON())
        for (let i = 0; i < productModel.GetItemsCount(); i++){
            let modelCategory = productModel.GetData("CategoryId", i);
            let pairId = productModel.GetData("PairId", i);
            if (category === modelCategory && pairId != ""){
                let resultIndex = resultModel.InsertNewItem();
                let productId = productModel.GetData("ProductId", i);
                let uuidId = productModel.GetData("Id", i);
                resultModel.SetData("ProductId", productId, resultIndex);
                resultModel.SetData("UuidId", uuidId, resultIndex);
                resultModel.SetData("Name", getProductName(productId), resultIndex);
            }
        }
        console.log("updateCategoryProducts2", resultModel.toJSON())
    }

    onDocumentModelChanged: {
        console.log("InstallationEditor onDocumentModelChanged", documentModel.toJSON());
        generateId();

        updateGui();
    }

//    Rectangle {
//        anchors.fill: parent;
//        color: "white";
//    }


    function updateGui(){
//        console.log("updateGui start", blockUpdatingModel, documentModel.toJSON())
        blockUpdatingModel = true;

        let productId = documentModel.GetData("ProductId");
        let pairId = documentModel.GetData("PairId");
        let categoryId = documentModel.GetData("CategoryId");

        let productModel = productCB.model;
        if (productModel){
            for (let i = 0; i < productModel.GetItemsCount(); i++){
                let id = productModel.GetData("Id", i);
                if (id === productId){
                    productCB.currentIndex = i;
                    break;
                }
            }
        }

        if (categoryId == "Software"){
            dependencyCB.model = installationEditorContainer.hardwareModel;
        }
        else{
            dependencyCB.model = installationEditorContainer.softwareModel;
        }
        productModel = dependencyCB.model;
        if (productModel){
            for (let i = 0; i < productModel.GetItemsCount(); i++){
                let uuidId = productModel.GetData("UuidId", i);
                if (uuidId === pairId){
                    dependencyCB.currentIndex = i;
                    break;
                }
            }
        }

        if (documentModel.ContainsKey("SerialNumber")){
            serialNumberInput.text = documentModel.GetData("SerialNumber");
        }

        if (documentModel.ContainsKey("MacAddress")){
            macAddressInput.text = documentModel.GetData("MacAddress");
        }

        licensesTable.rowModel.clear();

        let activeLicensesModel = documentModel.GetData("ActiveLicenses");
        if (!activeLicensesModel){
            activeLicensesModel = documentModel.AddTreeModel("ActiveLicenses");
        }

        let licensesModel;
        if (installationEditorContainer.licensesModel){
            for (let i = 0; i < installationEditorContainer.licensesModel.GetItemsCount(); i++){
                let id = installationEditorContainer.licensesModel.GetData("Id", i);
                if (id === productId){
                    let productLicensesModel = installationEditorContainer.licensesModel.GetData("Licenses", i);
                    licensesModel = productLicensesModel;
                }
            }
        }

        console.log("licensesProvider", installationEditorContainer.licensesModel.toJSON());

        if (licensesModel){
            console.log("licensesModel", licensesModel.toJSON());
            for (let i = 0; i < licensesModel.GetItemsCount(); i++){
                let licenseId = licensesModel.GetData("Id", i);
                let licenseName = licensesModel.GetData("Name", i);

                let row = {"Id": licenseId, "Name": licenseName, "LicenseState": Qt.Unchecked, "ExpirationState": Qt.Unchecked, "Expiration": ""}

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

                licensesTable.addRow(row);
            }
        }
//        console.log("updateGui finish", blockUpdatingModel)

        blockUpdatingModel = false;
    }

    function updateModel(){

        let selectedProductId = productCB.model.GetData("Id", productCB.currentIndex);
        documentModel.SetData("ProductId", selectedProductId);

        documentModel.SetData("CategoryId",  bodyColumn.productCategory);

        let selectedPairId = dependencyCB.model.GetData("UuidId", dependencyCB.currentIndex);
        documentModel.SetData("PairId", selectedPairId);

        documentModel.SetData("SerialNumber", serialNumberInput.text);
        documentModel.SetData("MacAddress", macAddressInput.text);

        let activeLicenses = documentModel.AddTreeModel("ActiveLicenses");

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
        console.log("updateModel finish", documentModel.toJSON())

    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    Column {
        id: bodyColumn;

        anchors.left: parent.left;
        anchors.leftMargin: 10;
        anchors.right: parent.right;
        anchors.rightMargin: 10;
        spacing: 7;
        property string productCategory;

        Text {
            id: titleProduct;

            text: qsTr("Product");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        ComboBox {
            id: productCB;

            width: parent.width;
            height: 23;

            radius: 3;

            onCurrentIndexChanged: {
                bodyColumn.productCategory = productCB.model.GetData("CategoryId", productCB.currentIndex);
//                console.log("InstallationEditor onCurrentIndexChanged",productCB.currentIndex, bodyColumn.productCategory);

                if (bodyColumn.productCategory == "Software"){
                    dependencyCB.model = installationEditorContainer.hardwareModel;
                }
                else{
                    dependencyCB.model = installationEditorContainer.softwareModel;
                }

                console.log("InstallationEditor onCurrentIndexChanged",productCB.currentIndex, dependencyCB.model.toJSON());

                if (!blockUpdatingModel){
                    installationEditorContainer.updateModel();
                    installationEditorContainer.updateGui();
                }
            }
        }

        Text {
            id: titleDependency;

            text: qsTr("Pair link");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        ComboBox {
            id: dependencyCB;

            width: parent.width;
            height: 23;

            radius: 3;

            onCurrentIndexChanged: {
                if (!blockUpdatingModel){
                    installationEditorContainer.updateModel();
                    installationEditorContainer.updateGui();
                }
            }
        }

        Text {
            id: titleMacAddress;

            text: qsTr("MAC address");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            visible: bodyColumn.productCategory == "Hardware"
        }

        RegExpValidator {
            id: macAddressRegExp;

            regExp: /^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$/;
        }

        CustomTextField {
            id: macAddressInput;

            width: parent.width;
            height: 30;
            placeHolderText: qsTr("Enter the MAC address");
            borderColor: Style.iconColorOnSelected;
            maximumLength: 17;
            visible: bodyColumn.productCategory == "Hardware"

            textInputValidator: macAddressRegExp;

            onEditingFinished: {
                updateModel();
            }
        }

        Text {
            id: titleSerialNumber;

            text: qsTr("Serial number");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            visible: bodyColumn.productCategory == "Hardware"
        }

        CustomTextField {
            id: serialNumberInput;

            width: parent.width;
            height: 30;
            placeHolderText: qsTr("Enter the serial number");
            borderColor: Style.iconColorOnSelected;
            maximumLength: 17;
            visible: bodyColumn.productCategory == "Hardware";

            onEditingFinished: {
                updateModel();
            }
        }

    }//Column bodyColumn

    Text {
        id: titleLicenses;
        anchors.top: bodyColumn.bottom;
        anchors.topMargin: 10;
        anchors.left: bodyColumn.left;

        text: qsTr("Licenses");
        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;
        visible: bodyColumn.productCategory == "Software";
    }

    BasicTableView {
        id: licensesTable;

        anchors.top: titleLicenses.bottom;
        anchors.topMargin: 10;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;
        anchors.left: bodyColumn.left;

        width: bodyColumn.width;
        visible: titleLicenses.visible;


        rowDelegate: LicenseInstanceItemDelegate {}

        Component.onCompleted: {
            licensesTable.addColumn({"Id": "Name", "Name": "License Name"});
            licensesTable.addColumn({"Id": "Expiration", "Name": "Expiration"});
        }

        onRowModelDataChanged: {
            console.log("licensesTable onRowModelDataChanged");

            if (!blockUpdatingModel){
                installationEditorContainer.updateModel();
            }
        }
    }

}//Container


