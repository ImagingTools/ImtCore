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

    property Item resultItem;
    property Item loaderDialog;
    property Item collectionViewLicenses;
    property Item thumbnailItem;

    property string licenseId;
    property string licenseName;

    property real backgroundOpacity: 0.3;

    property bool backgroundExist: true;
    property bool clickBackgroundClose: false;
    property bool centered: true;

    signal okClicked(string newId, string newName);
    signal cancelClicked();

    Component.onCompleted: {
        console.log("EditLicenseDialog onCompleted");
    }

    onFocusChanged: {
        console.log("EditLicenseDialog onFocusChanged", editLicenseContainer.focus);

        if (editLicenseContainer.focus){
            tfcLicenseNameText.setFocus(true);
        }
    }

    Keys.onPressed: {
        console.log("EditLicenseDialog keys pressed")
        if (event.key === Qt.Key_Escape){
            console.log('Key tab was pressed');
            cancelButton.clicked();
        }
        else if (event.key === Qt.Key_Return){
            okButton.clicked();
        }
    }

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
            return qsTr("License-ID can't be empty!");
        }

        if (!editLicenseContainer.collectionViewLicenses){
            return "";
        }

        var dataModelLocal = editLicenseContainer.collectionViewLicenses.collectionViewModel.GetData("data");

        for (var i = 0; i < dataModelLocal.GetItemsCount(); i++) {

            if (dataModelLocal.GetData("Id", i ) === id &&
                    editLicenseContainer.collectionViewLicenses.selectedIndex !== i) {
                return qsTr("License-ID already exists!");
            }
        }

        return "";
    }

    function validateName(name) {
        if (name === "") {
            return qsTr("License Name can't be empty!");
        }

        return "";
    }

    function generateKey() {
        console.log("EditLicenseDialog generateKey...");
        if (tfcLicenseIdText.text === "") {
            var key = "00.0000";
            //key = key.replace(/\s+/g, '');
            tfcLicenseIdText.text = key;
        }
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
            anchors.leftMargin: 10;
            anchors.verticalCenter: editLicenseDialogTopPanel.verticalCenter;

            width: 20;
            height: 20;

            sourceSize.height: height;
            sourceSize.width: width;

            source: "../../../" + "Icons/" + Style.theme + "/LisaIcon.svg";
        }

        Text {
            id: titleEditFeatureDialog;

            anchors.left: iconEditLicenseDialog.right;
            anchors.leftMargin: 10;
            anchors.verticalCenter: editLicenseDialogTopPanel.verticalCenter;
            anchors.horizontalCenter: editLicenseDialogTopPanel.horizontalCenter;

            text: qsTr("Edit License");
            color: Style.textColor;

            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_subtitle;
        }

        AuxButton {
            id: editLicenseDialogCloseButton;

            anchors.right: editLicenseDialogTopPanel.right;
            anchors.verticalCenter: editLicenseDialogTopPanel.verticalCenter;
            anchors.rightMargin: 15;

            width: 20;
            height: 20;

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
            border.color: Style.borderColor;

            onFocusChanged: {
                console.log("Rectangle tfcLicenseName onFocusChanged", tfcLicenseName.focus);

                if (tfcLicenseName.focus){
                    tfcLicenseNameText.setFocus(true);
                }
            }

            CustomTextField {
                id: tfcLicenseNameText;

                anchors.horizontalCenter: tfcLicenseName.horizontalCenter;
                anchors.verticalCenter: tfcLicenseName.verticalCenter;

                width: tfcLicenseName.width - 22;
                height: 30;

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

                KeyNavigation.tab: tfcLicenseId;
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
            border.color: Style.borderColor;

            onFocusChanged: {
                console.log("Rectangle tfcLicenseId onFocusChanged");

                if (tfcLicenseId.focus){
                    tfcLicenseIdText.setFocus(true);
                }
            }

            CustomTextField {
                id: tfcLicenseIdText;

                anchors.horizontalCenter: tfcLicenseId.horizontalCenter;
                anchors.verticalCenter: tfcLicenseId.verticalCenter;

                width: tfcLicenseId.width - 22;
                height: 30;

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

                onTextInputFocusChanged: {
                    editLicenseContainer.generateKey();
                }

                KeyNavigation.tab: okButton;
                KeyNavigation.backtab: tfcLicenseName;
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

            textButton: qsTr("OK");
            borderColor: okButton.highlighted ? Style.iconColorOnSelected :
                                              okButton.enabled ? Style.buttonColor :
                                                                 Style.imagingToolsGradient2;
            backgroundColor: okButton.enabled ? Style.imagingToolsGradient1 : Style.backgroundColor;

            enabled: errorIdMessage.text === "" && errorNameMessage.text === "";

            onClicked: {
                editLicenseContainer.exit("ok");
                editLicenseContainer.loaderDialog.closeItem();
            }

            KeyNavigation.tab: cancelButton;
            KeyNavigation.backtab: tfcLicenseId;

            onFocusChanged: {
                console.log("EditLicenseDialog okButton onFocusChanged");

                okButton.highlighted  = okButton.focus;
            }

            Keys.onPressed: {
                console.log("AuxButton okButton Key pressed!")
                if(event.key === Qt.Key_Return){
                    okButton.clicked();
                }
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

            textButton: qsTr("Cancel");
            borderColor: cancelButton.highlighted ? Style.iconColorOnSelected : Style.buttonColor;
            backgroundColor: Style.imagingToolsGradient1;

            onClicked: {
                editLicenseContainer.exit("cancel");
                editLicenseContainer.loaderDialog.closeItem();
            }

            onFocusChanged: {
                console.log("EditFeatureDialog cancelButton onFocusChanged");
                cancelButton.highlighted = cancelButton.focus;
            }

            Keys.onPressed: {
                console.log("AuxButton cancelButton Key pressed!")
                if(event.key === Qt.Key_Return) {
                    cancelButton.clicked();
                }
             }

            KeyNavigation.tab: tfcLicenseName;
            KeyNavigation.backtab: okButton;
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

            text: qsTr("License-ID can't be empty!");
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
