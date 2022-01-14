import QtQuick 2.0
import Acf 1.0
import imtqml 1.0
//import imtgui 1.0
import imtauthgui 1.0
import '../../../imtgui/Qml/imtgui/AuxComponents'

Rectangle {
    id: container;
    width: 400;
    height: 300;
    radius: 10;
    color: Style.backgroundColor;
    clip: true;
    Rectangle {
        id: editLicenseDialogTopPanel;
        width: container.width;
        height: 40;
        color: "white";

        Image {
            id: iconEditLicenseDialog;
            width: 15;
            height: 15;
            anchors.left: editLicenseDialogTopPanel.left;
            anchors.verticalCenter: editLicenseDialogTopPanel.verticalCenter;
            source: "../../../" + "Icons/" + Style.theme + "/" + "Icon" + "_" + "On" + "_" + "Normal" + ".svg";
        }

        Text {
            id: titleEditFeatureDialog;
            anchors.left: iconEditLicenseDialog.right;
            anchors.leftMargin: 10;
            anchors.verticalCenter: editLicenseDialogTopPanel.verticalCenter;
            anchors.horizontalCenter: editLicenseDialogTopPanel.horizontalCenter;
            text: "Edit License";
            color: Style.theme == "Dark"? "black": Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_subtitle;
        }

        AuxButton {
            id: editLicenseDialogCloseButton;
            anchors.right: editLicenseDialogTopPanel.right;
            anchors.verticalCenter: editLicenseDialogTopPanel.verticalCenter;
            anchors.rightMargin: 15;
            width: 15;
            height: 15;
            iconSource: "../../../" + "Icons/" + "Light" + "/" + "Close" + "_" + "On" + "_" + "Normal" + ".svg";
           MouseArea {
               anchors.fill: parent;
               onClicked: {
                   editLicenseDialog.visible = false;
               }
           }
        }
    }


    Rectangle {
        id: editLicenseDialogBody;
        anchors.topMargin: 40;
        color: container.color;
        anchors.top: editLicenseDialogTopPanel.bottom;
        anchors.horizontalCenter: container.horizontalCenter;
        anchors.verticalCenter: container.verticalCenter;
        width: container.width - 50;
        height: container.height - 100;

        Text {
            id: titleLicenseName;
            text: qsTr("License Name");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        Rectangle {
            id: tfcLicenseName;
            width: editLicenseDialogBody.width;
            height: 45;
            anchors.top: titleLicenseName.bottom;
            color: Style.imagingToolsGradient1;
            border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
            TextFieldCustom {
                width: tfcLicenseName.width - 22;
                height: 23;
                anchors.horizontalCenter: tfcLicenseName.horizontalCenter;
                anchors.verticalCenter: tfcLicenseName.verticalCenter;
            }
        }


        Text {
            id: titleLicenseId;
            anchors.top: tfcLicenseName.bottom;
            anchors.topMargin: 20;
            text: qsTr("License - ID");

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        Rectangle {
            id: tfcLicenseId;
            width: editLicenseDialogBody.width;
            height: 45;
            anchors.top: titleLicenseId.bottom;
            color: Style.imagingToolsGradient1;
            border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

            TextFieldCustom {
                width: tfcLicenseId.width - 22;
                height: 23;
                anchors.horizontalCenter: tfcLicenseId.horizontalCenter;
                anchors.verticalCenter: tfcLicenseId.verticalCenter;
            }
        }

        Rectangle {
            id: okButton;
            anchors.top: tfcLicenseId.bottom;
            anchors.right: cancelButton.left;
            anchors.rightMargin: 15;
            anchors.topMargin: 30;
            width: 70;
            height: 25;
            color: Style.backgroundColor;
            border.color: okButtonMa.containsMouse ? Style.iconColorOnSelected : Style.theme === "Light" ? "#d0d0d2" : "#3a3b3b" ;

            Text {
                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.verticalCenter: parent.verticalCenter;
                color: Style.textColor;
                text: qsTr("ОК");
                font.family: Style.fontFamily;
            }

            MouseArea {
                id: okButtonMa;
                anchors.fill: parent;
                hoverEnabled: enabled;
                cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;
                onClicked: {
                    editLicenseDialog.visible = false;
                }
            }
        }

        Rectangle {
            id: cancelButton;
            width: 70;
            height: 25;
            color: Style.backgroundColor;
            border.color: cancelButtonMa.containsMouse ? Style.iconColorOnSelected : Style.theme === "Light" ? "#d0d0d2" : "#3a3b3b" ;
            anchors.top: tfcLicenseId.bottom;
           // anchors.bottomMargin: 15;
            anchors.topMargin: 30;
            anchors.right: tfcLicenseId.right;


            Text {
                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.verticalCenter: parent.verticalCenter;
                color: Style.textColor;
                font.family: Style.fontFamily;
                text: qsTr("Cancel");
            }

            MouseArea {
                id: cancelButtonMa;
                anchors.fill: parent;
                hoverEnabled: enabled;
                cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

                onClicked: {
                    editLicenseDialog.visible = false;
                }

            }
        }
    }
}
