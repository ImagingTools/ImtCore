import QtQuick 2.12
import Acf 1.0
import imtqml 1.0


Rectangle {
    id: container;
    width: 400;
    height: 150;

    color: Style.backgroundColor;
    clip: true;
    Rectangle {
        id: removeDialogTopPanel;
        width: container.width;
        height: 40;
        color: "white";

        Image {
            id: iconRemoveDialog;
            width: 15;
            height: 15;
            anchors.left: removeDialogTopPanel.left;
            anchors.verticalCenter: removeDialogTopPanel.verticalCenter;
            source: "../../../" + "Icons/" + Style.theme + "/" + "Icon" + "_" + "On" + "_" + "Normal" + ".svg";
        }

        Text {
            id: titleRemoveDialog;
            anchors.left: iconRemoveDialog.right;
            anchors.leftMargin: 10;
            anchors.verticalCenter: removeDialogTopPanel.verticalCenter;
            text: "Remove";
//            color: Style.theme == "Dark"? "black": Style.textColor;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_subtitle;
        }

        AuxButton {
            id: removeDialogCloseButton;
            anchors.right: removeDialogTopPanel.right;
            anchors.verticalCenter: removeDialogTopPanel.verticalCenter;
            anchors.rightMargin: 15;
            width: 15;
            height: 15;
            iconSource: "../../../" + "Icons/" + Style.theme + "/" + "Close" + "_" + "On" + "_" + "Normal" + ".svg";
           MouseArea {
               anchors.fill: parent;
               onClicked: {
                   removeDialog.visible = false;
               }
           }
        }
    }

    Rectangle {
        id: removeDialogBody;
        color: container.color;
        anchors.top: removeDialogTopPanel.bottom;
        width: container.width;
        height: container.height - removeDialogTopPanel.height;

        Image {
            id: iconQuestion;
            width: 30;
            height: width;
            anchors.left: removeDialogBody.left;
            anchors.leftMargin: 15;
            anchors.top: removeDialogBody.top;
            anchors.topMargin: 15;
            source: "../../../" + "Icons/" + Style.theme + "/" + "Question" + "_" + "On" + "_" + "Normal" + ".svg";
        }

        Text {
            id: mainTextRemoveDialog;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_subtitle;
            text: qsTr("Remove selected file from the database ?");

            anchors.left: iconQuestion.right;
            anchors.leftMargin: 10;
            anchors.top: removeDialogBody.top;
            anchors.topMargin: 15;
        }

        Rectangle {
            id: yesButton;
            anchors.bottom: removeDialogBody.bottom;
            anchors.bottomMargin: 20;
            anchors.right: noButton.left;
            anchors.rightMargin: 15;
            width: 70;
            height: 25;
            color: Style.baseColor;
            border.color: "gray";

            Text {
                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.verticalCenter: parent.verticalCenter;
                color: Style.textColor;
                text: qsTr("Yes");
            }

            MouseArea {
                id: yesButtonMa;
                anchors.fill: parent;
                onClicked: {
                    //editFeatureDialog.visible = false;
                }
            }
        }

        Rectangle {
            id: noButton;
            width: 70;
            height: 25;
            color: Style.baseColor;
            border.color: "gray";
            anchors.bottom: removeDialogBody.bottom;
            anchors.right: removeDialogBody.right;
            anchors.bottomMargin: 20;
            anchors.rightMargin: 15;

            Text {
                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.verticalCenter: parent.verticalCenter;
                color: Style.textColor;
                text: qsTr("No");
            }

            MouseArea {
                id: noButtonMa;
                anchors.fill: parent;
                hoverEnabled: true;

                onClicked: {
                   // editFeatureDialog.visible = false;
                }

            }
        }
    }
}
