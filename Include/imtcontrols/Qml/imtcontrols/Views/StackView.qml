import QtQuick 2.12

Item {
    id: container;

    visible: countPage > 0;

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

    function peek(){
        if (pagesModel.count > 0){
            let loader = repeaterView.itemAt(pagesModel.count - 1);

            return loader.item;
        }

        return null;
    }

    Repeater {
        id: repeaterView;

        anchors.fill: parent;

        visible: pagesModel.count > 0;

        model: pagesModel;

        delegate: Loader {
            id: dialogLoader;

            anchors.fill: parent;

            source: model.Source;

            onLoaded: {
                let object = pagesModel.get(model.index);
                //dialogLoader.item.documentsData = model.documentsData;
                dialogLoader.item.documentManager = model.documentManager;
                dialogLoader.item.itemId = model.Id;
                dialogLoader.item.commandsId = model.CommandsId;
                dialogLoader.item.forceActiveFocus();
            }
        }
    }
}
