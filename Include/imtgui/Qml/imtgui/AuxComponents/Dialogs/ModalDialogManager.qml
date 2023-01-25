import QtQuick 2.12

Item {
    id: container;

    property var backgroundItem: null;
    property alias count: modalDialogModels.count;

    Component.onDestruction: {
        modalDialogModels.clear();
    }

    ListModel {
        id: modalDialogModels;
    }

    function openDialog(comp, parameters){
        console.log("DialogsManager addDialog", comp);
        modalDialogModels.append({"Component": comp, "Parameters": parameters});
    }

    function closeDialog(){
        console.log("DialogsManager closeDialog");

        if (modalDialogModels.count > 0){
            modalDialogModels.remove(modalDialogModels.count - 1);
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

            Rectangle {
                id: background;

                anchors.fill: parent;

                color: "gray";
//                color: "transparent";
                visible: modalDialogs.visible;

                property alias backgroundAreaItem: backgroundArea;
                MouseArea {
                    id: backgroundArea;
                    anchors.fill: parent;
                    hoverEnabled: true;

                    onWheel: {}
                }
            }

            Loader {
                id: dialogLoader;

                sourceComponent: model.Component;

                onLoaded: {
                    container.backgroundItem = background;
                    dialogLoader.item["root"] = container;
                    for (let key in model.Parameters) {
                        dialogLoader.item[key] = model.Parameters[key];
                    }
                    if (dialogLoader.item.centered){
                        dialogLoader.anchors.centerIn = dialogDelegate;
                    }
                    dialogLoader.item.started()
                    dialogLoader.item.forceActiveFocus();
                }
            }
        }
    }
}
