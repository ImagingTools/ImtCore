import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtauthgui 1.0


Item {
    id: dialogContainer;

    width: 300;

    property string bodySource: "MessageDialogBody.qml";

    function setTitle(title){
        topPanelDialog.title = title;
    }

    function addButton(buttonObj){
        buttonsDialog.addButton(buttonObj);
    }

    onBodySourceChanged: {
        loaderBodyDialog.source = dialogContainer.bodySource;
    }

    TopPanelDialog {
        id: topPanelDialog;

        width: dialogContainer.width;
        height: 40;
    }

    Loader {
        id: loaderBodyDialog;

        anchors.top: topPanelDialog.bottom;

        onItemChanged: {
            if (loaderBodyDialog.item){
                loaderBodyDialog.item.width = dialogContainer.width;
            }
        }
    }

    ButtonsDialog {
        id: buttonsDialog;

        anchors.bottom: dialogContainer.bottom;
    }
}
