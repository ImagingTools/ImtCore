import QtQuick 2.12
import Acf 1.0
import imtqml 1.0


Rectangle {
    id: messageDialogContainer;

    width: 350;
    height: 150;

    color: Style.backgroundColor;
    clip: true;

    property string message;
    property string nameDialog;
    property string dialogId;

    property string textOkButton: qsTr("Yes");

    property bool okButtonVisible: true;
    property bool noButtonVisible: true;
    property bool cancelButtonVisible: false;

    property bool backgroundExist: true;
    property bool clickBackgroundClose: false;

    property real backgroundOpacity: 0.3;
    property bool centered: true;

    property Item resultItem;
    property Item loaderDialog;
    property Item thumbnailItem;

    signal okButtonClicked();
    signal noButtonClicked();

    Component.onCompleted: {
//        yesButton.forceActiveFocus();
    }

    onFocusChanged: {
        console.log("MessageDialog onFocusChanged", messageDialogContainer.focus);
        if (messageDialogContainer.focus){
            yesButton.forceActiveFocus();
        }
    }

    Keys.onPressed: {
        console.log("MessageDialog keys pressed")
        if (event.key === Qt.Key_Escape){
            console.log('Key tab was pressed');
            cancelButton.clicked();
        }
        else if (event.key === Qt.Key_Return){
            yesButton.clicked();
        }
    }

    function exit(status) {
        var parameters  = {};
        parameters["status"] = status;
        parameters["dialog"] = messageDialogContainer.dialogId;

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
            anchors.leftMargin: 10;
            anchors.verticalCenter: removeDialogTopPanel.verticalCenter;

            width: 20;
            height: 20;

            sourceSize.height: height;
            sourceSize.width: width;

            source: "../../../" + "Icons/" + Style.theme + "/LisaIcon.svg";
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

        Row {
            anchors.bottom: removeDialogBody.bottom;
            anchors.right: removeDialogBody.right;
            anchors.bottomMargin: 20;
            anchors.rightMargin: 15;

            spacing: 10;

            AuxButton {
                id: yesButton;

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
//                    messageDialogContainer.okButtonClicked();
                    loaderDialog.closeItem();
                }

                KeyNavigation.tab: noButton;
                KeyNavigation.backtab: cancelButton;
            }

            AuxButton {
                id: noButton;

                width: 70;
                height: 25;

                hasText: true;
                hasIcon: false;

                textButton: qsTr("No");
                borderColor: noButton.highlighted ? Style.iconColorOnSelected : Style.buttonColor;
                backgroundColor: Style.imagingToolsGradient1;

                visible: messageDialogContainer.noButtonVisible;

                onClicked: {
                    messageDialogContainer.exit("no");
                    //messageDialogContainer.noButtonClicked();
                    loaderDialog.closeItem();
                }

                KeyNavigation.tab: cancelButton;
                KeyNavigation.backtab: yesButton;
            }

            AuxButton {
                id: cancelButton;

                width: 70;
                height: 25;

                hasText: true;
                hasIcon: false;

                textButton: qsTr("Cancel");
                borderColor: cancelButton.highlighted ? Style.iconColorOnSelected : Style.buttonColor;
                backgroundColor: Style.imagingToolsGradient1;

                visible: messageDialogContainer.cancelButtonVisible;

                onClicked: {
                    messageDialogContainer.exit("cancel");
                    //messageDialogContainer.noButtonClicked();
                    loaderDialog.closeItem();
                }

                KeyNavigation.tab: yesButton;
                KeyNavigation.backtab: noButton;
            }
        }
    }
}
