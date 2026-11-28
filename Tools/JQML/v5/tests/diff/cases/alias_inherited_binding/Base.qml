import QtQuick

Item {
    id: root
    property alias size: leaf.width
    property alias metric: root.size

    Item { id: leaf; width: 5 }
}
