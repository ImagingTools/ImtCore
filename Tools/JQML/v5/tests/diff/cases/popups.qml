import QtQuick
import QtQuick.Controls
import QtTest

// ToolTip (defaults, its place over its parent), Dialog (header, footer box, standard buttons, accept / reject / Escape),
// Menu (popup(x, y), items, keys, a click on an item), Drawer (edge, position, open / close)
Item {
    id: root
    width: 500; height: 400
    property var log: []
    Button { id: btn; x: 100; y: 100; width: 100; height: 40; ToolTip.text: "Hint"; ToolTip.visible: false }
    ToolTip { id: tip; parent: btn; text: "Tip text" }
    Dialog { id: dlg; title: "Title"; standardButtons: Dialog.Ok | Dialog.Cancel; x: 50; y: 50; width: 300
        onAccepted: log.push("accepted"); onRejected: log.push("rejected"); onClosed: log.push("closed") }
    Menu { id: menu
        MenuItem { id: mi1; text: "First"; onTriggered: log.push("triggered First") }
        MenuItem { id: mi2; text: "Check"; checkable: true }
        MenuSeparator { }
        MenuItem { text: "Third" } }
    Drawer { id: drawer; width: 200; height: root.height }
    TestCase { name: "Popups2"; when: windowShown
        function flush(n) { console.log(n + ": " + log.join(", ")); log = [] }
        function test_1_tooltip() {
            console.log("tip defaults", tip.delay, tip.timeout, tip.margins, tip.padding, tip.closePolicy, tip.x, tip.y, tip.implicitHeight > 0)
            tip.open(); let ti = tip.contentItem
            console.log("tip open", tip.visible, tip.x === (btn.width - tip.implicitWidth) / 2, tip.y === -tip.implicitHeight - 3, ti.text, ti.color, ti.wrapMode, tip.background.color, tip.background.border.color)
            tip.close()
            console.log("attached", btn.ToolTip.text, btn.ToolTip.timeout, btn.ToolTip.delay)
        }
        function test_2_dialog() {
            console.log("dlg defaults", dlg.modal, dlg.closePolicy, dlg.padding, dlg.result, dlg.title, dlg.header ? dlg.header.text : "-", dlg.footer ? dlg.footer.count : "-")
            dlg.open()
            console.log("dlg open", dlg.visible, dlg.height - dlg.implicitHeaderHeight, dlg.implicitHeaderHeight > 0, dlg.implicitFooterHeight, dlg.header.visible, dlg.header.font.bold, dlg.header.padding, dlg.footer.visible, dlg.footer.count, dlg.footer.padding, dlg.footer.spacing, dlg.footer.alignment)
            let ok = dlg.standardButton(Dialog.Ok)
            console.log("ok button", ok.text, ok.DialogButtonBox.buttonRole, ok.width, dlg.standardButton(Dialog.Cancel).text)
            mouseClick(ok, 5, 5); flush("click ok"); console.log("result", dlg.result, dlg.visible)
            dlg.open(); dlg.reject(); flush("reject"); console.log("result", dlg.result)
            dlg.open(); keyClick(Qt.Key_Escape); flush("escape")
        }
        function test_3_menu() {
            console.log("menu defaults", menu.count, menu.margins, menu.overlap, menu.padding, menu.currentIndex, menu.cascade, menu.focus, menu.modal)
            menu.popup(root, 20, 30)
            console.log("menu open", menu.visible, menu.x, menu.y, menu.width, menu.height, menu.contentItem.count, mi1.width, mi1.height, mi1.highlighted, mi2.indicator.visible, mi1.padding, mi1.contentItem.leftPadding, mi2.contentItem.leftPadding, menu.activeFocus)
            keyClick(Qt.Key_Down); console.log("down", menu.currentIndex, mi1.highlighted); keyClick(Qt.Key_Down); console.log("down", menu.currentIndex)
            mouseClick(mi1, 5, 5); flush("click item"); console.log("menu after", menu.visible)
            menu.open(); mouseClick(mi2, 5, 5); console.log("checkable", mi2.checked, menu.visible)
        }
        function test_4_drawer() {
            console.log("drawer", drawer.edge, drawer.position, drawer.modal, drawer.dim, drawer.interactive, drawer.dragMargin, drawer.closePolicy, drawer.parent === Overlay.overlay)
            drawer.open(); wait(500); let di = drawer.contentItem.parent
            console.log("drawer open", drawer.position, drawer.x, drawer.y, di.x, di.width, di.height, drawer.background.color, drawer.rightPadding)
            drawer.close(); wait(500); console.log("drawer closed", drawer.position, drawer.visible)
        }
    }
}
