import QtQuick
import QtQuick.Controls
import QtTest

// ApplicationWindow (menu bar, header, footer and the content item between them, its font passing to its controls, the
// attached ApplicationWindow), MenuBar and MenuBarItem (an item per menu, a click opening the menu below it)
Item {
    width: 300; height: 200
    ApplicationWindow { id: w; width: 400; height: 300; visible: true; font.pixelSize: 20
        menuBar: MenuBar { id: mb; Menu { id: fileMenu; title: "File"; MenuItem { text: "Open" } } Menu { title: "Edit" } }
        header: ToolBar { id: hdr }
        footer: Rectangle { id: ftr; implicitHeight: 30 }
        Rectangle { id: body; anchors.fill: parent }
        Button { id: btn }
    }
    TestCase { name: "AppWin"; when: windowShown
        function g(i) { return [i.x, i.y, i.width, i.height].join(",") }
        function test_appwin() {
            console.log("window", w.color, w.font.pixelSize, btn.font.pixelSize, w.contentItem !== null, g(w.contentItem), body.parent === w.contentItem, w.background === null)
            console.log("bars", g(mb), g(hdr), g(ftr), hdr.position, mb.count, mb.menus.length, mb.spacing, mb.padding)
            let item = mb.itemAt(0)
            console.log("menubar item", item.text, item.x, item.y, item.height, item.menu === fileMenu, item.leftPadding, item.rightPadding, item.highlighted, item.background.color)
            mouseClick(item, 5, 5)
            console.log("menu open", fileMenu.visible, fileMenu.x, fileMenu.y, item.highlighted, mb.currentIndex)
            console.log("attached", btn.ApplicationWindow.window === w, btn.ApplicationWindow.header === hdr, btn.ApplicationWindow.menuBar === mb)
        }
    }
}
