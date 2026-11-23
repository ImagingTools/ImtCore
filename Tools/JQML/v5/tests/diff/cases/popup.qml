import QtQuick
import QtQuick.Controls
import QtTest

// Popup: its defaults, open and close (the order of the signals), its place in the overlay (x, y in its parent, margins),
// its content and background, a press outside a popup and a modal one, NoAutoClose, the dimming, Escape
Item {
    id: root
    width: 400; height: 300
    property var log: []
    Rectangle { id: anchor; x: 50; y: 40; width: 100; height: 30
        Popup { id: p; x: 10; y: 35; width: 120; height: 80
            onAboutToShow: log.push("aboutToShow"); onOpened: log.push("opened"); onAboutToHide: log.push("aboutToHide"); onClosed: log.push("closed")
            onVisibleChanged: log.push("visible " + visible); onOpenedChanged: log.push("openedChanged " + opened)
            Rectangle { id: inside; implicitWidth: 30; implicitHeight: 20 } } }
    MouseArea { id: below; x: 300; y: 200; width: 100; height: 100; onPressed: log.push("below pressed") }
    Popup { id: modal; modal: true; x: 20; y: 20; width: 100; height: 100; focus: true }
    Popup { id: edge; x: 350; y: 260; width: 100; height: 100; margins: 0 }
    TestCase { name: "Popups"; when: windowShown
        function flush(n) { console.log(n + ": " + log.join(", ")); log = [] }
        function test_1_open() {
            console.log("defaults", p.visible, p.opened, p.modal, p.dim, p.closePolicy, p.padding, p.margins, p.topMargin, p.z, p.focus, p.popupType, p.parent === anchor)
            p.open(); flush("open")
            let item = p.contentItem.parent
            console.log("geometry", p.x, p.y, p.width, p.height, p.implicitWidth, p.implicitHeight, p.contentWidth, p.contentHeight, p.availableWidth, p.availableHeight)
            console.log("item", item.objectName, item.x, item.y, item.width, item.height, item.parent === Overlay.overlay, item.parent.z, Overlay.overlay.width, Overlay.overlay.height, item.z)
            console.log("content", p.contentItem.x, p.contentItem.y, p.contentItem.width, p.contentItem.height, inside.parent === p.contentItem, p.background.color, p.background.border.color, p.background.x, p.background.width)
            mouseClick(root, 320, 220); flush("press outside")
            console.log("after outside", p.visible)
        }
        function test_2_policies() {
            p.closePolicy = Popup.NoAutoClose; p.open(); mouseClick(root, 320, 220); console.log("no auto close", p.visible); log = []
            p.closePolicy = Popup.CloseOnEscape | Popup.CloseOnPressOutside; p.close(); log = []
            modal.open(); console.log("modal", modal.visible, modal.activeFocus, modal.dim)
            let ov = null; for (let i = 0; i < Overlay.overlay.children.length; i++) { let c = Overlay.overlay.children[i]; if (c.color !== undefined && c !== modal.background) ov = c }
            console.log("dimmer", ov ? [ov.x, ov.y, ov.width, ov.height, ov.color, ov.visible].join(",") : "none")
            mouseClick(root, 320, 220); flush("press outside modal"); console.log("modal after", modal.visible)
            modal.open(); keyClick(Qt.Key_Escape); console.log("escape", modal.visible)
            edge.open(); let ei = edge.contentItem.parent; console.log("edge", edge.x, edge.y, ei.x, ei.y); edge.close()
        }
    }
}
