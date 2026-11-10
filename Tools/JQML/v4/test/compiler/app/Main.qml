import QtQuick 2.15
import Widgets 1.0

// The entry of the compiler test: a module type, a singleton, a JavaScript library of the qmldir,
// a component next to the entry file and a translated string.
Rectangle {
    id: page
    objectName: "page"
    color: Theme.background

    property string greeting: qsTr("Hello")
    property int sum: Util.add(2, 3)

    Widget {
        id: widget
        objectName: "widget"
        label: page.greeting
        anchors.centerIn: parent
    }

    Sibling {
        objectName: "sibling"
        anchors.bottom: parent.bottom
    }
}
