import QtQuick 2.0
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0

Rectangle {
    id: container;
//    border.color: "#2b2d2d";
    color: Style.backgroundColor;
    clip: true;
    Rectangle {
        id: editFeatureDialogTopPanel;
        width: container.width;
        height: 40;
        color: "white";

        Image {
            id: iconEditFeatureDialog;
            width: 15;
            height: 15;
            anchors.left: editFeatureDialogTopPanel.left;
            anchors.verticalCenter: editFeatureDialogTopPanel.verticalCenter;
            source: "../../../" + "Icons/" + Style.theme + "/" + "Icon" + "_" + "On" + "_" + "Normal" + ".svg";
        }

        Text {
            id: titleEditFeatureDialog;
            anchors.left: iconEditFeatureDialog.right;
            anchors.leftMargin: 10;
            anchors.verticalCenter: editFeatureDialogTopPanel.verticalCenter;
            text: "Edit feature";
            color: Style.theme == "Dark"? "black": Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_subtitle;
        }

        AuxButton {
            id: editFeatureDialogCloseButton;
            anchors.right: editFeatureDialogTopPanel.right;
            anchors.verticalCenter: editFeatureDialogTopPanel.verticalCenter;
            anchors.rightMargin: 15;
            width: 15;
            height: 15;
            iconSource: "../../../" + "Icons/" + Style.theme + "/" + "Close" + "_" + "On" + "_" + "Normal" + ".svg";
           MouseArea {
               anchors.fill: parent;
               onClicked: {
                   editFeatureDialog.visible = false;
               }
           }
        }
    }


    Rectangle {
        id: editFeatureDialogBody;
        anchors.topMargin: 40;
        color: container.color;
        anchors.top: editFeatureDialogTopPanel.bottom;
        anchors.horizontalCenter: container.horizontalCenter;
        anchors.verticalCenter: container.verticalCenter;
        width: container.width - 50;
        height: container.height - 40;

        Text {
            id: titleFeatureName;
            text: qsTr("Feature Name");
            color: Style.textColor;
            font.family: Style.fontFamily;
           // font.pixelSize: Style.fontSize_subtitle;
        }

        Rectangle {
            id: tfcFeatureName;
            width: editFeatureDialogBody.width;
            height: 60;
            anchors.top: titleFeatureName.bottom;
            color: Style.baseColor;
            border.color: "gray";
            TextFieldCustom {
                width: tfcFeatureName.width - 50;
                height: 30;
                anchors.horizontalCenter: tfcFeatureName.horizontalCenter;
                anchors.verticalCenter: tfcFeatureName.verticalCenter;
            }
        }


        Text {
            id: titleFeatureId;
            anchors.top: tfcFeatureName.bottom;
            anchors.topMargin: 20;
            text: qsTr("Feature - ID");

            color: Style.textColor;
            font.family: Style.fontFamily;
            //font.pixelSize: Style.fontSize_subtitle;
        }

        Rectangle {
            id: tfcFeatureId;
            width: editFeatureDialogBody.width;
            height: 60;
            anchors.top: titleFeatureId.bottom;
            color: Style.baseColor;
            border.color: "gray";

            TextFieldCustom {
                width: tfcFeatureId.width - 50;
                height: 30;
                anchors.horizontalCenter: tfcFeatureId.horizontalCenter;
                anchors.verticalCenter: tfcFeatureId.verticalCenter;
            }
        }

        Rectangle {
            id: okButton;
            anchors.top: tfcFeatureId.bottom;
            anchors.right: cancelButton.left;
            anchors.rightMargin: 15;
            anchors.topMargin: 30;
            width: 70;
            height: 25;
            color: Style.baseColor;
            border.color: "gray";

            Text {
//                anchors.fill: parent;
                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.verticalCenter: parent.verticalCenter;
                color: Style.textColor;
                text: qsTr("ОК");
            }

            MouseArea {
                id: okButtonMa;
                anchors.fill: parent;
                onClicked: {
                    editFeatureDialog.visible = false;
                }
            }
        }

        Rectangle {
            id: cancelButton;
            width: 70;
            height: 25;
            color: Style.baseColor;
            border.color: "gray";
            anchors.top: tfcFeatureId.bottom;
           // anchors.bottomMargin: 15;
            anchors.topMargin: 30;
            anchors.right: tfcFeatureId.right;
         //   anchors.rightMargin: 15;


            Text {
//                anchors.fill: parent;
                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.verticalCenter: parent.verticalCenter;
                color: Style.textColor;
                text: qsTr("Cancel");
            }

            MouseArea {
                id: cancelButtonMa;
                anchors.fill: parent;
                hoverEnabled: true;

                onClicked: {
                    editFeatureDialog.visible = false;
                }

//                onEntered: {
//                    console.log("ENTERED!");
//                    if (cancelButtonMa.mouseX <= 0 || cancelButtonMa.mouseX >= cancelButton.width
//                            || cancelButtonMa.mouseY <= 0 || cancelButtonMa.mouseY >= cancelButton.height)
//                    {
//                        cancelButton.border.color = "#1560BD";
//                    } else
//                    {
//                        cancelButton.border.color = "gray";
//                    }
//                }
            }
        }
    }
}
