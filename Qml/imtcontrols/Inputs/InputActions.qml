import QtQuick 2.12
import Acf 1.0;
import imtcontrols 1.0

Item {
    id: inputActions;

    property bool canCopy: true;
    property bool canCut: true;
    property bool canPaste: true;
    property bool canRemove: true;
    property bool canSelectAll: true;

    signal copyClicked();
    signal cutClicked();
    signal pasteClicked();
    signal removeClicked();
    signal selectAllClicked();

    ListModel {
        id: contextMenuModel;
    }

    function open(x, y){
        if (!x){
            x = 0
        }

        if (!y){
            y = 0
        }
//        ModalDialogManager.closeDialog();
        contextMenuModel.clear();

        if (inputActions.canCut){
            contextMenuModel.append({"id": "Cut", "name": qsTr("Cut"), "IconSource": ""});
        }

        if (inputActions.canCopy){
            contextMenuModel.append({"id": "Copy", "name": qsTr("Copy"), "IconSource": ""});
        }

        if (inputActions.canPaste){
            contextMenuModel.append({"id": "Paste", "name": qsTr("Paste"), "IconSource": ""});
        }

        if (inputActions.canRemove){
            contextMenuModel.append({"id": "Remove", "name": qsTr("Remove"), "IconSource": ""});
        }

        if (inputActions.canSelectAll){
            contextMenuModel.append({"id": "SelectAll", "name": qsTr("Select All"), "IconSource": ""});
        }

        ModalDialogManager.openDialog(popupMenu, {"x": x, "y": y, "model": contextMenuModel});
    }

    Component {
        id: popupMenu;

        PopupMenuDialog {
            id: popupMenuDialog;

            onFinished: {
                if (commandId === "Cut"){
                    inputActions.cutClicked();
                }
                else if (commandId === "Copy"){
                    inputActions.copyClicked();
                }
                else if (commandId === "Paste"){
                    inputActions.pasteClicked();
                }
                else if (commandId === "Remove"){
                    inputActions.removeClicked();
                }
                else if (commandId === "SelectAll"){
                    inputActions.selectAllClicked();
                }
            }

            function onBackgroundClicked(){
                popupMenuDialog.root.closeDialog();
            }
        }
    }
}
