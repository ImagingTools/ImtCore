import QtQuick

Item {
    id: item
    property real radius: 80
    property alias outerRadius: item.radius

    Component.onCompleted: {
        outerRadius = 55
        console.log("radius =", radius)
        console.log("outerRadius =", outerRadius)
        Qt.quit();
    }
}