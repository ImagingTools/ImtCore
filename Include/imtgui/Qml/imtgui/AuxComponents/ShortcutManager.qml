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

    Shortcut {
        sequence: "Ctrl+A";
        context: Qt.ApplicationShortcut;

        onActivated: {
            console.log("onActivated Ctrl+A");

            Events.sendEvent("SelectAll");
        }
    }

    Shortcut {
        sequence: "Shift+Down";
        context: Qt.ApplicationShortcut;

        onActivated: {
            console.log("onActivated Shift+Down");

            Events.sendEvent("ShiftDown");
        }
    }

    Shortcut {
        sequence: "Shift+Up";
        context: Qt.ApplicationShortcut;

        onActivated: {
            console.log("onActivated Shift+Up");

            Events.sendEvent("ShiftUp");
        }
    }

    Shortcut {
        sequence: "Shift+LBM";
        context: Qt.ApplicationShortcut;

        onActivated: {
            console.log("onActivated Shift+LBM");

//            Events.sendEvent("ShiftUp");
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

