import QtQuick

// A row that passes the focus it gets on to a field inside a Loader, from its own onFocusChanged
Window {
    id: root
    visible: true
    width: 300
    height: 200

    Rectangle {
        id: row
        width: 200
        height: 40

        onFocusChanged: {
            if (focus && loader.item){
                loader.item.forceActiveFocus()
            }
        }

        Loader {
            id: loader
            sourceComponent: Component {
                TextInput { width: 100; height: 20 }
            }
        }
    }

    Timer {
        interval: 200
        running: true
        onTriggered: {
            row.forceActiveFocus()
            console.log("row:", row.activeFocus, "field:", loader.item.activeFocus, "row focus:", row.focus)
            Qt.quit();
        }
    }
}
