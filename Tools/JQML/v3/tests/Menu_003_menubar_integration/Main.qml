import QtQuick 2.12
import imtcontrols 1.0

Item {
    id: root
    width: 800
    height: 400

    Component { id: menuComponent; Menu {} }
    Component { id: menuItemComponent; MenuItem {} }

    MenuBar {
        id: bar
    }

    Timer {
        interval: 0
        running: true
        repeat: false
        onTriggered: {
            var fileMenu = menuComponent.createObject(root, { title: "&File" })
            var editMenu = menuComponent.createObject(root, { title: "&Edit" })
            var viewMenu = menuComponent.createObject(root, { title: "&View" })

            fileMenu.addItem(menuItemComponent.createObject(root, { text: "Open" }))
            editMenu.addItem(menuItemComponent.createObject(root, { text: "Copy" }))
            viewMenu.addItem(menuItemComponent.createObject(root, { text: "Zoom In" }))

            bar.addMenu(fileMenu)
            bar.addMenu(editMenu)
            bar.addMenu(viewMenu)

            console.log("menu003-items:" + bar._items.length)

            bar._openAt(0)
            console.log("menu003-open0:" + bar.currentIndex + ":" + fileMenu.opened + ":" + editMenu.opened)

            bar._onHover(2)
            console.log("menu003-hover2:" + bar.currentIndex + ":" + fileMenu.opened + ":" + viewMenu.opened)

            bar._toggleAt(2)
            console.log("menu003-toggle-close:" + bar.currentIndex + ":active=" + bar.active)

            bar.focusedIndex = 0
            bar._moveFocus(+1)
            console.log("menu003-focus-move:" + bar.focusedIndex)

            bar._openAt(bar.focusedIndex)
            bar._items[bar.currentIndex].menu._onItemClick(0)
            console.log("menu003-after-click:" + bar.currentIndex + ":active=" + bar.active)

            Qt.quit()
        }
    }
}
