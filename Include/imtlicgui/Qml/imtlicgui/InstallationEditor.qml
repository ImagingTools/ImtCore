import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: installationEditorContainer;

    property TreeItemModel documentModel: TreeItemModel{}

    property bool blockUpdatingModel: false;
    property bool centered: true;

    width: 550;
    height: 800;

    Component.onCompleted: {
        licensesProvider.updateModel();

        dataProvider.updateModel({}, ["Id", "Name", "CategoryId"]);
    }

    onDocumentModelChanged: {
        console.log("InstallationEditor onDocumentModelChanged", documentModel.toJSON());
    }

    CollectionDataProvider {
        id: dataProvider;

        commandId: "Products";

        onCollectionModelChanged: {
            productCB.model = dataProvider.collectionModel;

            installationEditorContainer.updateGui();
        }
    }

    Rectangle {
        anchors.fill: parent;
        color: "white";
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
        blockUpdatingModel = true;

        let productId = documentModel.GetData("ProductId");
        let pairId = documentModel.GetData("PairId");

        let productModel = productCB.model;
        for (let i = 0; i < productModel.GetItemsCount(); i++){
            let id = productModel.GetData("Id", i);
            if (id === productId){
                productCB.currentIndex = i;
                break;
            }
        }

        serialNumberInput.text = documentModel.GetData("SerialNumber");
        macAddressInput.text = documentModel.GetData("MacAddress");

        let licensesModel;
        if (licensesProvider.model){
            for (let i = 0; i < licensesProvider.model.GetItemsCount(); i++){
                let id = licensesProvider.model.GetData("Id", i);
                if (id === productId){
                    let productLicensesModel = licensesProvider.model.GetData("Licenses", i);
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
        documentModel.SetData("ProductId", selectedProductId);

        let selectedPairId = customerCB.model.GetData("Id", dependencyCB.currentIndex);
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
                console.log("InstallationEditor onCurrentIndexChanged",productCB.currentIndex, bodyColumn.productCategory);

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

            currentText: "RTVision.3d Sensor";

            model: ListModel {
                id: modelCategogy;
                ListElement {
                    Name: "RTVision.3d Sensor"
                }
            }

            onCurrentIndexChanged: {

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
            visible: bodyColumn.productCategory == "Hardware"

            onEditingFinished: {
                updateModel();
            }
        }

    }//Column bodyColumn

    Text {
        id: titleLicenses;
        anchors.top: bodyColumn.bottom;
        anchors.topMargin: 10;

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
                updateModel();
            }
        }
    }

}//Container


