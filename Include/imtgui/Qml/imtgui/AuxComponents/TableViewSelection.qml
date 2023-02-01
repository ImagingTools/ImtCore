import QtQuick 2.12

QtObject {
    id: root;

    property int count: items.length;

    property var items: [];

    signal selectionChanged();

    property ModelIndex selectedIndex: null;

    onSelectedIndexChanged: {
        console.log("TableViewSelection onSelectedIndexChanged");
    }

    function tableKeyPressed(event){
        console.log("TableViewSelection tableKeyPressed", event.key);
        if (tableViewRoot.selectedIndex == null){
            return;
        }

        if (event.key == Qt.Key_Up) {
            console.log("Key_Up");
            let currentIndex = tableViewRoot.selectedIndex;
            let prevIndex = currentIndex.prevIndex;
            if (prevIndex == null){
                let parentIndex = currentIndex.parentIndex;

                prevIndex = parentIndex;
            }
            else{
                while (prevIndex.childModel.length > 0){
                    prevIndex = prevIndex.childModel[prevIndex.childModel.length - 1];
                }
            }

            if (prevIndex != null){
                root.select(prevIndex.itemData);
            }
        }
        else if (event.key == Qt.Key_Down) {
            let currentIndex = tableViewRoot.selectedIndex;
            if (currentIndex != null){
                let nextIndex;
                // Если есть дочерние индексы => переходим к ним
                if (currentIndex.childModel.length > 0){
                    nextIndex = currentIndex.childModel[0];
                }
                else{
                    nextIndex = tableViewRoot.selectedIndex.nextIndex;
                }

                // Если следующего нет => берем следующий у parent
                if (nextIndex == null){
                    let parentIndex = currentIndex.parentIndex;

                    while (parentIndex != null){
                        nextIndex = parentIndex.nextIndex;
                        if (nextIndex != null){
                            break;
                        }

                        parentIndex = parentIndex.parentIndex;
                    }
                }

                if (nextIndex != null){
                    root.select(nextIndex.itemData);
                }
            }
        }
    }

    //    property var selectionMode: SelectionMode.SM_SINGLE_SELECTION;

    //    enum SelectionMode {
    //        SM_UNKNOWN,
    //        SM_SINGLE_SELECTION,
    //        SM_MULTI_SELECTION
    //    }

    function select(item){
        console.log("select", item);
        //        if (selectionMode == SelectionMode.SM_SINGLE_SELECTION){
        //            items = []
        //        }

        root.updateSelection();

        item.Selected = true;

        root.items.push(item)

        console.log("items", items);

        root.selectionChanged();
    }

    function deselect(item){
        console.log("deselect", item);

        item.Selected = false;

        console.log("items 1 ", root.items);

        let pos = root.items.indexOf(item)
        console.log("pos", pos);

        if (pos >= 0){
            root.items.splice(pos, 1)
        }

        console.log("items 2 ", root.items);

        root.selectionChanged();
    }

    function contains(item){
        console.log("Selection contains", item);
        return root.items.includes(item);
    }

    function updateSelection(){
        for (let i = 0; i < root.items.length; i++){
            root.items[i].Selected = false;
        }

        root.items = [];
    }
}
