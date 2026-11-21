import QtQuick

// a delegate of a type's view reads an id of the document the type is used in (ScreenAbout of QmlAppTemplate:
// appWindow.width in a Repeater delegate) and a role named like nothing else
Column {
    Repeater {
        model: ListModel { ListElement { label: "a" } ListElement { label: "b" } }
        delegate: Text { text: label + " " + (appWindow.width > 100) + " " + appWindow.objectName }
    }
}
