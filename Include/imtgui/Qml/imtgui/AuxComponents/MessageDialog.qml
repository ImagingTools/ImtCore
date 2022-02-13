import QtQuick 2.12
import Acf 1.0
import imtqml 1.0


Rectangle {
    id: container;
    anchors.centerIn: parent;

    width: 350;
    height: 150;

    color: Style.backgroundColor;
    clip: true;

    property string message;
    property string nameDialog;

    property string textOkButton: "Yes";

    property bool okButtonVisible: true;
    property bool noButtonVisible: true;
    property bool backgroundExist: true;
    property bool clickBackgroundClose: false;

    property real backgroundOpacity: 0.4;
    property bool centered: true;
    property Item resultItem;
    property Item loaderDialog;

    signal okButtonClicked();
    signal noButtonClicked();

    function exit(status) {
        var parameters  = {};
        parameters["status"] = status;
        parameters["dialog"] = nameDialog;
        container.resultItem.dialogResult(parameters);
    }

    MouseArea {
        anchors.fill: parent;
    }

    Rectangle {
        id: removeDialogTopPanel;
        width: container.width;
        height: 40;
        border.color: container.color;
        color: Style.baseColor;

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
//            text: "Remove";
            text: container.nameDialog;
//            color: Style.theme == "Dark"? "black": Style.textColor;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        AuxButton {
            id: removeDialogCloseButton;
            anchors.right: removeDialogTopPanel.right;
            anchors.verticalCenter: removeDialogTopPanel.verticalCenter;
            anchors.rightMargin: 15;
            width: 17;
            height: 17;
            iconSource: "../../../" + "Icons/" + Style.theme + "/" + "Close" + "_" + "On" + "_" + "Normal" + ".svg";

           onClicked: {
               container.exit("close");
               loaderDialog.closeItem();
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
            font.pixelSize: Style.fontSize_common;
            text: container.message;

            anchors.left: iconQuestion.right;
            anchors.leftMargin: 10;
            anchors.top: removeDialogBody.top;
            anchors.topMargin: 15;
        }

        AuxButton {
            id: yesButton;

            anchors.bottom: removeDialogBody.bottom;
            anchors.bottomMargin: 20;
            anchors.right: noButton.left;
            anchors.rightMargin: 15;

            width: 70;
            height: 25;

            hasText: true;
            hasIcon: false;

            textButton: container.textOkButton;
            borderColor: yesButton.highlighted ? Style.iconColorOnSelected : Style.buttonColor;
            backgroundColor: Style.imagingToolsGradient1;

            visible: container.okButtonVisible;

            onClicked: {
                container.exit(container.textOkButton.toLowerCase());
                container.okButtonClicked();
                loaderDialog.closeItem();
            }
        }

        AuxButton {
            id: noButton;

            anchors.bottom: removeDialogBody.bottom;
            anchors.right: removeDialogBody.right;
            anchors.bottomMargin: 20;
            anchors.rightMargin: 15;

            width: 70;
            height: 25;

            hasText: true;
            hasIcon: false;

            textButton: "No";
            borderColor: noButton.highlighted ? Style.iconColorOnSelected : Style.buttonColor;
            backgroundColor: Style.imagingToolsGradient1;

            visible: container.noButtonVisible;

            onClicked: {
                container.exit("no");
                container.noButtonClicked();
                loaderDialog.closeItem();
            }
        }
    }
}
