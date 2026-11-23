import QtQuick
import QtQuick.Controls
import QtTest

// A Menu of Actions declared in it (its MenuItems made for them), in a ToolButton; Escape closes the open menu and does not
// reach a Shortcut of Escape
Item {
    width: 360; height: 300
    property var log: []
    Shortcut { sequences: ["Esc"]; onActivated: log.push("shortcut") }
    ToolButton {
        id: button
        x: 300; text: "⋮"
        onClicked: menu.open()
        Menu {
            id: menu
            x: parent.width - width
            transformOrigin: Menu.TopRight
            Action { text: "Settings"; onTriggered: log.push("settings") }
            Action { text: "Help"; onTriggered: log.push("help") }
            MenuItem { text: "Item" }
        }
    }
    TestCase { name: "MenuActions"; when: windowShown
        function test_a() {
            let names = []; for (let i = 0; i < menu.count; i++) names.push(menu.itemAt(i).text); console.log("items", names.join(","))
            console.log("count", menu.count, menu.itemAt(0).text, menu.itemAt(1).text, menu.itemAt(2).text, menu.actionAt(0).text, menu.itemAt(0).action !== null)
            mouseClick(button); wait(300)
            console.log("open", menu.visible, menu.width > 100, menu.itemAt(0).height > 10)
            keyClick(Qt.Key_Escape); wait(300)
            console.log("escape", menu.visible, log.join(","))
            mouseClick(button); wait(300)
            let item = menu.itemAt(1)
            mouseClick(item, item.width / 2, item.height / 2); wait(300)
            console.log("click", menu.visible, log.join(","))
        }
    }
}
