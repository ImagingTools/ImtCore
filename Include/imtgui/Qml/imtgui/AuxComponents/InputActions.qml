import QtQuick 2.12
import Acf 1.0;
import imtgui 1.0

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
        modalDialogManager.closeDialog();
        contextMenuModel.clear();

        if (inputActions.canCut){
            contextMenuModel.append({"Id": "Cut", "Name": qsTr("Cut"), "IconSource": ""});
        }

        if (inputActions.canCopy){
            contextMenuModel.append({"Id": "Copy", "Name": qsTr("Copy"), "IconSource": ""});
        }

        if (inputActions.canPaste){
            contextMenuModel.append({"Id": "Paste", "Name": qsTr("Paste"), "IconSource": ""});
        }

        if (inputActions.canRemove){
            contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": ""});
        }

        if (inputActions.canSelectAll){
            contextMenuModel.append({"Id": "SelectAll", "Name": qsTr("Select All"), "IconSource": ""});
        }

        modalDialogManager.openDialog(popupMenu, {"x": x, "y": y, "model": contextMenuModel});
    }

    Component {
        id: popupMenu;

        PopupMenuDialog {
            onFinished: {
                this.root.closeDialog();

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
        }
    }
}
