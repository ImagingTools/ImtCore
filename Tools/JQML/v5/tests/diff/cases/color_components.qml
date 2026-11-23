import QtQuick

// the components of a color value: r g b a, hsv and hsl, valid (ButtonImpl of QmlAppTemplate: Qt.rgba(color.r, ...))
Item {
    property color theme: "#e8e8e8"
    property bool checked: false
    property color color: checked ? "#6c3ce0" : theme
    property color bg: Qt.rgba(color.r, color.g, color.b, checked ? 0.2 : 1)
    property color half: "#80ff8000"
    Component.onCompleted: {
        console.log(color, color.r, color.g, color.b, color.a, bg)
        checked = true
        console.log(bg, color.hsvHue, color.hsvSaturation, color.hsvValue, color.hslHue, color.hslSaturation, color.hslLightness, color.valid)
        console.log(half.a, half.r, half.g, half.b, half.hsvHue, half.hslLightness)
        let gray = Qt.rgba(0.5, 0.5, 0.5, 1)
        console.log(gray.hsvHue, gray.hslHue, gray.hsvSaturation)
        Qt.quit()
    }
}
