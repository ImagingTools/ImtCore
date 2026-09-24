import QtQuick
import QtQuick.Layouts
import QtQml.Models

Item {
    id: root
    width: 200
    height: 100

    property int foo: 1

    MyBox {
        id: box
        anchors.fill: parent
        Layout.preferredWidth: 80
        onTapped: root.foo = box.count
    }

    ListModel {
        ListElement {
            name: "a"
        }
    }
}
