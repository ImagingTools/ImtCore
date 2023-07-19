import QtQuick 2.12
import Acf 1.0

QtObject {
    id: root;

    property var selectedIndexes: []

    property ListView table: null;
    signal selectionChanged();

    property bool isMultiSelect: true;

    property int countElements: root.table ? root.table.count : -1;

    Component.onDestruction: {
        root.unsubscribeEvents();
    }

    onTableChanged: {
        if (root.table != null){
            root.subscribeEvents();
        }
    }

    onSelectionChanged: {
        console.log("onSelectionChanged", root.selectedIndexes);
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
        root.selectedIndexes = []

        root.selectionChanged();
    }

    function singleSelect(index){
        root.selectedIndexes = []
        root.selectedIndexes.push(index)

        root.selectionChanged();
    }

    function selectAll(){
        if (!root.isMultiSelect){
            return;
        }

        selectedIndexes = []

        for (let i = 0; i < root.countElements; i++){
            root.selectedIndexes.push(i);
        }

        root.selectionChanged();
    }

    function isSelected(index){
        return root.selectedIndexes.includes(index);
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
            if (root.countElements - 1 > lastIndex){
                root.singleSelect(lastIndex + 1)

                root.selectionChanged();
            }
        }
    }

    function shiftUp(){
        if (!root.isMultiSelect){
            return;
        }

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
        if (!root.isMultiSelect){
            return;
        }

        if (root.selectedIndexes.length > 0){
            let lastIndex = root.selectedIndexes[root.selectedIndexes.length - 1];
            if (root.countElements - 1 > lastIndex){
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
