import QtQuick 2.12

Item {
    id: container;

    visible: countPage > 0;

    property int countPage: repeaterView.count;
//    property var pagesModel: [];

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

        model: pagesModel;

        delegate: Loader {
            id: dialogLoader;

            anchors.fill: parent;

            sourceComponent: model.Component;

            onLoaded: {
                console.log("StackView onLoaded", item);

                container.itemAdded(pagesModel.count - 1, item);
            }

            onStatusChanged: {
                console.log("StackView Loader onStatusChanged", status);
                if (status === Loader.Error){
                    console.error("StackView item loading was failed!");
                }
            }

            Component.onCompleted: {
                console.log("StackView Loader onCompleted");
            }
        }

        onItemRemoved: {
            container.itemRemoved(index, item);
        }
    }
}
