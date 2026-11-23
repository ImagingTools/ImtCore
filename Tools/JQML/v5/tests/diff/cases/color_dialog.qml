import QtQuick
import QtQuick.Dialogs

// ColorDialog of QtQuick.Dialogs: its properties and enums (on the web the native color input picks the color)
Item {
    ColorDialog { id: d; title: "Pick" }
    ColorDialog { id: e; selectedColor: "#80ff0000"; options: ColorDialog.ShowAlphaChannel | ColorDialog.NoEyeDropperButton }
    Component.onCompleted: {
        console.log(d.selectedColor, d.options, d.title, d.visible, d.result, d.modality)
        console.log(e.selectedColor, e.options, ColorDialog.ShowAlphaChannel, ColorDialog.NoButtons, ColorDialog.DontUseNativeDialog, ColorDialog.NoEyeDropperButton)
        d.selectedColor = "red"
        console.log(d.selectedColor)
        Qt.quit()
    }
}
