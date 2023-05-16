import QtQuick 2.12

QtObject {
    id: root;

    property int count: items.length;

    property var items: [];

//    property ModelIndex selectedIndex: null;

    signal selectionChanged();

    function resetSelection(){
        root.items = []

        root.selectionChanged();
    }

    function singleSelect(modelIndex){
        root.items = []
        root.items.push(modelIndex)

        root.selectionChanged();
    }

    function isSelected(modelIndex){
        return root.items.includes(modelIndex);
    }

//    function tableKeyPressed(event){
//        if (tableViewRoot.selectedIndex == null){
//            return;
//        }

//        if (event.key == Qt.Key_Up) {
//            let currentIndex = tableViewRoot.selectedIndex;
//            let prevIndex = currentIndex.prevIndex;
//            if (prevIndex == null){
//                let parentIndex = currentIndex.parentIndex;

//                prevIndex = parentIndex;
//            }
//            else if (prevIndex != null && prevIndex.itemData.IsOpen){
//                // Если есть предыдущий смотрим его childModel
//                while (prevIndex.childModel.length > 0){
//                    prevIndex = prevIndex.childModel[prevIndex.childModel.length - 1];
//                    if (!prevIndex.itemData.IsOpen){
//                        break;
//                    }
//                }
//            }

//            if (prevIndex != null){
//                root.select(prevIndex.itemData);
//            }
//        }
//        else if (event.key == Qt.Key_Down) {
//            let currentIndex = tableViewRoot.selectedIndex;
//            if (currentIndex != null){
//                let nextIndex;
//                // Если есть дочерние индексы и элемент раскрыт => переходим к ним
//                if (currentIndex.childModel.length > 0 && currentIndex.itemData.IsOpen){
//                    nextIndex = currentIndex.childModel[0];
//                }
//                else{
//                    nextIndex = tableViewRoot.selectedIndex.nextIndex;
//                }

//                // Если следующего нет => берем следующий у parent
//                if (nextIndex == null){
//                    let parent = currentIndex.parentIndex;

//                    while (parent != null){
//                        nextIndex = parent.nextIndex;
//                        if (nextIndex != null){
//                            break;
//                        }

//                        parent = parent.parentIndex;
//                    }
//                }

//                if (nextIndex != null){
//                    root.select(nextIndex.itemData);
//                }
//            }
//        }
//    }

    //    property var selectionMode: SelectionMode.SM_SINGLE_SELECTION;

    //    enum SelectionMode {
    //        SM_UNKNOWN,
    //        SM_SINGLE_SELECTION,
    //        SM_MULTI_SELECTION
    //    }

    function select(item){
        root.updateSelection();

        item.Selected = true;

        root.items.push(item)

        root.selectionChanged();
    }

    function deselect(item){
        item.Selected = false;
        let pos = root.items.indexOf(item)

        if (pos >= 0){
            root.items.splice(pos, 1)
        }

        root.selectionChanged();
    }

    function contains(item){
        return root.items.includes(item);
    }

    function updateSelection(){
        for (let i = 0; i < root.items.length; i++){
            root.items[i].Selected = false;
        }

        root.items = [];
    }
}
