import QtQuick

Item {
    id: root

    property int baseCount: 2

    function flowTag() {
        return "flow:" + baseCount
    }
}
