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
    radius: 2;
    color: Style.backgroundColor;
    clip: true;
    focus: true;

    property Item resultItem;
    property string licenseId;
    property string licenseName;
    property real backgroundOpacity: 0.4;
    signal okClicked(string newId, string newName);
    signal cancelClicked();
    property bool backgroundExist: true;
    property bool centered: true;

    function exit(status) {
        var parameters  = {};
        if (status === "ok") {

            parameters["newLicenseId"] = tfcLicenseIdText.text;
            parameters["newLicenseName"] = tfcLicenseNameText.text;

            parameters["dialog"] = "EditLicense";
        }
        parameters["status"] = status;
        container.resultItem.dialogResult(parameters);
    }

    MouseArea {
        anchors.fill: parent;
    }

    Rectangle {
        id: editLicenseDialogTopPanel;
        width: container.width;
        height: 40;
        border.color: container.color;
        color: Style.baseColor;

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
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        AuxButton {
            id: editLicenseDialogCloseButton;
            anchors.right: editLicenseDialogTopPanel.right;
            anchors.verticalCenter: editLicenseDialogTopPanel.verticalCenter;
            anchors.rightMargin: 15;
            width: 15;
            height: 15;
            iconSource: "../../../" + "Icons/" + "Light" + "/" + "Close" + "_" + "On" + "_" + "Normal" + ".svg";

            onClicked: {
                container.exit("close");
                loaderDialog.closeItem();
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
                id: tfcLicenseNameText;
                width: tfcLicenseName.width - 22;
                height: 23;
                anchors.horizontalCenter: tfcLicenseName.horizontalCenter;
                anchors.verticalCenter: tfcLicenseName.verticalCenter;

                text: container.licenseName;
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
                id: tfcLicenseIdText;
                width: tfcLicenseId.width - 22;
                height: 23;

                anchors.horizontalCenter: tfcLicenseId.horizontalCenter;
                anchors.verticalCenter: tfcLicenseId.verticalCenter;

                text: container.licenseId;
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
//                    editLicenseDialog.visible = false;
                    container.exit("ok");
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
                    container.exit("cancel");
                    loaderDialog.closeItem();
                }

            }
        }
    }
}
