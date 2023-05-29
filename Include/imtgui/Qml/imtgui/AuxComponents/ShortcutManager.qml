import QtQuick 2.12
import Acf 1.0

QtObject {
    id: collectionViewContainer;

    property Shortcut ctrlF: Shortcut {
        sequence: "Ctrl+F";
        context: Qt.ApplicationShortcut;

        onActivated: {
            console.log("onActivated Ctrl+F");

            Events.sendEvent("FilterActivated");
        }
    }

    property Shortcut ctrlA: Shortcut {
        sequence: "Ctrl+A";
        context: Qt.ApplicationShortcut;

        onActivated: {
            console.log("onActivated Ctrl+A");

            Events.sendEvent("SelectAll");
        }
    }

    property Shortcut ctrlDown: Shortcut {
        sequence: "Shift+Down";
        context: Qt.ApplicationShortcut;

        onActivated: {
            console.log("onActivated Shift+Down");

            Events.sendEvent("ShiftDown");
        }
    }

    property Shortcut ctrlUp: Shortcut {
        sequence: "Shift+Up";
        context: Qt.ApplicationShortcut;

        onActivated: {
            console.log("onActivated Shift+Up");

            Events.sendEvent("ShiftUp");
        }
    }
}

