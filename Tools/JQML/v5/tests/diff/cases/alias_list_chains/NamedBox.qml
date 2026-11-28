import QtQuick
Item {
    id: box
    property Item slot: Item { parent: box }
    property alias content: box.slot.data
    property alias forwarded: box.content
}
