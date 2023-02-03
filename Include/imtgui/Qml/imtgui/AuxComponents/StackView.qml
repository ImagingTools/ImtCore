import QtQuick 2.12

Item {
    id: container;

    property int countPage: pagesModel.count;

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
                    dialogLoader.item.documentsData = model.documentsData;
                    dialogLoader.item.documentManager = model.documentManager;
                    dialogLoader.item.commandsId = model.CommandsId;
                    dialogLoader.item.forceActiveFocus();
                }
            }
        }
    }
}
