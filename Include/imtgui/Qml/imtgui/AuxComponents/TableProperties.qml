import QtQuick 2.12

QtObject {
    id: root;

    property var checkedItems: []
    property var selectedItems: []
    property var invisibleItems: []
    property var disableItems: []

    signal visibleItemsChanged();
    signal stateItemsChanged();

    function addInvisibleItem(index){
        root.invisibleItems.push(index)

        root.visibleItemsChanged();
    }

    function clearInvisibleItems(){
        root.invisibleItems = []

        root.visibleItemsChanged();
    }

    function clearDisabledItems(){
        root.disableItems = []

        root.stateItemsChanged();
    }

    function itemIsEnabled(index){
        return !root.disableItems.includes(index);
    }

    function setIsEnabledItem(index, isEnabled){
        if (isEnabled){
            let i = root.disableItems.indexOf(index);
            if (i !== -1) {
                root.disableItems.splice(i, 1);

                root.stateItemsChanged();
            }
        }
        else{
            root.disableItems.push(index)

            root.stateItemsChanged();
        }
    }

    function setIsVisibleItem(index, isVisible){
        if (isVisible){
            let i = root.invisibleItems.indexOf(index);
            if (i !== -1) {
                root.invisibleItems.splice(i, 1);

                root.visibleItemsChanged();
            }
        }
        else{
            root.invisibleItems.push(index)

            root.visibleItemsChanged();
        }
    }

    function itemIsVisible(index){
        return !root.invisibleItems.includes(index);
    }
}


