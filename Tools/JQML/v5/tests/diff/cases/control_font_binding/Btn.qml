import QtQuick
import QtQuick.Templates as T

// the font of a control given whole to its content item (ButtonImpl of QmlAppTemplate: font: control.font)
T.Button {
    id: control
    font.pixelSize: 14
    font.bold: false
    contentItem: Text { text: control.text; font: control.font }
}
