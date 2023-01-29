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

    property string selectedPairId: "";

    width: 550;
    height: 800;

    UuidGenerator {
        id: uuidGenerator;
    }

    Component.onCompleted: {

    }

    function started(){
        productCB.model = installationEditorContainer.productsModel;

        let productId = documentModel.GetData("ProductId");
        let id = documentModel.GetData("Id");
        let pairId = documentModel.GetData("PairId");
        let categoryId = documentModel.GetData("CategoryId");
        let productModel = installationEditorContainer.orderProductsModel;

        selectedPairId = "";
        dependencyCB.currentIndex = -1;
        dependencyCB.model.Clear();
        let isPairFinded = false;
        if (productModel){
            for (let i = 0; i < productModel.GetItemsCount(); i++){
                //  let modelMacAddress = productModel.GetData("PairId", i);
                if (categoryId == "Hardware" && productModel.GetData("CategoryId", i) == "Software"
                        && findHardwarePair(productModel.GetData("Id", i)) !== ""){
                    isPairFinded = true;
                }
                if (categoryId == "Software" && id === productModel.GetData("PairId", i) && productModel.GetData("CategoryId", i) == "Hardware"){
                    isPairFinded = true;
                }
                if (isPairFinded){
                    let dependencyModel = dependencyCB.model;
                    dependencyModel.SetData("ProductId", productModel.GetData("ProductId", i), 0);
                    dependencyModel.SetData("Id", productModel.GetData("Id", i), 0);
                    dependencyModel.SetData("MacAddress", productModel.GetData("MacAddress", i), 0);
                    dependencyModel.SetData("Name", getProductName(productModel.GetData("ProductId", i)), 0);
                    let index = dependencyModel.InsertNewItem();
                    dependencyModel.SetData("ProductId", "", index);
                    dependencyModel.SetData("Id","", index);
                    dependencyModel.SetData("MacAddress", "", index);
                    dependencyModel.SetData("Name", qsTr("None"), index);
                    break;
                }
            }
        }
        //        }
        if (!isPairFinded){
            if (categoryId == "Software"){
                updateHardwareCategoryProducts()
            }
            else{
                updateSoftwareCategoryProducts()
            }
        }

        generateId();
        updateGui();
        if (isPairFinded){
            dependencyCB.currentIndex = 0;
        }

        console.log("started documentModel", documentModel.toJSON());
        console.log("started licensesModel", licensesModel.toJSON());

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

//    function updateCategoryProducts(category){
//        let productModel = installationEditorContainer.orderProductsModel;
//        let resultModel = dependencyCB.model;
//        console.log("updateCategoryProducts", resultModel.toJSON())
//        for (let i = 0; i < productModel.GetItemsCount(); i++){
//            let modelCategory = productModel.GetData("CategoryId", i);
//            let pairId = productModel.GetData("PairId", i);
//            if (category === modelCategory && pairId === ""){
//                let resultIndex = resultModel.InsertNewItem();
//                let productId = productModel.GetData("ProductId", i);
//                resultModel.SetData("ProductId", productId, resultIndex);
//                resultModel.SetData("Id", productModel.GetData("Id", i), resultIndex);
//                resultModel.SetData("MacAddress", productModel.GetData("MacAddress", i), resultIndex);
//                resultModel.SetData("Name", getProductName(productId), resultIndex);
//            }
//        }
//        console.log("updateCategoryProducts2", resultModel.toJSON())
//    }
    function updateHardwareCategoryProducts(){
        let productModel = installationEditorContainer.orderProductsModel;
        let resultModel = dependencyCB.model;
        for (let i = 0; i < productModel.GetItemsCount(); i++){
            let pairId = productModel.GetData("PairId", i);
            if (productModel.GetData("CategoryId", i) === "Hardware" && pairId === ""){
                let resultIndex = resultModel.InsertNewItem();
                let productId = productModel.GetData("ProductId", i);
                resultModel.SetData("ProductId", productId, resultIndex);
                resultModel.SetData("Id", productModel.GetData("Id", i), resultIndex);
                resultModel.SetData("MacAddress", productModel.GetData("MacAddress", i), resultIndex);
                resultModel.SetData("Name", getProductName(productId), resultIndex);
            }
        }
        console.log("updateHardwareCategoryProducts", resultModel.toJSON())
    }

    function findHardwarePair(id){
        let retVal = ""
        let productModel = installationEditorContainer.orderProductsModel;
        for (let i = 0; i < productModel.GetItemsCount(); i++){
            let pairId = productModel.GetData("PairId", i)
            if (productModel.GetData("CategoryId", i) === "Hardware"
                    && pairId === id){
                retVal = productModel.GetData("Id", i)
                break
            }

        }

        return retVal
    }

    function updateSoftwareCategoryProducts(){
        let productModel = installationEditorContainer.orderProductsModel;
        let resultModel = dependencyCB.model;
        for (let i = 0; i < productModel.GetItemsCount(); i++){
            if (productModel.GetData("CategoryId", i) === "Software"){
                let hardwareId = findHardwarePair(productModel.GetData("Id", i));
                if (hardwareId == ""){
                    let productId = productModel.GetData("ProductId", i);
                    let resultIndex = resultModel.InsertNewItem();
                    resultModel.SetData("ProductId", productId, resultIndex);
                    resultModel.SetData("Id", productModel.GetData("Id", i), resultIndex);
                    resultModel.SetData("MacAddress", productModel.GetData("MacAddress", i), resultIndex);
                    resultModel.SetData("Name", getProductName(productId), resultIndex);
                }
            }
        }
        console.log("updateSoftwareCategoryProducts", resultModel.toJSON())
    }

    onDocumentModelChanged: {
        console.log("InstallationEditor onDocumentModelChanged", documentModel.toJSON());
    }

//    Rectangle {
//        anchors.fill: parent;
//        color: "white";
//    }


    function updateGui(){
        blockUpdatingModel = true;

        let productId = documentModel.GetData("ProductId");
        let id = documentModel.GetData("Id");
        let pairId = documentModel.GetData("PairId");
        let categoryId = documentModel.GetData("CategoryId");
        let macAddress = documentModel.GetData("MacAddress");

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

        if (licensesModel){
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

        blockUpdatingModel = false;
    }

    function updateModel(){

        let selectedProductId = productCB.model.GetData("Id", productCB.currentIndex);
        if (selectedProductId){
            documentModel.SetData("ProductId", selectedProductId);
        }

        documentModel.SetData("CategoryId",  bodyColumn.productCategory);

        selectedPairId = "";
        let pairId = dependencyCB.model.GetData("Id", dependencyCB.currentIndex);
        if (pairId){
            selectedPairId = pairId;
        }

        documentModel.SetData("SerialNumber", serialNumberInput.text);
        documentModel.SetData("MacAddress", macAddressInput.text);

        let activeLicenses = documentModel.AddTreeModel("ActiveLicenses");
        activeLicenses.Clear();
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

//                if (bodyColumn.productCategory == "Software"){
//                    dependencyCB.model = installationEditorContainer.hardwareModel;
//                }
//                else{
//                    dependencyCB.model = installationEditorContainer.softwareModel;
//                }

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

            model: TreeItemModel{}

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


