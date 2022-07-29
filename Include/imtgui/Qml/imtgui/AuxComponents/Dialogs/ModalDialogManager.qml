import QtQuick 2.12

Item {
    id: container;

    ListModel {
        id: modalDialogModels;
    }

    function openDialog(comp, parameters){
        console.log("DialogsManager addDialog", comp);
        modalDialogModels.append({"Component": comp, "Parameters": parameters});
    }

    function closeDialog(){
        console.log("DialogsManager closeDialog");
        modalDialogModels.remove(modalDialogModels.count - 1);
    }

    Rectangle {
        id: background;

        anchors.fill: parent;

        //TODO: Style.backgroundColor
        color: "gray";
        visible: modalDialogs.visible;

        opacity: 0.4;
        MouseArea { anchors.fill: parent; }
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

                anchors.centerIn: dialogDelegate;

                sourceComponent: model.Component;

                onLoaded: {
                    dialogLoader.item["root"] = container;
                    for (let key in model.Parameters) {
                        console.log(key, model.Parameters[key]);
                        dialogLoader.item[key]  = model.Parameters[key];
                    }
                }
            }
        }
    }
}
