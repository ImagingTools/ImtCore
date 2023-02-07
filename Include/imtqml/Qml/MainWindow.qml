import QtQuick 2.6
import QtQuick.Window 2.1

Window
{
    id: mainWindow;

    height: 600;
    width: 800;

    minimumHeight: 480;
    minimumWidth: 740;

    visible: true;

    property var activeFocusItems: []

    onActiveFocusItemChanged: {
        console.log("onActiveFocusItemChanged", activeFocusItem);

//        if (mainWindow.activeFocusItem != null){
//            activeFocusItems.push(mainWindow.activeFocusItem);
//        }
//        else{
//            activeFocusItems.pop();

//            let item = activeFocusItems[activeFocusItems.length - 1]

//            item.forceActiveFocus();
//        }
    }
}


