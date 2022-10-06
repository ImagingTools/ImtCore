import QtQuick 2.12

Item {
    id: container;

    property alias backgroundItem: background;
    property bool openST: false;

    Component.onDestruction: {
        modalDialogModels.clear();
    }

    ListModel {
        id: modalDialogModels;
    }

    function openDialog(comp, parameters){
        console.log("DialogsManager addDialog", comp);
        modalDialogModels.append({"Component": comp, "Parameters": parameters});
        container.openST = true;
    }

    function closeDialog(){
        console.log("DialogsManager closeDialog");

        if (modalDialogModels.count > 0){
            modalDialogModels.remove(modalDialogModels.count - 1);
        }

        container.openST = false;

    }

    Rectangle {
        id: background;

        anchors.fill: parent;

        //TODO: Style.backgroundColor
        color: "gray";
        visible: modalDialogs.visible;

        property alias backgroundAreaItem: backgroundArea;
        MouseArea {
            id: backgroundArea;
            anchors.fill: parent;

            onWheel: {}
        }
    }

    Repeater {
        id: modalDialogs;

        anchors.fill: parent;

        visible: modalDialogModels.count > 0;

        model: modalDialogModels;

        delegate: Item {
            id: dialogDelegate;

            anchors.fill: modalDialogs;

            Loader {
                id: dialogLoader;

                sourceComponent: model.Component;

                onLoaded: {
                    dialogLoader.item["root"] = container;
                    for (let key in model.Parameters) {
                        console.log(key, model.Parameters[key]);
                        dialogLoader.item[key] = model.Parameters[key];
                    }
                    if (dialogLoader.item.centered){
                        dialogLoader.anchors.centerIn = dialogDelegate;
                    }

                    dialogLoader.item.forceActiveFocus();
                }
            }
        }
    }
}
