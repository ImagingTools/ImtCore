import QtQuick 2.12

Item {
    id: container;

    visible: countPage > 0;

    property int countPage: repeaterView.count;

    property ListModel pagesModel: ListModel {}

    signal itemAdded(int index, var item);
    signal itemRemoved(int index, var item);

    function push(item){
        console.log("StackView push", item);

        pagesModel.append({"Component": item})
    }

    function pop(){
        let loader = repeaterView.itemAt(repeaterView.count - 1);
        pagesModel.remove(pagesModel.count - 1)

        return loader.item;
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

        model: container.pagesModel;

        delegate: Loader {
            id: dialogLoader;

            anchors.fill: parent;

            sourceComponent: model.Component;

            visible: model.index === container.pagesModel.count - 1;

            onLoaded: {
                container.itemAdded(container.pagesModel.count - 1, item);
            }

            onStatusChanged: {
                if (status === Loader.Error){
                    console.error("StackView item loading was failed!");
                }
            }
        }

        onItemRemoved: {
            container.itemRemoved(index, item);
        }
    }
}
