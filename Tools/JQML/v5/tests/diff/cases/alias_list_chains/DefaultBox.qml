import QtQuick
Item {
    id: box
    property Item slot: Item { parent: box }
    property alias content: box.slot.data
    default property alias forwarded: box.content
}
