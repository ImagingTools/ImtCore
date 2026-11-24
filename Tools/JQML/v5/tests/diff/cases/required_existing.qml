import QtQuick

// `required name`: a property a type has already (here of a component of the file) is required; a delegate gets the role
Item {
    component Card: Item { property string subtitle: "none"; property string title: "t" }
    ListModel { id: m; ListElement { subtitle: "one"; accent: "red" } ListElement { subtitle: "two"; accent: "blue" } }
    Repeater {
        id: rep
        model: m
        delegate: Card {
            required subtitle
            required property color accent
            title: subtitle + "/" + accent
        }
    }
    Component.onCompleted: { console.log(rep.itemAt(0).title, rep.itemAt(1).title); Qt.quit() }
}
