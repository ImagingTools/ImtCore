import QtQuick
Item {
    id: box
    property rect region: Qt.rect(1, 2, 3, 4)
    property alias fieldX: box.region.x
    property alias fieldY: box.region.y
}
