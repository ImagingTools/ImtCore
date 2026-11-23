import QtQuick
import QtQuick.Controls
import QtTest

// ComboBox: its defaults and parts, the roles of a ListModel, a click opening the popup (its place, size, delegates),
// a click on a delegate, the keys closed and open (Up, Down, Space, Return), an editable one
Item {
    id: root
    width: 400; height: 400
    property var log: []
    ComboBox { id: c; x: 10; y: 10; model: ["Apple", "Banana", "Cherry"]
        onActivated: (i) => log.push("activated " + i); onHighlighted: (i) => log.push("highlighted " + i)
        onCurrentIndexChanged: log.push("currentIndex " + currentIndex) }
    ComboBox { id: m; x: 200; y: 10; textRole: "name"; valueRole: "code"
        model: ListModel { ListElement { name: "One"; code: 1 } ListElement { name: "Two"; code: 2 } } }
    ComboBox { id: e; x: 10; y: 300; editable: true; model: ["a", "b"] }
    TestCase { name: "Combo"; when: windowShown
        function flush(n) { console.log(n + ": " + log.join(", ")); log = [] }
        function test_combo() {
            console.log("defaults", c.count, c.currentIndex, c.currentText, c.displayText, c.highlightedIndex, c.textRole, c.valueRole, c.currentValue, c.editable, c.flat, c.down, c.pressed, c.focusPolicy, c.implicitWidth > 0, c.height, c.padding, c.leftPadding, c.rightPadding, c.indicator.x, c.indicator.y, c.indicator.width, c.indicator.height)
            console.log("bg", c.background.color, c.background.width, c.background.height, "content", c.contentItem.x, c.contentItem.width, c.contentItem.leftPadding, c.contentItem.text, c.contentItem.enabled, c.contentItem.color)
            console.log("model", m.currentText, m.currentValue, m.displayText, m.indexOfValue(2), m.textAt(1), m.valueAt(1), m.find("Two"))
            mouseClick(c, 50, 20); flush("click")
            let p = c.popup
            console.log("popup", p.visible, p.x, p.y, p.width, p.height, p.topMargin, p.padding, p.contentItem.count, c.highlightedIndex, p.contentItem.currentIndex)
            let d = p.contentItem.itemAtIndex(1)
            console.log("delegate", d.width, d.height, d.text, d.highlighted, d.font.weight, p.contentItem.itemAtIndex(0).font.weight)
            mouseClick(d, 10, 10); flush("pick 1")
            console.log("after", c.currentIndex, c.currentText, c.displayText, p.visible)
            c.forceActiveFocus(); keyClick(Qt.Key_Down); flush("down"); keyClick(Qt.Key_Up); flush("up")
            keyClick(Qt.Key_Space); flush("space"); console.log("space popup", p.visible, c.highlightedIndex)
            keyClick(Qt.Key_Down); flush("down in popup"); keyClick(Qt.Key_Return); flush("return"); console.log("after return", c.currentIndex, p.visible)
            console.log("editable", e.editText, e.displayText, e.contentItem.enabled, e.contentItem.readOnly)
        }
    }
}
