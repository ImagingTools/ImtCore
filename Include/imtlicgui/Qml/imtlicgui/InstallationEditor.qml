import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: installationEditorContainer;

    property TreeItemModel documentModel: TreeItemModel{}

//    commandsDelegateSourceComp: installationEditorCommandsDelegate;

    property bool blockUpdatingModel: false;
    property bool centered: true;

    width: 550;
    height: 800;


    Component.onCompleted: {
        licensesProvider.updateModel();
    }

    InstallationEditorCommandsDelegate {
        onProductsModelChanged: {
            console.log("onProductsModelChanged", productsModel);
            productCB.model = productsModel;

            installationEditorContainer.updateModel();
        }
    }

//    Component {
//        id: installationEditorCommandsDelegate;
//        InstallationEditorCommandsDelegate {
//            onProductsModelChanged: {
//                console.log("onProductsModelChanged", productsModel);
//                productCB.model = productsModel;
//            }
//        }

//    }

    onDocumentModelChanged: {
//        let activeLicensesModel = documentModel.GetData("ActiveLicenses");
//        if (!activeLicensesModel){
//            activeLicensesModel = documentModel.AddTreeModel("ActiveLicenses");
//        }

        updateGui();

        undoRedoManager.registerModel(documentModel)
    }


//    onProductsModelChanged: {
//        console.log("onProductsModelChanged", productsModel);
//        productCB.model = productsModel;
//    }

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

  //      instanceIdInput.text = documentModel.GetData("Id");

        let productId = documentModel.GetData("ProductId");
        let pairId = documentModel.GetData("PairId");


        //        productCB.currentText = "";
        let productModel = productCB.model;
        for (let i = 0; i < productModel.GetItemsCount(); i++){
            let id = productModel.GetData("Id", i);
            if (id === productId){
                productCB.currentIndex = i;
                break;
            }
        }

        //        customerCB.currentText = "";
//        let dependencyModel = dependencyCB.model;
//        for (let i = 0; i < dependencyModel.GetItemsCount(); i++){
//            let id = dependencyModel.GetData("Id", i);
//            if (id === pairId){
//                dependencyCB.currentIndex = i;
//                break;
//            }
//        }

        let activeLicenseModel = documentModel.GetData("ActiveLicense");
        if (!activeLicenseModel){
            activeLicenseModel = documentModel.AddTreeModel("ActiveLicense");
        }

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
            console.log("licensesModel", licensesModel.toJSON());
            licenseCB.model = licensesModel;
            console.log("activeLicenseModel", activeLicenseModel.toJSON());
            for (let i = 0; i < licensesModel.GetItemsCount(); i++){
                let licenseId = licensesModel.GetData("Id", i);
                let licenseName = licensesModel.GetData("Name", i);
                let activeLicenseId = activeLicenseModel.GetData("Id");
                let expiration = activeLicenseModel.GetData("Expiration");
                if (licenseId == activeLicenseId){

                    if (expiration == ""){
                        rightPart.expirationState = Qt.Unchecked;
                    }
                    else{
                        rightPart.expirationState = Qt.Checked;
                        rightPart.expirationData = expiration;
                    }
                }
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

        let selectedPairId = customerCB.model.GetData("Id", dependencyCB.currentIndex);
        documentModel.SetData("PairId", selectedPairId);

        let activeLicense = documentModel.AddTreeModel("ActiveLicense");
        let licenseId;
        let expirationState  = rightPart.expirationState;
        let expiration  = rightPart.expirationDate;

        if( licenseCB.currentIndex >= 0){
             licenseId = licenseCB.model.GetData("Id", licenseCB.currentIndex);
        }
        activeLicense.SetData("Id", licenseId);

        if (expirationState == Qt.Checked){
            activeLicense.SetData("Expiration", expiration);
        }
        else{
            activeLicense.SetData("Expiration", "");
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

          //  commandId: "Products"

            onCurrentIndexChanged: {
                console.log("InstallationEditor onCurrentIndexChanged",productCB.currentIndex);

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
            id: titleLicenses;

            text: qsTr("Licenses");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        ComboBox {
            id: licenseCB;

            width: parent.width;
            height: 23;

            radius: 3;

            onCurrentIndexChanged: {
                console.log("InstallationEditor onCurrentIndexChanged",licenseCB.currentIndex);

                if (!blockUpdatingModel){
                    installationEditorContainer.licenseId = licenseCB.model.GetData("Id", licenseCB.currentIndex);
                }
            }
        }

        Item {
            id: rightPart;

            width: parent.width;
            height: 100;
            property int expirationState: 0;
            property string expirationDate;

            visible: licenseCB.currentIndex > -1;

            Text {
                id: titleExpiration;

                text: qsTr("Date of expiration");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CheckBox {
                id: checkBoxExpiration;

                anchors.top: titleExpiration.bottom;
                anchors.topMargin: 7
                anchors.left: rightPart.left;

                checkState: rightPart.expirationState;

                onClicked: {
                    console.log("checkBoxExpiration onClicked");
                    rightPart.expirationState = Qt.Checked - checkBoxExpiration.checkState;

                    if (rightPart.expirationState == Qt.Checked){
                        datePicker.setCurrentDay();
                    }
                    console.log("checkBoxExpiration state", state);
                    //root.rowModelDataChanged(packageTreeItemDelegate, "ExpirationState");
                }
            }

            Text {
                id: textUnlimited;

                anchors.verticalCenter: checkBoxExpiration.verticalCenter;
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

                anchors.verticalCenter: checkBoxExpiration.verticalCenter;
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


}//Container


