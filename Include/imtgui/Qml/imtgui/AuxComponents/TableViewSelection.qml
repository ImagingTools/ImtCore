import QtQuick 2.12

QtObject {
    id: tableViewSelection;

    property int count: items.length;

    property var items: [];

    signal selectionChanged();

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

        tableViewSelection.updateSelection();

        item.Selected = true;

        tableViewSelection.items.push(item)

        console.log("items", items);

        tableViewSelection.selectionChanged();
    }

    function deselect(item){
        console.log("deselect", item);

        item.Selected = false;

        console.log("items 1 ", tableViewSelection.items);

        let pos = tableViewSelection.items.indexOf(item)
        console.log("pos", pos);

        if (pos >= 0){
            tableViewSelection.items.splice(pos, 1)
        }

        console.log("items 2 ", tableViewSelection.items);

        tableViewSelection.selectionChanged();
    }

    function contains(item){
        console.log("Selection contains", item);
        return tableViewSelection.items.includes(item);
    }

    function updateSelection(){
        for (let i = 0; i < tableViewSelection.items.length; i++){
            tableViewSelection.items[i].Selected = false;
        }

        tableViewSelection.items = [];
    }
}
