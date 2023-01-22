import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: installationEditorContainer;

//    commandsDelegateSourceComp: installationEditorCommandsDelegate;


    property TreeItemModel productsModel: TreeItemModel {}

    property bool blockUpdatingModel: false;
    property bool centered: true;

    width: 550;
    height: 800;


    Component.onCompleted: {
        licensesProvider.updateModel();
    }

    Component {
        id: installationEditorCommandsDelegate;
        InstallationEditorCommandsDelegate {}
    }

//    onDocumentModelChanged: {
//        let activeLicensesModel = documentModel.GetData("ActiveLicenses");
//        if (!activeLicensesModel){
//            activeLicensesModel = documentModel.AddTreeModel("ActiveLicenses");
//        }

//        updateGui();

//        undoRedoManager.registerModel(documentModel)
//    }


    onProductsModelChanged: {
        console.log("onProductsModelChanged", productsModel);
        productCB.model = productsModel;
    }

    Rectangle {
        anchors.fill: parent;
        color: "white";
    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: installationEditorContainer.commandsId;

        onModelStateChanged: {
            updateGui();
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
        blockUpdatingModel = true;

        instanceIdInput.text = documentModel.GetData("Id");

        let accountId = documentModel.GetData("AccountId");
        let productId = documentModel.GetData("ProductId");

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

        let activeLicensesModel = documentModel.GetData("ActiveLicenses");
        if (!activeLicensesModel){
            activeLicensesModel = documentModel.AddTreeModel("ActiveLicenses");
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

                console.log("row addRow", JSON.stringify(row));
                licensesTable.addRow(row);
            }
        }

        blockUpdatingModel = false;
        console.log("End updateGui");
    }

    function updateModel(){
        console.log("Begin updateModel");
        undoRedoManager.beginChanges();

 //       documentModel.SetData("Id", instanceIdInput.text)

        let selectedProductId = productCB.model.GetData("Id", productCB.currentIndex);
        documentModel.SetData("ProductId", selectedProductId);

        let selectedAccountId = customerCB.model.GetData("Id", customerCB.currentIndex);
        documentModel.SetData("AccountId", selectedAccountId);

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

        undoRedoManager.endChanges();
        console.log("End updateModel");
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }


    Column {
        id: bodyColumn;

        anchors.left: installationEditorContainer.left;
        anchors.leftMargin: 10;
        anchors.right: installationEditorContainer.right;
        anchors.rightMargin: 10;
        spacing: 7;

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
                console.log("InstallationEditor onCurrentIndexChanged",productCB.currentIndex);

                if (!blockUpdatingModel){
                    let selectedProductId = productCB.model.GetData("Id", productCB.currentIndex);

                    //                        commandsDelegate.updateLicenses(selectedProductId);

                    updateModel();

                    updateGui();
                }
            }
        }

        Text {
            id: titleDependency;

            text: qsTr("Dependency");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        ComboBox {
            id: dependencyComboBox;

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
            id: titleLicenses;

            text: qsTr("Licenses");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        ComboBox {
            id: licenseComboBox;

            width: parent.width;
            height: 23;

            radius: 3;

            onCurrentIndexChanged: {

            }
        }

        Item {
            id: rightPart;

            width: parent.width;
            height: 100;
            property int expirationState: 0;
            property string expirationDate;

//            visible: checkBoxLicense.checkState === 2;

            CheckBox {
                id: checkBoxExpiration;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: rightPart.left;

                checkState: rightPart.expirationState;

                onClicked: {
                    console.log("checkBoxExpiration onClicked");
                    rightPart.expirationState = Qt.Checked - checkBoxExpiration.checkState;

                    if (model.ExpirationState == Qt.Checked){
                        datePicker.setCurrentDay();
                    }
                    console.log("checkBoxExpiration state", state);
                    //root.rowModelDataChanged(packageTreeItemDelegate, "ExpirationState");
                }
            }

            Text {
                id: textUnlimited;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: checkBoxExpiration.right;
                anchors.leftMargin: 5;

                visible: checkBoxExpiration.checkState === 0;

                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
                color: Style.textColor;

                text: qsTr("Unlimited");
            }

            DatePicker {
                id: datePicker;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: checkBoxExpiration.right;
                anchors.leftMargin: 5;

                visible: checkBoxExpiration.checkState === 2;

                width: 100;
                height: 20;

                currentDayButtonVisible: false;
                startWithCurrentDay: false;

                property string expirationDate: rightPart.expirationDate;

                onExpirationDateChanged: {
                    console.log("onExpirationDateChanged", datePicker.expirationDate);

                    let date = rightPart.expirationDate;
                    let data = date.split("-");
                    datePicker.setDate(Number(data[0]), Number(data[1]) - 1, Number(data[2]));
                }

                //                Component.onCompleted: {
                //                    console.log("onCompleted");
                //                    let date = model.Expiration;
                //                    let data = date.split("-");
                //                    datePicker.setDate(Number(data[0]), Number(data[1]) - 1, Number(data[2]));
                //                }

                onDateChanged: {
                    console.log("onDateChanged", datePicker.getDate());
                    rightPart.expirationDate = datePicker.getDate();

                    console.log("model.Expiration", datePicker.expirationDate);

//                    root.rowModelDataChanged(packageTreeItemDelegate, "Expiration");
                }
            }
        }

    }//Column bodyColumn

//    BasicTableView {
//        id: licensesTable;

//        anchors.top: bodyColumn.bottom;
//        anchors.topMargin: 10;
//        anchors.bottom: parent.bottom;
//        anchors.bottomMargin: 10;
//        anchors.left: parent.left;
//        anchors.leftMargin: 10;

//        width: bodyColumn.width;


//        rowDelegate: LicenseInstanceItemDelegate {}

//        Component.onCompleted: {
//            licensesTable.addColumn({"Id": "Name", "Name": "License Name"});
//            licensesTable.addColumn({"Id": "Expiration", "Name": "Expiration"});
//        }

//        onRowModelDataChanged: {
//            console.log("licensesTable onRowModelDataChanged");

//            if (!blockUpdatingModel){
//                updateModel();
//            }
//        }
//    }
}//Container


