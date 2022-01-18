import QtQuick 2.0
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
//import '../../../imtgui/Qml/imtgui/AuxComponents'

Rectangle {
    id: container;
    width: 400;
    height: 300;
    radius: 10;
    color: Style.backgroundColor;
    clip: true;
    property string featureId;
    property string featureName;
    signal okClicked(string newId, string newName);
    signal cancelClicked();


    function generateKey() {
        console.log("EditFeatureDialog generateKey...");
        if (tfcFeatureIdText.text === "" && tfcFeatureNameText.text !== "") {
            var key = "#" + tfcFeatureNameText.text;
            key = key.replace(/\s+/g, '');
            tfcFeatureIdText.text = key;
            console.log("EditFeatureDialog key has been generated!");
        }
    }

    Rectangle {
        id: editFeatureDialogTopPanel;
        width: container.width;
        height: 40;
        border.color: container.color;
        color: Style.baseColor;

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
            anchors.horizontalCenter: editFeatureDialogTopPanel.horizontalCenter;
            text: "Edit feature";
            //color: Style.theme == "Dark"? "black": Style.textColor;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        AuxButton {
            id: editFeatureDialogCloseButton;
            anchors.right: editFeatureDialogTopPanel.right;
            anchors.verticalCenter: editFeatureDialogTopPanel.verticalCenter;
            anchors.rightMargin: 15;
            width: 15;
            height: 15;
            iconSource: "../../../" + "Icons/" + "Light" + "/" + "Close" + "_" + "On" + "_" + "Normal" + ".svg";
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
        height: container.height - 100;

        Text {
            id: titleFeatureName;
            text: qsTr("Feature Name");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        Rectangle {
            id: tfcFeatureName;
            width: editFeatureDialogBody.width;
            height: 45;
            anchors.top: titleFeatureName.bottom;
            color: Style.imagingToolsGradient1;
            border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;
            TextFieldCustom {
                id: tfcFeatureNameText;
                width: tfcFeatureName.width - 22;
                height: 23;
                text: container.featureName;
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
            font.pixelSize: Style.fontSize_common;
        }

        Rectangle {
            id: tfcFeatureId;
            width: editFeatureDialogBody.width;
            height: 45;
            anchors.top: titleFeatureId.bottom;
            color: Style.imagingToolsGradient1;
            border.color: Style.theme == "Light" ? "#d0d0d2" : "#3a3b3b" ;

            TextFieldCustom {
                id: tfcFeatureIdText;
                width: tfcFeatureId.width - 22;
                height: 23;
                text: container.featureId;
                anchors.horizontalCenter: tfcFeatureId.horizontalCenter;
                anchors.verticalCenter: tfcFeatureId.verticalCenter;

                onFocusChanged: {
                    container.generateKey();
                }
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
//                    if (tfcFeatureNameText.text !== container.featureName || tfcFeatureIdText.text !== container.featureId) {
//                        container.okClicked(tfcFeatureIdText.text, tfcFeatureNameText.text);
//                    }
                    container.generateKey();
                    container.okClicked(tfcFeatureIdText.text, tfcFeatureNameText.text);
                    editFeatureDialog.visible = false;
                }
            }
        }

        Rectangle {
            id: cancelButton;
            width: 70;
            height: 25;
            color: Style.backgroundColor;
            border.color: cancelButtonMa.containsMouse ? Style.iconColorOnSelected : Style.theme === "Light" ? "#d0d0d2" : "#3a3b3b" ;
            anchors.top: tfcFeatureId.bottom;
            anchors.topMargin: 30;
            anchors.right: tfcFeatureId.right;


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
                    editFeatureDialog.visible = false;
                }
            }
        }
    }
}
