import QtQuick

Item {
    id: root
    width: 120
    height: 40
    property int count: 0

    signal tapped()

    function bump() {
        root.count = root.count + 1
    }
}
