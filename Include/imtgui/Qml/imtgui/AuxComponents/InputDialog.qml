import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0


Rectangle {
    id: inputDialogContainer;

    width: 300;
    height: 160;

    color: Style.backgroundColor;
    clip: true;

    radius: 2;

    property string message;
    property string nameDialog;
    property string typeOperation;

    property string startingValue;

    property bool okButtonVisible: true;
    property bool cancelButtonVisible: true;
    property bool centered: true;

    property real backgroundOpacity: 0.3;
    property bool backgroundExist: true;
    property bool clickBackgroundClose: false;

    property Item resultItem;
    property Item loaderDialog;
    property Item thumbnailItem;

    signal okButtonClicked();
    signal cancelButtonClicked();

    Component.onCompleted: {
        console.log("InputDialog onCompleted");
        //inputDialogContainer.forceActiveFocus();
    }

    Keys.onPressed: {
        console.log("InputDialog keys pressed")
        if (event.key === Qt.Key_Escape){
            console.log('Key tab was pressed');
            cancelButton.clicked();
        }
        else if (event.key === Qt.Key_Return){
            okButton.clicked();
        }
    }

    onFocusChanged: {
        console.log("InputDialog onFocusChanged", inputDialogContainer.focus);

        if (inputDialogContainer.focus){
//            tfcInputDialog.setFocus(true);
            rectTfcInputDialog.forceActiveFocus();
        }
    }

    function exit(status, value) {
        var parameters  = {};
        parameters["status"] = status;
        parameters["value"] = value;
        parameters["typeOperation"] = inputDialogContainer.typeOperation;
        parameters["dialog"] = "InputDialog";
        inputDialogContainer.resultItem.dialogResult(parameters);
    }

    MouseArea {
        anchors.fill: parent;
    }

    Rectangle {
        id: inputDialogTopPanel;

        width: inputDialogContainer.width;
        height: 40;

        border.width: 1;
        border.color: inputDialogContainer.color;

        color: Style.baseColor;
        radius: inputDialogContainer.radius;

        Image {
            id: iconDialog;

            anchors.left: inputDialogTopPanel.left;
            anchors.leftMargin: 10;
            anchors.verticalCenter: inputDialogTopPanel.verticalCenter;

            width: 20;
            height: 20;

            sourceSize.height: height;
            sourceSize.width: width;

            source: "../../../" + "Icons/" + Style.theme + "/LisaIcon.svg";
        }

        Text {
            id: titleRemoveDialog;

            anchors.left: iconDialog.right;
            anchors.leftMargin: 10;
            anchors.verticalCenter: inputDialogTopPanel.verticalCenter;

            text: inputDialogContainer.nameDialog;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_subtitle;
        }

        AuxButton {
            id: dialogCloseButton;

            anchors.right: inputDialogTopPanel.right;
            anchors.verticalCenter: inputDialogTopPanel.verticalCenter;
            anchors.rightMargin: 15;

            width: 20;
            height: 20;

            iconSource: "../../../" + "Icons/" + Style.theme + "/" + "Close" + "_" + "On" + "_" + "Normal" + ".svg";

            onClicked: {
                inputDialogContainer.exit("close");
                loaderDialog.closeItem();
            }
        }
    }

    Rectangle {
        id: dialogBody;

        anchors.top: inputDialogTopPanel.bottom;

        width: inputDialogContainer.width;
        height: inputDialogContainer.height - inputDialogTopPanel.height;

        color: inputDialogContainer.color;

        Rectangle {
            id: rectText;

            anchors.horizontalCenter: dialogBody.horizontalCenter;

            height: 30;
            width: dialogBody.width - 20;

            color: "transparent";

            Text {
                id: mainTextRemoveDialog;

                anchors.fill: parent;
                anchors.verticalCenter: dialogBody.verticalCenter;
                anchors.topMargin: 10;

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
                text: inputDialogContainer.message;
            }
        }

        Rectangle {
            id: rectTfcInputDialog;

            anchors.top: rectText.bottom;
            anchors.topMargin: 5;
            anchors.horizontalCenter: dialogBody.horizontalCenter;

            width: dialogBody.width - 20;
            height: 30;

            TextFieldCustom {
                id: tfcInputDialog;

                anchors.fill: parent;

                text: inputDialogContainer.startingValue;
            }

            onFocusChanged: {
                if (rectTfcInputDialog.focus){
                    tfcInputDialog.setFocus(true);
                }
            }

            KeyNavigation.tab: okButton;
            KeyNavigation.backtab: cancelButton;
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

            textButton: qsTr("OK");
            borderColor: okButton.highlighted ? Style.iconColorOnSelected : Style.buttonColor;
            backgroundColor: Style.imagingToolsGradient1;

            visible: inputDialogContainer.okButtonVisible;

            onClicked: {
                inputDialogContainer.exit("ok", tfcInputDialog.text);
                inputDialogContainer.okButtonClicked();
                loaderDialog.closeItem();
            }

            KeyNavigation.tab: cancelButton;
            KeyNavigation.backtab: rectTfcInputDialog;
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

            textButton: qsTr("Cancel");
            borderColor: cancelButton.highlighted ? Style.iconColorOnSelected : Style.buttonColor;
            backgroundColor: Style.imagingToolsGradient1;

            onClicked: {
                inputDialogContainer.exit("cancel", "");
                inputDialogContainer.cancelButtonClicked();
                loaderDialog.closeItem();
            }
            KeyNavigation.tab: rectTfcInputDialog;
            KeyNavigation.backtab: okButton;
        }
    }
}
