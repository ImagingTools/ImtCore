import QtQuick 2.12
import Acf 1.0
import imtqml 1.0


Rectangle {
    id: messageDialogContainer;

//    anchors.centerIn: parent;

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
        parameters["dialog"] = messageDialogContainer.nameDialog;

        messageDialogContainer.resultItem.dialogResult(parameters);
    }

    MouseArea {
        anchors.fill: parent;
    }

    Rectangle {
        id: removeDialogTopPanel;

        width: messageDialogContainer.width;
        height: 40;

        border.color: messageDialogContainer.color;
        color: Style.baseColor;

        Image {
            id: iconRemoveDialog;

            anchors.left: removeDialogTopPanel.left;
            anchors.verticalCenter: removeDialogTopPanel.verticalCenter;

            width: 20;
            height: 20;

            source: "../../../" + "Icons/" + Style.theme + "/" + "Icon" + "_" + "On" + "_" + "Normal" + ".svg";
        }

        Text {
            id: titleRemoveDialog;

            anchors.left: iconRemoveDialog.right;
            anchors.leftMargin: 10;
            anchors.verticalCenter: removeDialogTopPanel.verticalCenter;

            text: messageDialogContainer.nameDialog;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_subtitle;
        }

        AuxButton {
            id: removeDialogCloseButton;

            anchors.right: removeDialogTopPanel.right;
            anchors.verticalCenter: removeDialogTopPanel.verticalCenter;
            anchors.rightMargin: 15;

            width: 20;
            height: 20;

            iconSource: "../../../" + "Icons/" + Style.theme + "/" + "Close" + "_" + "On" + "_" + "Normal" + ".svg";

            onClicked: {
                messageDialogContainer.exit("close");
                loaderDialog.closeItem();
            }
        }
    }

    Rectangle {
        id: removeDialogBody;

        anchors.top: removeDialogTopPanel.bottom;

        width: messageDialogContainer.width;
        height: messageDialogContainer.height - removeDialogTopPanel.height;

        color: messageDialogContainer.color;

        Image {
            id: iconQuestion;

            anchors.left: removeDialogBody.left;
            anchors.leftMargin: 15;
            anchors.top: removeDialogBody.top;
            anchors.topMargin: 15;

            width: 30;
            height: width;

            source: "../../../" + "Icons/" + Style.theme + "/" + "Question" + "_" + "On" + "_" + "Normal" + ".svg";
        }

        Text {
            id: mainTextRemoveDialog;

            anchors.left: iconQuestion.right;
            anchors.leftMargin: 10;
            anchors.top: removeDialogBody.top;
            anchors.topMargin: 15;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            text: messageDialogContainer.message;
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

            textButton: messageDialogContainer.textOkButton;
            borderColor: yesButton.highlighted ? Style.iconColorOnSelected : Style.buttonColor;
            backgroundColor: Style.imagingToolsGradient1;

            visible: messageDialogContainer.okButtonVisible;

            onClicked: {
                var result = messageDialogContainer.textOkButton.toLowerCase();
                messageDialogContainer.exit(result);
                messageDialogContainer.okButtonClicked();
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

            visible: messageDialogContainer.noButtonVisible;

            onClicked: {
                messageDialogContainer.exit("no");
                messageDialogContainer.noButtonClicked();
                loaderDialog.closeItem();
            }
        }
    }
}
