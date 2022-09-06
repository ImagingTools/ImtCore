import QtQuick 2.12

Item {
    id: container;

    Component.onDestruction: {
        pagesModel.clear();
    }

    ListModel {
        id: pagesModel;
    }

    function push(parameters){
        console.log("StackView openPage");
        pagesModel.append(parameters);
    }

    function pop(){
        if (pagesModel.count > 0){
            pagesModel.remove(pagesModel.count - 1);
        }
    }

    Repeater {
        id: repeaterView;

        anchors.fill: parent;

        visible: pagesModel.count > 0;

        model: pagesModel;

        delegate: Item {
            id: dialogDelegate;

            anchors.fill: repeaterView;
            Loader {
                id: dialogLoader;

                anchors.fill: parent;

                source: model.Source;

                onLoaded: {
                    let object = pagesModel.get(model.index);
                    dialogLoader.item.commandsId = model.CommandsId;
//                    for (let key in object) {
//                        console.log("key", key);
//                        console.log("model[key]", object[key]);

//                        if (dialogLoader.item[key]){
//                            dialogLoader.item[key] = object[key];
//                        }

//                        dialogLoader.item.commandsId = object["CommandsId"];
//                    }

                    dialogLoader.item.forceActiveFocus();
                }
            }
        }
    }
}
