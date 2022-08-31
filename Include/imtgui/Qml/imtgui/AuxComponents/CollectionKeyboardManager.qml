import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: keyboardManager;

    onFocusChanged: {
        console.log("KeyboardManager onFocusChanged", focus);
    }

    Keys.onPressed: {
        console.log("KeyboardManager onPressed", event.key);
        let countElements = table.elements.GetItemsCount();
        if (event.key == Qt.Key_Up){
            if (table.selectedIndex < countElements - 1){
                table.selectedIndex++;
            }
            else{
                table.selectedIndex = 0;
            }

            table.selectedIndex++;
        }
        else if (event.key == Qt.Key_Down){
            if (table.selectedIndex > 0){
                table.selectedIndex--;
            }
            else{
                table.selectedIndex = countElements - 1;
            }
        }
    }
}
