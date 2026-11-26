import QtQuick
import QtQuick.Controls

// ComboBox without a textRole on a ListModel of one role and of two: the text of modelData
Item {
    ListModel { id: one; ListElement { color: "red" } ListElement { color: "blue" } }
    ListModel { id: two; ListElement { color: "red"; n: 1 } }
    ComboBox { id: a; model: one }
    ComboBox { id: b; model: two }
    Component.onCompleted: { console.log(a.currentText, a.textAt(1), "[" + b.currentText + "]"); Qt.quit() }
}
