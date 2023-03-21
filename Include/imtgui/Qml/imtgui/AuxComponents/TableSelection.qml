import QtQuick 2.12
import Acf 1.0

QtObject {
    id: root;

    property var selectedIndexes: []

    property ListView table: null;

    signal selectionChanged();

    Component.onDestruction: {
       root.unsubscribeEvents();
    }

    onTableChanged: {
        if (table != null){
            root.subscribeEvents();
        }
    }

    onSelectionChanged: {
        for (let i = 0; i < root.table.count; i++){
            let item = root.table.itemAtIndex(i);
            if (root.selectedIndexes.includes(i)){
                item.selected = true;
            }
            else{
                item.selected = false;
            }
        }
    }

    function subscribeEvents(){
        console.log("TableSelection subscribeEvents");

        Events.subscribeEvent("SelectAll", root.selectAll);
        Events.subscribeEvent("ShiftUp", root.shiftUp);
        Events.subscribeEvent("ShiftDown", root.shiftDown);
    }

    function unsubscribeEvents(){
        Events.unSubscribeEvent("SelectAll", root.selectAll);
        Events.unSubscribeEvent("ShiftUp", root.shiftUp);
        Events.unSubscribeEvent("ShiftDown", root.shiftDown);
    }

    function resetSelection(){
        selectedIndexes = []

        root.selectionChanged();
    }

    function singleSelect(index){
        selectedIndexes = []
        selectedIndexes.push(index)

        root.selectionChanged();
    }

    function selectAll(){
        selectedIndexes = []

        for (let i = 0; i < table.count; i++){
            selectedIndexes.push(i);
        }

        root.selectionChanged();
    }

    function up(){
        if (root.selectedIndexes.length > 0){
            let lastIndex = root.selectedIndexes[root.selectedIndexes.length - 1];
            if (lastIndex > 0){
                root.singleSelect(lastIndex - 1)

                root.selectionChanged();
            }
        }
    }

    function down(){
        if (root.selectedIndexes.length > 0){
            let lastIndex = root.selectedIndexes[root.selectedIndexes.length - 1];
            if (table.count - 1 > lastIndex){
                root.singleSelect(lastIndex + 1)

                root.selectionChanged();
            }
        }
    }

    function shiftUp(){
        if (root.selectedIndexes.length > 0){
            let lastIndex = root.selectedIndexes[root.selectedIndexes.length - 1];
            if (lastIndex > 0){
                var index = root.selectedIndexes.indexOf(lastIndex - 1);
                if (index >= 0){
                    root.selectedIndexes.splice(root.selectedIndexes.indexOf(lastIndex), 1);
                }
                else{
                    root.selectedIndexes.push(lastIndex - 1);
                }

                root.selectionChanged();
            }
        }
    }

    function shiftDown(){
        if (root.selectedIndexes.length > 0){
            let lastIndex = root.selectedIndexes[root.selectedIndexes.length - 1];
            if (table.count - 1 > lastIndex){
                var index = root.selectedIndexes.indexOf(lastIndex + 1);
                if (index >= 0){
                    root.selectedIndexes.splice(root.selectedIndexes.indexOf(lastIndex), 1);
                }
                else{
                    root.selectedIndexes.push(lastIndex + 1);
                }

                root.selectionChanged();
            }
        }
    }
}
