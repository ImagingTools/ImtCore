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
        console.log("singleSelect", modelIndex);

        root.items = []
        root.items.push(modelIndex)

        root.selectionChanged();
    }

    function isSelected(modelIndex){
        return root.items.includes(modelIndex);
    }

//    function select(item){
//        root.updateSelection();

//        item.Selected = true;

//        root.items.push(item)

//        root.selectionChanged();
//    }

//    function deselect(item){
//        item.Selected = false;
//        let pos = root.items.indexOf(item)

//        if (pos >= 0){
//            root.items.splice(pos, 1)
//        }

//        root.selectionChanged();
//    }

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
