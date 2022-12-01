import QtQuick 2.12

QtObject {
    property int count: items.length;
    signal selectionChanged();

    property var items: []

//    property var selectionMode: SelectionMode.SM_SINGLE_SELECTION;

//    enum SelectionMode {
//        SM_UNKNOWN,
//        SM_SINGLE_SELECTION,
//        SM_MULTI_SELECTION
//    }

    function select(item){
//        if (selectionMode == SelectionMode.SM_SINGLE_SELECTION){
//            items = []
//        }

        updateSelection();

        item.Selected = true;

        items.push(item)

        selectionChanged();
    }

    function deselect(item){
        item.Selected = false;

        let pos = items.indexOf(item)
        if (pos >= 0){
            items.splice(pos, 1)
        }

        selectionChanged();
    }

    function contains(item){
        console.log("Selection contains", item);
        return items.includes(item);
    }

    function updateSelection(){
        for (let i = 0; i < items.length; i++){
            items[i].Selected = false;
        }

        items = []
    }
}
