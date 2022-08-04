import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: keyboardManager;

    property Item collection;

    Keys.onPressed: {
        console.log("KeyboardManager onPressed", event.key);
        let countElements = collection.table.elements.GetItemsCount();
        if (event.key == Qt.Key_Up){
            if (collection.table.selectedIndex < countElements - 1){
                collection.table.selectedIndex++;
            }
            else{
                collection.table.selectedIndex = 0;
            }

            collection.table.selectedIndex++;
        }
        else if (event.key == Qt.Key_Down){
            if (collection.table.selectedIndex > 0){
                collection.table.selectedIndex--;
            }
            else{
                collection.table.selectedIndex = countElements - 1;
            }
        }
    }
}
