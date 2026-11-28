import QtQuick
Item {
    id: root
    property rect region: Qt.rect(1, 2, 3, 4)
    property alias regionX: root.region.x
    property alias fontSize: label.font.pixelSize
    onRegionXChanged: console.log("rect changed",regionX)
    onFontSizeChanged: console.log("font changed",fontSize)
    Text { id: label; font.pixelSize: 16 }
    Component.onCompleted: {
        console.log("initial", regionX, fontSize)
        regionX = 5
        console.log("write", region.x)
        region = Qt.rect(5, 6, 7, 8)
        fontSize = 23
        console.log("font", label.font.pixelSize)
        Qt.quit()
    }
}
