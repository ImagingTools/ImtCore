import QtQuick
Item {
    id: box
    property Item slot: Item { parent: box }
    default property alias content: box.slot.data
}
