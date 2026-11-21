import QtQuick
import QtQuick.Controls
import QtTest

// SearchField: the defaults, typing (textEdited, searchTriggered when live, the popup with suggestions), the highlight
// by Up / Down / Home / End, Return taking the highlighted suggestion (activated, accepted, searchTriggered), Escape, a
// click on a suggestion, the clear and search indicators, a text equal to a suggestion makes it current
Item {
    width: 400; height: 400
    property var log: []
    SearchField { id: s; width: 250
        suggestionModel: ["apple", "apricot", "banana", "blueberry", "cherry"]
        onTextEdited: log.push("edited " + text)
        onSearchTriggered: log.push("search " + text)
        onAccepted: log.push("accepted")
        onActivated: (index) => log.push("activated " + index)
        onHighlighted: (index) => log.push("highlighted " + index)
        onSearchButtonPressed: log.push("search button")
        onClearButtonPressed: log.push("clear button")
    }
    SearchField { id: m; y: 300; live: false; textRole: "name"; suggestionModel: ListModel { ListElement { name: "one" } ListElement { name: "two" } } }
    function flush(n) { console.log(n + ": " + log.join(", ")); log = [] }
    function st(f) { return [f.text, f.currentIndex, f.highlightedIndex, f.popup.visible, f.suggestionCount].join(" ") }
    TestCase { name: "Search"; when: windowShown
        function test_a_defaults() {
            console.log("defaults", s.live, m.live, s.currentIndex, s.highlightedIndex, s.suggestionCount, m.suggestionCount, s.selectTextByMouse, s.focusPolicy, s.textRole, s.searchIndicator.indicator !== null, s.clearIndicator.indicator.visible, s.contentItem.activeFocus)
            console.log("indicators", s.searchIndicator.indicator.x, s.searchIndicator.indicator.width, s.clearIndicator.indicator.x, s.leftPadding, s.rightPadding)
        }
        function test_b_typing() {
            s.forceActiveFocus(); console.log("focus", s.activeFocus, s.contentItem.activeFocus)
            keyClick(Qt.Key_A); flush("a"); console.log(st(s))
            keyClick(Qt.Key_P); flush("p"); console.log(st(s), s.clearIndicator.indicator.visible, s.rightPadding)
            keyClick(Qt.Key_Down); flush("down"); keyClick(Qt.Key_Down); flush("down"); keyClick(Qt.Key_Up); keyClick(Qt.Key_Up); flush("up up"); console.log(st(s))
            keyClick(Qt.Key_End); flush("end"); keyClick(Qt.Key_Home); flush("home")
            keyClick(Qt.Key_Down); keyClick(Qt.Key_Return); flush("return"); console.log(st(s))
            keyClick(Qt.Key_Escape); flush("escape"); console.log(st(s))
        }
        function test_c_click() {
            s.forceActiveFocus(); keyClick(Qt.Key_B); wait(50); flush("b"); console.log(st(s))
            let view = s.popup.contentItem
            mouseClick(view.itemAtIndex(1), 10, 10); wait(50); flush("click second"); console.log(st(s))
            mouseClick(s, s.width - 10, s.height / 2); flush("clear"); console.log(st(s))
            mouseClick(s, 10, s.height / 2); flush("search button"); console.log(st(s), s.contentItem.activeFocus)
            s.text = "cherry"; wait(50); console.log("set text", st(s)); s.popup.close(); wait(50)
        }
        function test_d_not_live() {
            m.forceActiveFocus(); keyClick(Qt.Key_T); console.log("m", st(m)); keyClick(Qt.Key_Return); console.log("m return", st(m))
        }
    }
}
