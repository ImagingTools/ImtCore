import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: collectionViewContainer;

    Shortcut {
        sequence: "Ctrl+F";
        context: Qt.ApplicationShortcut;

        onActivated: {
            console.log("onActivated Ctrl+F");

            Events.sendEvent("FilterActivated");
        }
    }

//    Shortcut {
//        sequence: "Ctrl+S";
//        context: Qt.ApplicationShortcut;

//        onActivated: {
//            console.log("onActivated Ctrl+S");

//            Events.sendEvent("SaveActivated");
//        }
//    }

//    Shortcut {
//        sequence: "Ctrl+Z";
//        context: Qt.ApplicationShortcut;

//        onActivated: {
//            console.log("onActivated Ctrl+Z");

//            Events.sendEvent("UndoActivated");
//        }
//    }

//    Shortcut {
//        sequence: "Ctrl+Shift+Z";
//        context: Qt.ApplicationShortcut;

//        onActivated: {
//            console.log("onActivated Ctrl+Shift+Z");

//            Events.sendEvent("RedoActivated");
//        }
//    }
}

