import QtQuick

BaseRow {
    id: root

    property int extraSpacing: 3

    function totalSpacing() {
        return rowSize() + extraSpacing
    }
}
