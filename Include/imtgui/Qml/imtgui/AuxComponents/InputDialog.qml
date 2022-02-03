import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0


Rectangle {
    id: container;
//    anchors.centerIn: parent;

    width: 300;
    height: 160;

    color: Style.backgroundColor;
    clip: true;
    focus: true;

    radius: 2;

    property string message;
    property string nameDialog;
    property string typeOperation;

    property string startingValue;

    property bool okButtonVisible: true;
    property bool cancelButtonVisible: true;
    property bool centered: true;

    property real backgroundOpacity: 0.4;
    property bool backgroundExist: true;
    property bool clickBackgroundClose: false;

    property Item resultItem;

    signal okButtonClicked();
    signal cancelButtonClicked();

    function exit(status, value) {
        var parameters  = {};
        parameters["status"] = status;
        parameters["value"] = value;
        parameters["typeOperation"] = container.typeOperation;
        parameters["dialog"] = "InputDialog";
        container.resultItem.dialogResult(parameters);
    }

    Component.onCompleted: {
        tfcInputDialog.textField.focus = true;
        tfcInputDialog.textField.cursorVisible = true;
    }

    MouseArea {
        anchors.fill: parent;
    }

    Rectangle {
        id: inputDialogTopPanel;
        width: container.width;
        height: 40;
        border.color: container.color;
        color: Style.baseColor;
        radius: container.radius;

        Image {
            id: iconDialog;
            width: 15;
            height: 15;
            anchors.left: inputDialogTopPanel.left;
            anchors.verticalCenter: inputDialogTopPanel.verticalCenter;
            source: "../../../" + "Icons/" + Style.theme + "/" + "Icon" + "_" + "On" + "_" + "Normal" + ".svg";
        }

        Text {
            id: titleRemoveDialog;
            anchors.left: iconDialog.right;
            anchors.leftMargin: 10;
            anchors.verticalCenter: inputDialogTopPanel.verticalCenter;
//            text: "Remove";
            text: container.nameDialog;
//            color: Style.theme == "Dark"? "black": Style.textColor;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        AuxButton {
            id: dialogCloseButton;
            anchors.right: inputDialogTopPanel.right;
            anchors.verticalCenter: inputDialogTopPanel.verticalCenter;
            anchors.rightMargin: 15;
            width: 15;
            height: 15;
            iconSource: "../../../" + "Icons/" + Style.theme + "/" + "Close" + "_" + "On" + "_" + "Normal" + ".svg";
           MouseArea {
               anchors.fill: parent;
               onClicked: {
                   container.exit("close");
                   loaderDialog.closeItem();
               }
           }
        }
    }

    Rectangle {
        id: dialogBody;
        color: container.color;
        anchors.top: inputDialogTopPanel.bottom;
        width: container.width;
        height: container.height - inputDialogTopPanel.height;

        Rectangle {
            id: rectText;
            height: 30;
            width: dialogBody.width - 20;
            anchors.horizontalCenter: dialogBody.horizontalCenter;
//            anchors.topMargin: 10;
            color: "transparent";
            Text {
                id: mainTextRemoveDialog;
                anchors.fill: parent;
                anchors.verticalCenter: dialogBody.verticalCenter;
                anchors.topMargin: 10;
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
                text: container.message;
            }
        }

        TextFieldCustom {
            id: tfcInputDialog;
            anchors.top: rectText.bottom;
            anchors.topMargin: 5;
            anchors.horizontalCenter: dialogBody.horizontalCenter;
            text: container.startingValue;

            width: dialogBody.width - 20;
            height: 30;
        }

        AuxButton {
            id: okButton;

            anchors.bottom: dialogBody.bottom;
            anchors.bottomMargin: 10;
            anchors.right: cancelButton.left;
            anchors.rightMargin: 10;

            width: 70;
            height: 25;

            hasText: true;
            hasIcon: false;

            textButton: "OK";
            borderColor: okButton.highlighted ? Style.iconColorOnSelected : Style.buttonColor;
            backgroundColor: Style.imagingToolsGradient1;

            //enabled: errorIdMessage.text === "" && errorNameMessage.text === "";

            visible: container.okButtonVisible;

            onClicked: {
                container.exit("ok", tfcInputDialog.text);
                container.okButtonClicked();
                loaderDialog.closeItem();
            }
        }

        AuxButton {
            id: cancelButton;

            anchors.bottom: dialogBody.bottom;
            anchors.right: dialogBody.right;
            anchors.bottomMargin: 10;
            anchors.rightMargin: 10;

            width: 70;
            height: 25;

            hasText: true;
            hasIcon: false;

            textButton: "Cancel";
            borderColor: cancelButton.highlighted ? Style.iconColorOnSelected : Style.buttonColor;
            backgroundColor: Style.imagingToolsGradient1;

            onClicked: {
                container.exit("cancel", "");
                container.cancelButtonClicked();
                loaderDialog.closeItem();
            }
        }
    }
}
