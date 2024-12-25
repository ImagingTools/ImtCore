import QtQuick 2.12

Item {
    id: container;

    visible: countPage > 0;

    property int countPage: repeaterView.count;

    property ListModel pagesModel: ListModel {}

	signal itemAdded(int index, var item);
	signal itemRemoved(int index, var item);

    function push(item){
        pagesModel.append({"Component": item})
    }

    function pop(){
        let loader = repeaterView.itemAt(repeaterView.count - 1);
        pagesModel.remove(pagesModel.count - 1)
    }

    function peekComp(){
        if (pagesModel.count > 0){
            let loader = repeaterView.itemAt(pagesModel.count - 1);

            return loader.sourceComponent;
        }

        return null;
    }

    function peekItem(){
        if (pagesModel.count > 0){
            let loader = repeaterView.itemAt(pagesModel.count - 1);

            return loader.item;
        }

        return null;
    }

    function getPageIndex(pageComp){
        for (let i = 0; i < pagesModel.count; i++){
            let comp = pagesModel.get(i).Component;
            if (comp && comp === pageComp){
                return i;
            }
        }

        return -1;
    }

    function removePageByIndex(index){
        pagesModel.remove(index)
    }

    function removePageByComp(pageComp){
        let index = getPageIndex(pageComp);
        if (index >= 0){
            removePageByIndex(index);
        }
    }

    function clear(){
        pagesModel.clear();
    }

    Repeater {
        id: repeaterView;
        anchors.fill: parent;
        model: container.pagesModel;

        delegate: Loader {
            anchors.fill: parent;
            sourceComponent: model.Component;
            visible: model.index === container.pagesModel.count - 1;

            onLoaded: {
				container.itemAdded(container.pagesModel.count - 1, item);
            }

            onStatusChanged: {
                if (status === Loader.Error){
                    console.error("StackView item loading was failed");
                }
            }
        }

        onItemRemoved: {
			container.itemRemoved(index, item);
        }
    }
}
