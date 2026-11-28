import QtQuick
Item {
    id: box
    property list<Item> entries: [Item { objectName: "base" }]
    default property alias content: box.entries
}
