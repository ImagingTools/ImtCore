import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0


Rectangle {
    id: container;
    anchors.centerIn: parent;

    width: 350;
    height: 150;

    color: Style.backgroundColor;
    clip: true;

    radius: 5;

    property string message;
    property string nameDialog;

    property bool okButtonVisible: true;
    property bool cancelButtonVisible: true;

    property Item resultItem;

    signal okButtonClicked();
    signal cancelButtonClicked();

    function exit(status, value) {
        var parameters  = {};
        parameters["status"] = status;
        parameters["value"] = value;
        container.resultItem.dialogResult(parameters);
    }

    Rectangle {
        id: inputDialogTopPanel;
        width: container.width;
        height: 40;
        border.color: container.color;
        color: Style.baseColor;

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

        Text {
            id: mainTextRemoveDialog;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            text: container.message;
        }


        TextFieldCustom {
            id: tfcInputDialog;
            anchors.top: mainTextRemoveDialog.bottom;
            anchors.topMargin: 5;
            anchors.horizontalCenter: dialogBody.horizontalCenter;

            width: dialogBody.width - 20;
            height: 23;
        }

        Rectangle {
            id: okButton;
            anchors.bottom: dialogBody.bottom;
            anchors.bottomMargin: 20;
            anchors.right: cancelButton.left;
            anchors.rightMargin: 15;
            width: 70;
            height: 25;
            color: Style.backgroundColor;
            border.color: okButtonMa.containsMouse ? Style.iconColorOnSelected : Style.theme === "Light" ? "#d0d0d2" : "#3a3b3b" ;
            visible: container.okButtonVisible;

            Text {
                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.verticalCenter: parent.verticalCenter;
                color: Style.textColor;
                text: qsTr("OK");
                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;
            }

            MouseArea {
                id: okButtonMa;
                anchors.fill: parent;
                hoverEnabled: true;
                onClicked: {
                    container.exit("ok", tfcInputDialog.text);
                    container.okButtonClicked();
                    loaderDialog.closeItem();
                }
            }
        }

        Rectangle {
            id: cancelButton;
            width: 70;
            height: 25;
            color: Style.backgroundColor;
            border.color: cancelButtonMa.containsMouse ? Style.iconColorOnSelected : Style.theme === "Light" ? "#d0d0d2" : "#3a3b3b" ;
            anchors.bottom: dialogBody.bottom;
            anchors.right: dialogBody.right;
            anchors.bottomMargin: 20;
            anchors.rightMargin: 15;

            visible: container.cancelButtonVisible;

            Text {
                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.verticalCenter: parent.verticalCenter;
                color: Style.textColor;
                text: qsTr("Cancel");
                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;
            }

            MouseArea {
                id: cancelButtonMa;
                anchors.fill: parent;
                hoverEnabled: true;

                onClicked: {
                    container.exit("cancel", "");
                    container.cancelButtonClicked();
                    loaderDialog.closeItem();
                }

            }
        }
    }
}
