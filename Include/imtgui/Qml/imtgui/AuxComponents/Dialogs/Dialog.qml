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

    property Item root;
    property Item bodyItem: loaderBodyDialog.item;

    property alias buttons: buttonsDialog;

    signal finished(string buttonId);

    onFinished: {
        root.closeDialog();
    }

    onTitleChanged: {
        topPanelDialog.title = title;
    }

    onBodySourceChanged: {
        loaderBodyDialog.source = dialogContainer.bodySource;
    }

    Column {
        id: dialogColumn;

        width: dialogContainer.width;

        TopPanelDialog {
            id: topPanelDialog;

            width: dialogContainer.width;
            height: 40;

            onCloseButtonClicked: {
                dialogContainer.finished(buttonId);
            }
        }

        Loader {
            id: loaderBodyDialog;

            onItemChanged: {
                console.log("Dialog onItemChanged", loaderBodyDialog.item);
                if (loaderBodyDialog.item){
                    loaderBodyDialog.item.width = dialogContainer.width;
                }
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
