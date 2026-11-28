import QtQuick
Item {
    id: root
    property alias plottedSize: label.font.pixelSize
    property bool observe: false
    onPlottedSizeChanged: {
        if (observe) console.log("size changed",plottedSize)
    }
    Text { id: label; font.pixelSize: 16 }
    Component.onCompleted: {
        observe = true
        label.font.bold = true
        label.font.pixelSize = 23
        console.log("size", plottedSize)
        Qt.quit()
    }
}
