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
    property int touched: 0

    // The handler has a comment and a blank line: the lines of the packed script must stay the lines of this file.
    Component.onCompleted: {
        /* a comment
           of two lines */

        page.touched = Util.add(1, 1) // the statement the source map test looks for
    }

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
