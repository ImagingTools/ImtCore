import QtQuick

Item {
    id: root
    property alias tag: container.tag

    Item {
        id: container
        property string tag: ""
    }
}
