import QtQuick 2.0
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
//import '../../../imtgui/Qml/imtgui/AuxComponents'

Rectangle {
    id: editLicenseContainer;

    width: 400;
    height: 280;

    radius: 2;
    color: Style.backgroundColor;
    clip: true;
    focus: true;

    property Item resultItem;
    property Item loaderDialog;
    property Item collectionViewLicenses;

    property string licenseId;
    property string licenseName;

    property real backgroundOpacity: 0.4;

    property bool backgroundExist: true;
    property bool clickBackgroundClose: false;
    property bool centered: true;

    signal okClicked(string newId, string newName);
    signal cancelClicked();

    function exit(status) {
        var parameters  = {};
        if (status === "ok") {

            parameters["newLicenseId"] = tfcLicenseIdText.text;
            parameters["newLicenseName"] = tfcLicenseNameText.text;
        }
        parameters["dialog"] = "EditLicense";
        parameters["status"] = status;
        editLicenseContainer.resultItem.dialogResult(parameters);
    }

    function validateId(id) {
        if (id === "") {
            return "Id can't be empty!";
        }

        if (!editLicenseContainer.collectionViewLicenses){
            return "";
        }

        var dataModelLocal = editLicenseContainer.collectionViewLicenses.collectionViewModel.GetData("data");

        for (var i = 0; i < dataModelLocal.GetItemsCount(); i++) {

            if (dataModelLocal.GetData("Id", i ) === id &&
                    editLicenseContainer.collectionViewLicenses.selectedIndex !== i) {
                return "Id already exist!";
            }
        }

        return "";
    }

    function validateName(name) {
        if (name === "") {
            return "Name can't be empty!";
        }

        return "";
    }

    MouseArea {
        anchors.fill: parent;
    }

    Rectangle {
        id: editLicenseDialogTopPanel;

        width: editLicenseContainer.width;
        height: 40;

        border.width: 1;
        border.color: editLicenseContainer.color;

        color: Style.baseColor;

        Image {
            id: iconEditLicenseDialog;

            anchors.left: editLicenseDialogTopPanel.left;
            anchors.verticalCenter: editLicenseDialogTopPanel.verticalCenter;

            width: 15;
            height: 15;

            source: "../../../" + "Icons/" + Style.theme + "/" + "Icon" + "_" + "On" + "_" + "Normal" + ".svg";
        }

        Text {
            id: titleEditFeatureDialog;

            anchors.left: iconEditLicenseDialog.right;
            anchors.leftMargin: 10;
            anchors.verticalCenter: editLicenseDialogTopPanel.verticalCenter;
            anchors.horizontalCenter: editLicenseDialogTopPanel.horizontalCenter;

            text: "Edit License";
            color: Style.textColor;

            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        AuxButton {
            id: editLicenseDialogCloseButton;

            anchors.right: editLicenseDialogTopPanel.right;
            anchors.verticalCenter: editLicenseDialogTopPanel.verticalCenter;
            anchors.rightMargin: 15;

            width: 17;
            height: 17;

            iconSource: "../../../" + "Icons/" + "Light" + "/" + "Close" + "_" + "On" + "_" + "Normal" + ".svg";

            onClicked: {
                editLicenseContainer.exit("close");
                editLicenseContainer.loaderDialog.closeItem();
            }
        }
    }


    Rectangle {
        id: editLicenseDialogBody;

        anchors.topMargin: 20;
        anchors.top: editLicenseDialogTopPanel.bottom;
        anchors.horizontalCenter: editLicenseContainer.horizontalCenter;
        anchors.verticalCenter: editLicenseContainer.verticalCenter;

        width: editLicenseContainer.width - 50;
        height: editLicenseContainer.height - 50;

        color: editLicenseContainer.color;

        Text {
            id: titleLicenseName;

            text: qsTr("License Name");

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        Rectangle {
            id: tfcLicenseName;

            anchors.top: titleLicenseName.bottom;

            width: editLicenseDialogBody.width;
            height: 45;

            color: Style.imagingToolsGradient1;

            border.width: 1;
            border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

            TextFieldCustom {
                id: tfcLicenseNameText;

                anchors.horizontalCenter: tfcLicenseName.horizontalCenter;
                anchors.verticalCenter: tfcLicenseName.verticalCenter;

                width: tfcLicenseName.width - 22;
                height: 23;

                text: editLicenseContainer.licenseName;
                focus: true;
                correctData: errorNameMessage.text !== "";
                borderColor: errorNameMessage.text !== "" ? Style.errorTextColor : Style.iconColorOnSelected;

                onTextChanged: {
                    errorNameMessage.text = "";

                    var nameMessage = editLicenseContainer.validateName(tfcLicenseNameText.text);
                    if (nameMessage !== "") {
                       errorNameMessage.text = nameMessage;
                    }
                }
            }
        }

        Text {
            id: titleLicenseId;

            anchors.top: tfcLicenseName.bottom;
            anchors.topMargin: 25;

            text: qsTr("License - ID");

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        Rectangle {
            id: tfcLicenseId;

            anchors.top: titleLicenseId.bottom;

            width: editLicenseDialogBody.width;
            height: 45;

            color: Style.imagingToolsGradient1;

            border.width: 1;
            border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

            TextFieldCustom {
                id: tfcLicenseIdText;

                anchors.horizontalCenter: tfcLicenseId.horizontalCenter;
                anchors.verticalCenter: tfcLicenseId.verticalCenter;

                width: tfcLicenseId.width - 22;
                height: 23;

                text: editLicenseContainer.licenseId;
                borderColor: errorIdMessage.text !== "" ? Style.errorTextColor : Style.iconColorOnSelected;
                correctData: errorIdMessage.text !== "";

                onTextChanged: {
                    errorIdMessage.text = "";

                    var idMessage = editLicenseContainer.validateId(tfcLicenseIdText.text);
                    if (idMessage !== "") {
                       errorIdMessage.text = idMessage;
                    }
                }
            }
        }

        AuxButton {
            id: okButton;

            anchors.top: tfcLicenseId.bottom;
            anchors.right: cancelButton.left;
            anchors.rightMargin: 15;
            anchors.topMargin: 30;

            width: 70;
            height: 25;

            hasText: true;
            hasIcon: false;

            textButton: "OK";
            borderColor: okButton.highlighted ? Style.iconColorOnSelected :
                                              okButton.enabled ? Style.buttonColor :
                                                                 Style.imagingToolsGradient2;
            backgroundColor: okButton.enabled ? Style.imagingToolsGradient1 : Style.backgroundColor;

            enabled: errorIdMessage.text === "" && errorNameMessage.text === "";

            onClicked: {
                editLicenseContainer.okClicked(tfcLicenseIdText.text, tfcLicenseNameText.text);
                editLicenseContainer.exit("ok");
                editLicenseContainer.loaderDialog.closeItem();
            }
        }

        AuxButton {
            id: cancelButton;

            anchors.top: tfcLicenseId.bottom;
            anchors.topMargin: 30;
            anchors.right: tfcLicenseId.right;

            width: 70;
            height: 25;

            hasText: true;
            hasIcon: false;

            textButton: "Cancel";
            borderColor: cancelButton.highlighted ? Style.iconColorOnSelected : Style.buttonColor;
            backgroundColor: Style.imagingToolsGradient1;

            onClicked: {
                editLicenseContainer.exit("cancel");
                editLicenseContainer.loaderDialog.closeItem();
            }
        }

        Text {
            id: errorIdMessage;

            anchors.top: tfcLicenseId.bottom;
            anchors.topMargin: 3;
            anchors.left: editLicenseDialogBody.left;
            anchors.leftMargin: 10;

            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            color: Style.errorTextColor;

            text: "";
        }

        Text {
            id: errorNameMessage;

            anchors.top: tfcLicenseName.bottom;
            anchors.topMargin: 3;
            anchors.left: editLicenseDialogBody.left;
            anchors.leftMargin: 10;

            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            color: Style.errorTextColor;

            text: "";
        }
    }
}
