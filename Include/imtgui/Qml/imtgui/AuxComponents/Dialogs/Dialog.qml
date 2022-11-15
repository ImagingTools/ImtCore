import QtQuick 2.12
import Acf 1.0
import imtqml 1.0

Rectangle {
    id: dialogContainer;

    width: 300;
    height: dialogColumn.height;

    color: Style.backgroundColor;

    property string title;
    property string bodySource;

    property bool centered: true;
    property bool hasIcon: true;

    property Item root;
    property Item bodyItem: loaderBodyDialog.item;

    property alias buttons: buttonsDialog;

    property string notClosingButtons: "";

    signal finished(string buttonId);

    property Component content;

    onFocusChanged: {
        console.log("Dialog onFocusChanged", focus);

        if (focus){
            loaderBodyDialog.item.focus = true;
        }
    }

    onRootChanged: {
        root.backgroundItem.opacity = 0.4;
    }

    onFinished: {

        if (root){
            if(dialogContainer.notClosingButtons.indexOf(buttonId) == -1){
                root.closeDialog();
            }
        }
    }

    onTitleChanged: {
        console.log("Dialog onTitleChanged", title);
        topPanelDialog.title = title;
    }

    onBodySourceChanged: {
        loaderBodyDialog.source = dialogContainer.bodySource;
    }

    MouseArea {
        anchors.fill: parent;
        onClicked: {}
    }

    Column {
        id: dialogColumn;

        width: dialogContainer.width;

        TopPanelDialog {
            id: topPanelDialog;

            width: dialogContainer.width;
            height: 40;
            hasIcon: dialogContainer.hasIcon;

            onCloseButtonClicked: {
                dialogContainer.finished(buttonId);
            }
        }

        Loader {
            id: loaderBodyDialog;

            sourceComponent: content;

            onLoaded: {
                loaderBodyDialog.item.width = dialogContainer.width;
            }
        }

        ButtonsDialog {
            id: buttonsDialog;

            anchors.right: parent.right;
            anchors.rightMargin: 10;

            onButtonClicked: {
                console.log("ButtonsDialog onButtonClicked", buttonId);
                dialogContainer.finished(buttonId);
            }
        }
    }
}
