import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0


Rectangle {
    id: dialogContainer;

    z: 10;
//    anchors.centerIn: dialogContainer.root;
    anchors.centerIn: background;

    width: 300;
    height: dialogColumn.height;

    color: Style.backgroundColor;

    visible: false;

    property Item root;
    property Item backgroundItem: background;

    signal finished(string buttonId);

    Component.onCompleted: {
        console.log("Dialog onCompleted", dialogContainer);
        console.log("background", background);
        console.log("thubnailDecoratorContainer", thubnailDecoratorContainer);
    }

    onHeightChanged: {
        console.log("dialogContainer onHeightChanged", dialogContainer.height);
    }

    property string bodySource: "MessageDialogBody.qml";

    function setTitle(title){
        topPanelDialog.title = title;
    }

    function addButton(buttonObj){
        buttonsDialog.addButton(buttonObj);
    }

    function getLoaderBodyItem(){
        return loaderBodyDialog.item;
    }

    function open(){
        dialogContainer.visible = true;
        dialogContainer.backgroundItem.z = 9;
        dialogContainer.backgroundItem.visible = dialogContainer.visible;
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
                dialogContainer.visible = false;
            }
        }

        Loader {
            id: loaderBodyDialog;

            onItemChanged: {
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
                dialogContainer.visible = false;
            }
        }
    }
}
