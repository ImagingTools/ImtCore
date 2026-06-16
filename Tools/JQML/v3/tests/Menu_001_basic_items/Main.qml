import QtQuick 2.12
import imtcontrols 1.0

Item {
    id: root
    width: 640
    height: 480

    property int triggeredCount: 0
    property string lastTriggeredText: ""

    Menu {
        id: fileMenu
        title: "&File"

        MenuItem {
            id: newItem
            text: "&New"
            checkable: true
        }

        MenuItem {
            text: "Open"
        }

        MenuSeparator {}

        Menu {
            id: recentMenu
            title: "Recent"

            MenuItem { text: "Session A" }
            MenuItem { text: "Session B" }
        }

        onTriggered: {
            root.triggeredCount += 1
            root.lastTriggeredText = menuItem ? menuItem.text : "<null>"
        }
    }

    Timer {
        interval: 0
        running: true
        repeat: false
        onTriggered: {
            if (fileMenu.count === 0)
                fileMenu._ingestDeclaredChildren()

            var row0 = fileMenu.itemAt(0)
            var row3 = fileMenu.itemAt(3)

            console.log("menu001-count:" + fileMenu.count)
            console.log("menu001-row0:" + (row0 ? row0.text : "<null>"))
            console.log("menu001-row3-submenu:" + (row3 ? row3.hasSubmenu + ":" + row3.submenu.title : "<null>"))

            fileMenu._onItemClick(0)

            console.log("menu001-triggered:" + root.triggeredCount + ":" + root.lastTriggeredText)
            console.log("menu001-new-checked:" + newItem.checked)

            Qt.quit()
        }
    }
}
